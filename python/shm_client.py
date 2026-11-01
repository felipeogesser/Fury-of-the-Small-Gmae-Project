import ctypes
import gc
import os
import time
from dataclasses import dataclass
from multiprocessing import shared_memory

import numpy as np

import shm_layout as layout
from config import MAX_BATTALIONS


def open_shared_memory(name):
    try:
        return shared_memory.SharedMemory(name=name, create=False, track=False)
    except TypeError:
        shm = shared_memory.SharedMemory(name=name, create=False)
        try:
            from multiprocessing import resource_tracker
            resource_tracker.unregister(shm._name, "shared_memory")
        except Exception:
            pass
        return shm


@dataclass
class BattleInfo:
    episode_index: int
    battalion_count: int
    left: float
    top: float
    right: float
    bottom: float
    width: float
    height: float
    tick_dt: float
    ticks_per_decision: int
    max_ticks: int


class SharedMemoryClient:

    def __init__(self, name, timeout=10.0):
        self.shm = open_shared_memory(name)
        self.buf = self.shm.buf
        self.readonly = self.buf.toreadonly()
        self.size = self.shm.size
        self.control = layout.SharedMemoryControl.from_buffer(self.buf)
        self._wait_for_layout(timeout)
        header = self.control.header
        self.c_pid = int(header.c_pid)
        self.span_offset = int(header.span_offset)
        self.span_size = int(header.span_size)
        self.order_offset = int(header.order_offset)
        self.spans = layout.BattalionTableSpan.from_buffer(self.buf, self.span_offset)
        self.orders = [self._build_order_views(army) for army in range(2)]
        self.static = [None, None]
        self.dynamic = [None, None]
        self._span_bytes = None

    def _wait_for_layout(self, timeout):
        deadline = time.monotonic() + timeout
        while True:
            header = self.control.header
            if header.magic == layout.SHARED_MEMORY_MAGIC and self.control.layout_ready.value == 1:
                break
            if time.monotonic() > deadline:
                raise TimeoutError("C side did not publish the shared memory layout")
            time.sleep(0.001)
        if header.control_size != ctypes.sizeof(layout.SharedMemoryControl):
            raise RuntimeError("control block size mismatch")
        if header.span_size != ctypes.sizeof(layout.BattalionTableSpan):
            raise RuntimeError("span block size mismatch")
        if header.order_size != ctypes.sizeof(layout.OrderBuffer):
            raise RuntimeError("order buffer size mismatch")
        if header.span_offset + header.span_size > self.size or header.order_offset + header.order_size > self.size:
            raise RuntimeError("span block or order buffer outside of shared memory")

    def _build_order_views(self, army):
        base = self.order_offset + army * ctypes.sizeof(layout.OrderBufferArmy)
        views = {}
        for name, dtype in layout.ORDER_FIELDS:
            offset = base + getattr(layout.OrderBufferArmy, name).offset
            views[name] = np.ndarray((MAX_BATTALIONS,), dtype=dtype, buffer=self.buf, offset=offset)
        return views

    def _build_group(self, group_span, fields):
        views = {}
        for name, dtype in fields:
            span = getattr(group_span, name)
            offset = int(span.offset)
            size = int(span.size)
            itemsize = np.dtype(dtype).itemsize
            if size % itemsize != 0 or offset + size > self.size:
                raise RuntimeError(f"invalid span for {name}")
            views[name] = np.frombuffer(self.readonly, dtype=dtype, count=size // itemsize, offset=offset)
        return views

    def refresh_battle(self):
        span_bytes = bytes(self.readonly[self.span_offset:self.span_offset + self.span_size])
        if span_bytes != self._span_bytes:
            self.static = [
                self._build_group(self.spans.friendly_static, layout.STATIC_FIELDS),
                self._build_group(self.spans.enemy_static, layout.STATIC_FIELDS),
            ]
            self.dynamic = [
                self._build_group(self.spans.friendly_dynamic, layout.DYNAMIC_FIELDS),
                self._build_group(self.spans.enemy_dynamic, layout.DYNAMIC_FIELDS),
            ]
            self._span_bytes = span_bytes
        count = int(self.control.battalion_count.value)
        if count > MAX_BATTALIONS:
            raise RuntimeError(f"battalion count {count} exceeds {MAX_BATTALIONS}")
        for group in self.static + self.dynamic:
            for name, view in group.items():
                if view.shape[0] < count:
                    raise RuntimeError(f"span for {name} is shorter than the battalion count")
        map_info = self.control.map_info
        timing_info = self.control.timing_info
        return BattleInfo(
            episode_index=int(self.control.episode_index.value),
            battalion_count=count,
            left=float(map_info.left),
            top=float(map_info.top),
            right=float(map_info.right),
            bottom=float(map_info.bottom),
            width=max(float(map_info.width), 1.0),
            height=max(float(map_info.height), 1.0),
            tick_dt=float(timing_info.tick_dt),
            ticks_per_decision=max(int(timing_info.ticks_per_decision), 1),
            max_ticks=max(int(timing_info.max_ticks), 1),
        )

    def c_alive(self):
        try:
            os.kill(self.c_pid, 0)
        except ProcessLookupError:
            return False
        except PermissionError:
            return True
        return True

    def c_shutdown(self):
        return self.control.c_shutdown.value != 0

    def battle_active(self):
        return self.control.battle_active.value != 0

    def episode_index(self):
        return int(self.control.episode_index.value)

    def episode_result(self):
        value = int(self.control.episode_result.value)
        try:
            return layout.EpisodeResult(value)
        except ValueError:
            return layout.EpisodeResult.DRAW

    def wait_for_step(self, should_stop):
        spins = 0
        while True:
            if self.control.c_shutdown.value != 0 or should_stop():
                return None
            step = self.control.c_step.value
            if step != self.control.py_step.value:
                return step
            if self.control.battle_active.value == 0:
                return None
            spins += 1
            if spins < 2000:
                continue
            if spins % 2000 == 0 and not self.c_alive():
                return None
            time.sleep(0.0001)

    def set_py_step(self, step):
        self.control.py_step.value = step

    def set_ready(self):
        self.control.py_pid.value = os.getpid()
        self.control.py_ready.value = 1

    def set_shutdown(self):
        if self.control is not None:
            self.control.py_shutdown.value = 1

    def close(self):
        self.static = [None, None]
        self.dynamic = [None, None]
        self.orders = None
        self.spans = None
        self.control = None
        gc.collect()
        try:
            self.readonly.release()
        except BufferError:
            pass
        try:
            self.shm.close()
        except BufferError:
            pass