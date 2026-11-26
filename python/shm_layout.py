import ctypes
from enum import IntEnum

import numpy as np

from config import MAX_BATTALIONS

SHARED_MEMORY_MAGIC = 0x53484D41
SHARED_MEMORY_CONTROL_SIZE = 1024
CACHE_LINE = 64


class EpisodeResult(IntEnum):
    RUNNING = 0
    PLAYER_WON = 1
    ENEMY_WON = 2
    DRAW = 3
    TIMEOUT = 4


STATIC_FIELDS = [
    ("battalion_type", np.int32),
    ("range", np.float32),
    ("charge_bonus", np.float32),
    ("armor", np.float32),
    ("max_hp", np.float32),
    ("attack_speed", np.float32),
    ("start_entity_count", np.uint32),
    ("formation_x", np.uint8),
    ("formation_y", np.uint8),
    ("is_shielded", np.bool_),
    ("is_ranged", np.bool_),
    ("is_large", np.bool_),
    ("is_armor_piercing", np.bool_),
    ("is_anti_large", np.bool_),
]

DYNAMIC_FIELDS = [
    ("headless_movement_mode", np.int32),
    ("fatigue_state", np.int32),
    ("morale_state", np.int32),
    ("lifecycle_state", np.int32),
    ("movement_state", np.int32),
    ("movement_pace", np.int32),
    ("flank_exposure", np.int32),
    ("flank_attack", np.int32),
    ("target_battalion_idx", np.uint32),
    ("morale_modifiers", np.uint32),
    ("withdrawed_entity_count", np.uint32),
    ("dead_entity_count", np.uint32),
    ("alive_entity_count", np.uint32),
    ("unit_alive_count", np.uint32),
    ("target_x", np.float32),
    ("target_y", np.float32),
    ("x", np.float32),
    ("y", np.float32),
    ("morale", np.float32),
    ("fatigue", np.float32),
    ("hp", np.float32),
    ("speed", np.float32),
    ("meele_attack", np.float32),
    ("meele_defense", np.float32),
    ("strength", np.float32),
    ("rally_timer", np.float32),
    ("charge_timer", np.float32),
    ("facing", np.float32),
    ("is_active", np.bool_),
    ("is_rallying", np.bool_),
    ("is_charging", np.bool_),
    ("is_engaging", np.bool_),
    ("is_engaged", np.bool_),
    ("is_controllable", np.bool_),
    ("is_present", np.bool_),
    ("is_headless", np.bool_),
    ("is_bracing", np.bool_),
]

ORDER_FIELDS = [
    ("type", np.int32),
    ("pace", np.int32),
    ("target", np.uint32),
    ("dest_x", np.float32),
    ("dest_y", np.float32),
]


class SignalLine(ctypes.Structure):
    _fields_ = [
        ("value", ctypes.c_uint64),
        ("_pad", ctypes.c_uint8 * 56),
    ]


class SharedMemoryHeader(ctypes.Structure):
    _fields_ = [
        ("magic", ctypes.c_uint32),
        ("version", ctypes.c_uint32),
        ("control_size", ctypes.c_uint64),
        ("span_offset", ctypes.c_uint64),
        ("span_size", ctypes.c_uint64),
        ("order_offset", ctypes.c_uint64),
        ("order_size", ctypes.c_uint64),
        ("c_pid", ctypes.c_uint64),
        ("_pad", ctypes.c_uint8 * 8),
    ]


class SharedMemoryMapInfo(ctypes.Structure):
    _fields_ = [
        ("left", ctypes.c_float),
        ("top", ctypes.c_float),
        ("right", ctypes.c_float),
        ("bottom", ctypes.c_float),
        ("width", ctypes.c_float),
        ("height", ctypes.c_float),
        ("_pad", ctypes.c_uint8 * 40),
    ]


class SharedMemoryTimingInfo(ctypes.Structure):
    _fields_ = [
        ("tick_dt", ctypes.c_float),
        ("ticks_per_decision", ctypes.c_uint32),
        ("max_ticks", ctypes.c_uint64),
        ("_pad", ctypes.c_uint8 * 48),
    ]


class SharedMemoryControl(ctypes.Structure):
    _fields_ = [
        ("header", SharedMemoryHeader),
        ("layout_ready", SignalLine),
        ("c_step", SignalLine),
        ("py_step", SignalLine),
        ("episode_result", SignalLine),
        ("episode_index", SignalLine),
        ("battalion_count", SignalLine),
        ("battle_active", SignalLine),
        ("c_shutdown", SignalLine),
        ("py_shutdown", SignalLine),
        ("py_ready", SignalLine),
        ("py_pid", SignalLine),
        ("map_info", SharedMemoryMapInfo),
        ("timing_info", SharedMemoryTimingInfo),
        ("_reserved", ctypes.c_uint8 * (SHARED_MEMORY_CONTROL_SIZE - 14 * CACHE_LINE)),
    ]


class Span(ctypes.Structure):
    _fields_ = [
        ("offset", ctypes.c_uint64),
        ("size", ctypes.c_uint64),
    ]


def _make_span_struct(name, fields):
    return type(name, (ctypes.Structure,), {"_fields_": [(field_name, Span) for field_name, _ in fields]})


BattalionStaticTableSpan = _make_span_struct("BattalionStaticTableSpan", STATIC_FIELDS)
BattalionDynamicTableSpan = _make_span_struct("BattalionDynamicTableSpan", DYNAMIC_FIELDS)


class BattalionTableSpan(ctypes.Structure):
    _fields_ = [
        ("friendly_static", BattalionStaticTableSpan),
        ("enemy_static", BattalionStaticTableSpan),
        ("friendly_dynamic", BattalionDynamicTableSpan),
        ("enemy_dynamic", BattalionDynamicTableSpan),
    ]


class OrderBufferArmy(ctypes.Structure):
    _fields_ = [
        ("type", ctypes.c_int32 * MAX_BATTALIONS),
        ("pace", ctypes.c_int32 * MAX_BATTALIONS),
        ("target", ctypes.c_uint32 * MAX_BATTALIONS),
        ("dest_x", ctypes.c_float * MAX_BATTALIONS),
        ("dest_y", ctypes.c_float * MAX_BATTALIONS),
    ]


class OrderBuffer(ctypes.Structure):
    _fields_ = [
        ("army", OrderBufferArmy * 2),
    ]


def _require(condition, message):
    if not condition:
        raise RuntimeError(f"shared memory layout mismatch: {message}")


def _check_layout():
    _require(ctypes.sizeof(SignalLine) == 64, "SignalLine size")
    _require(ctypes.sizeof(SharedMemoryHeader) == 64, "SharedMemoryHeader size")
    _require(SharedMemoryHeader.c_pid.offset == 48, "c_pid offset")
    _require(ctypes.sizeof(SharedMemoryMapInfo) == 64, "SharedMemoryMapInfo size")
    _require(ctypes.sizeof(SharedMemoryTimingInfo) == 64, "SharedMemoryTimingInfo size")
    _require(ctypes.sizeof(SharedMemoryControl) == SHARED_MEMORY_CONTROL_SIZE, "SharedMemoryControl size")
    expected_offsets = {
        "layout_ready": 64,
        "c_step": 128,
        "py_step": 192,
        "episode_result": 256,
        "episode_index": 320,
        "battalion_count": 384,
        "battle_active": 448,
        "c_shutdown": 512,
        "py_shutdown": 576,
        "py_ready": 640,
        "py_pid": 704,
        "map_info": 768,
        "timing_info": 832,
    }
    for name, offset in expected_offsets.items():
        _require(getattr(SharedMemoryControl, name).offset == offset, f"{name} offset")
    _require(ctypes.sizeof(BattalionStaticTableSpan) == 14 * 16, "BattalionStaticTableSpan size")
    _require(ctypes.sizeof(BattalionDynamicTableSpan) == 37 * 16, "BattalionDynamicTableSpan size")
    _require(ctypes.sizeof(BattalionTableSpan) == (2 * 14 + 2 * 37) * 16, "BattalionTableSpan size")
    _require(ctypes.sizeof(OrderBufferArmy) == 5 * 4 * MAX_BATTALIONS, "OrderBufferArmy size")
    _require(ctypes.sizeof(OrderBuffer) == 2 * 5 * 4 * MAX_BATTALIONS, "OrderBuffer size")


_check_layout()