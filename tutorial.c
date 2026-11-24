#include "tutorial.h"
#include "battlefield_hud.h"
#include "battle_controls.h"
#include "battleplan.h"
#include "battleplan_inspect.h"
#include "engine_internal.h"
#include "help_overlay.h"
#include "inventory.h"
#include "run.h"
#include "stage_map.h"
#include "tutorial_dialog.h"
#include "tutorial_script.h"
#include <stdbool.h>
#include <string.h>

#define TUTORIAL_ATTEMPTS 3
#define TUTORIAL_TYPE_RATE 70.0f
#define TUTORIAL_MAX_FRAME_DT 0.1f

enum TutorialOutcome {
    TUTORIAL_OUTCOME_NONE,
    TUTORIAL_OUTCOME_LOST,
    TUTORIAL_OUTCOME_WON,
    TUTORIAL_OUTCOME_SPARED
};

typedef struct Tutorial {

    _Bool is_pending;
    unsigned int attempts;
    enum TutorialOutcome outcome;
    enum TutorialChapter chapter;
    unsigned int step;
    float typed;
    const char *reply;
    Uint32 last_ticks;
    _Bool swallows_release;

} Tutorial;

static Tutorial tutorial = {true, 0, TUTORIAL_OUTCOME_NONE, TUTORIAL_CHAPTER_NONE, 0, 0.0f, NULL, 0, false};

_Bool tutorial_is_finished(void) {

    return !tutorial.is_pending;

}

void tutorial_set_finished(_Bool is_finished) {

    tutorial.is_pending = !is_finished;

}

static const TutorialStep *current_step(void) {

    const TutorialChapterScript *script = tutorial_chapter_script(tutorial.chapter);
    if (script == NULL || tutorial.step >= script->step_count) return NULL;
    return &script->steps[tutorial.step];

}

static _Bool step_reads(const TutorialStep *step) {

    return tutorial.reply != NULL || step->wait == TUTORIAL_WAIT_READ || step->wait == TUTORIAL_WAIT_CHOICE;

}

static void start_chapter(enum TutorialChapter chapter) {

    tutorial.chapter = chapter;
    tutorial.step = 0;
    tutorial.typed = 0.0f;
    tutorial.reply = NULL;

}

static void finish_tutorial(void) {

    tutorial.is_pending = false;
    inventory_set_starting_army(STARTING_ARMY_TRAINING);
    run_new();

}

static void advance(void) {

    const TutorialChapterScript *script = tutorial_chapter_script(tutorial.chapter);
    tutorial.typed = 0.0f;
    tutorial.reply = NULL;
    tutorial.step++;

    if (script != NULL && tutorial.step < script->step_count) return;

    tutorial.chapter = TUTORIAL_CHAPTER_NONE;
    if (script != NULL && script->ends_tutorial) finish_tutorial();

}

static _Bool is_wait_done(enum TutorialWait wait) {

    _Bool is_enemy = false;
    unsigned int index = 0;

    switch (wait) {
        case TUTORIAL_WAIT_DRAWER_OPEN: return battleplan_is_drawer_open();
        case TUTORIAL_WAIT_GENERAL_PLACED: return battleplan_deployed_count() > 0;
        case TUTORIAL_WAIT_GENERAL_INSPECTED: return battleplan_inspect_is_open();
        case TUTORIAL_WAIT_ARMY_READY: return battleplan_deployed_count() == battleplan_required_count();
        case TUTORIAL_WAIT_ENEMY_CARDS: return battlefield_hud_is_showing_enemy();
        case TUTORIAL_WAIT_CARD_SELECTED: return battlefield_hud_selected_battalion(&is_enemy, &index);
        case TUTORIAL_WAIT_HELP_OPENED: return help_overlay_is_open();
        default: return false;
    }

}

static SDL_Rect target_rect(enum TutorialTarget target) {

    switch (target) {
        case TUTORIAL_TARGET_STAGE_CURRENT: return stage_map_current_rect();
        case TUTORIAL_TARGET_PLAN_PLAYER_GRID: return battleplan_area(BATTLEPLAN_AREA_PLAYER_GRID);
        case TUTORIAL_TARGET_PLAN_ENEMY_GRID: return battleplan_area(BATTLEPLAN_AREA_ENEMY_GRID);
        case TUTORIAL_TARGET_PLAN_PLAYER_HEADER: return battleplan_area(BATTLEPLAN_AREA_PLAYER_HEADER);
        case TUTORIAL_TARGET_PLAN_DRAWER_HANDLE: return battleplan_area(BATTLEPLAN_AREA_DRAWER_HANDLE);
        case TUTORIAL_TARGET_PLAN_DRAWER: return battleplan_area(BATTLEPLAN_AREA_DRAWER);
        case TUTORIAL_TARGET_PLAN_START_BUTTON: return battleplan_area(BATTLEPLAN_AREA_START_BUTTON);
        case TUTORIAL_TARGET_PLAN_STATS: return battleplan_inspect_is_open() ? battleplan_inspect_rect() : (SDL_Rect){0, 0, 0, 0};
        case TUTORIAL_TARGET_FIELD_POWER_BAR: return battlefield_hud_area(BATTLEFIELD_HUD_POWER_BAR);
        case TUTORIAL_TARGET_FIELD_CARDS: return battlefield_hud_area(BATTLEFIELD_HUD_CARDS);
        case TUTORIAL_TARGET_FIELD_TABS: return battlefield_hud_area(BATTLEFIELD_HUD_TABS);
        case TUTORIAL_TARGET_FIELD_DETAIL: return battlefield_hud_area(BATTLEFIELD_HUD_DETAIL);
        case TUTORIAL_TARGET_FIELD_CONTROLS: return battle_controls_rect();
        default: return (SDL_Rect){0, 0, 0, 0};
    }

}

static TutorialDialogView build_view(const TutorialStep *step) {

    TutorialDialogView view = {0};
    view.speaker = tutorial_speaker;
    view.text = tutorial.reply != NULL ? tutorial.reply : step->text;
    view.typed = tutorial.typed;
    view.place = step->place;
    view.target = tutorial.reply != NULL ? (SDL_Rect){0, 0, 0, 0} : target_rect(step->target);
    view.dims_screen = step_reads(step);
    view.footer = step_reads(step) ? "Click or press Space to continue" : "Go ahead";

    if (tutorial.reply == NULL && step->wait == TUTORIAL_WAIT_CHOICE) {
        for (unsigned int i = 0; i < TUTORIAL_DIALOG_CHOICES; i++) {
            if (step->choices[i].label == NULL) continue;
            view.choices[view.choice_count++] = step->choices[i].label;
        }
    }

    return view;

}

static _Bool is_leaving(enum Scene scene, const SDL_Event *e) {

    if (scene != STAGE_MAP) return false;
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) return true;
    if (e->type != SDL_MOUSEBUTTONDOWN && e->type != SDL_MOUSEBUTTONUP) return false;
    const SDL_Point point = {e->button.x, e->button.y};
    const SDL_Rect back = stage_map_back_rect();
    return SDL_PointInRect(&point, &back);

}

static void choose(const TutorialStep *step, unsigned int choice) {

    if (choice >= TUTORIAL_DIALOG_CHOICES || step->choices[choice].label == NULL) return;
    tutorial.reply = step->choices[choice].reply;
    tutorial.typed = 0.0f;
    if (tutorial.reply == NULL) advance();

}

static void press(const TutorialStep *step, const TutorialDialogView *view, int x, int y, _Bool is_mouse) {

    if (!tutorial_dialog_is_typed(view)) {
        tutorial.typed = (float)strlen(view->text);
        return;
    }

    if (tutorial.reply == NULL && step->wait == TUTORIAL_WAIT_CHOICE) {
        if (!is_mouse) return;
        TutorialDialogLayout layout;
        tutorial_dialog_layout(view, &layout);
        const SDL_Point point = {x, y};
        for (unsigned int i = 0; i < view->choice_count; i++) {
            if (SDL_PointInRect(&point, &layout.choices[i])) choose(step, i);
        }
        return;
    }

    advance();

}

_Bool tutorial_is_active(void) {

    return tutorial.is_pending && !engine.has_program_been_initialized_with_ai_bypass;

}

void tutorial_scene_entered(enum Scene scene) {

    tutorial.chapter = TUTORIAL_CHAPTER_NONE;
    tutorial.swallows_release = false;
    if (scene == BATTLEPLAN) battleplan_lock_exit(tutorial_is_active());
    if (!tutorial_is_active()) return;

    if (scene == STAGE_MAP) {
        const enum TutorialOutcome outcome = tutorial.outcome;
        tutorial.outcome = TUTORIAL_OUTCOME_NONE;
        if (outcome == TUTORIAL_OUTCOME_WON) start_chapter(TUTORIAL_CHAPTER_FAREWELL_WON);
        else if (outcome == TUTORIAL_OUTCOME_SPARED) start_chapter(TUTORIAL_CHAPTER_FAREWELL_SPARED);
        else if (outcome == TUTORIAL_OUTCOME_LOST) start_chapter(tutorial.attempts <= 1 ? TUTORIAL_CHAPTER_FIRST_LOSS : TUTORIAL_CHAPTER_SECOND_LOSS);
        else start_chapter(TUTORIAL_CHAPTER_STAGE_INTRO);
    }

    if (scene == BATTLEPLAN) start_chapter(tutorial.attempts == 0 ? TUTORIAL_CHAPTER_DEPLOY : TUTORIAL_CHAPTER_DEPLOY_RETRY);
    if (scene == BATTLEFIELD) start_chapter(tutorial.attempts == 0 ? TUTORIAL_CHAPTER_BATTLE_INTRO : TUTORIAL_CHAPTER_BATTLE_RETRY);

}

_Bool tutorial_input(enum Scene scene, const SDL_Event *e) {

    if (!tutorial_is_active()) return false;
    if (is_leaving(scene, e)) return true;

    if (e->type == SDL_MOUSEBUTTONUP && tutorial.swallows_release) {
        tutorial.swallows_release = false;
        return true;
    }

    const TutorialStep *step = current_step();
    if (step == NULL || !step_reads(step)) return false;

    const TutorialDialogView view = build_view(step);

    if (e->type == SDL_MOUSEBUTTONDOWN) {
        tutorial.swallows_release = true;
        if (e->button.button == SDL_BUTTON_LEFT) press(step, &view, e->button.x, e->button.y, true);
        return true;
    }

    if (e->type == SDL_KEYDOWN) {
        const SDL_Keycode key = e->key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_RETURN || key == SDLK_KP_ENTER) press(step, &view, 0, 0, false);
        if (tutorial.reply == NULL && step->wait == TUTORIAL_WAIT_CHOICE && tutorial_dialog_is_typed(&view)) {
            if (key == SDLK_1) choose(step, 0);
            if (key == SDLK_2) choose(step, 1);
        }
        return true;
    }

    return e->type == SDL_MOUSEWHEEL;

}

void tutorial_render(enum Scene scene) {

    (void)scene;

    const Uint32 now = SDL_GetTicks();
    float dt = tutorial.last_ticks == 0 ? 0.0f : (float)(now - tutorial.last_ticks) / 1000.0f;
    if (dt > TUTORIAL_MAX_FRAME_DT) dt = TUTORIAL_MAX_FRAME_DT;
    tutorial.last_ticks = now;

    if (!tutorial_is_active()) return;

    const TutorialStep *step = current_step();
    if (step == NULL) return;

    while (!step_reads(step) && is_wait_done(step->wait)) {
        advance();
        step = current_step();
        if (step == NULL) return;
    }

    if (help_overlay_is_open()) return;

    tutorial.typed += dt * TUTORIAL_TYPE_RATE;
    TutorialDialogView view = build_view(step);
    const float length = (float)strlen(view.text);
    if (tutorial.typed > length) tutorial.typed = length;
    view.typed = tutorial.typed;

    TutorialDialogLayout layout;
    tutorial_dialog_layout(&view, &layout);
    tutorial_dialog_render(&view, &layout);

}

_Bool tutorial_holds_battle(void) {

    if (!tutorial_is_active()) return false;
    const TutorialChapterScript *script = tutorial_chapter_script(tutorial.chapter);
    return script != NULL && script->holds_battle;

}

_Bool tutorial_record_battle(enum EpisodeResult result) {

    if (!tutorial_is_active()) return false;

    tutorial.attempts++;
    if (result == EPISODE_PLAYER_WON) tutorial.outcome = TUTORIAL_OUTCOME_WON;
    else if (tutorial.attempts >= TUTORIAL_ATTEMPTS) tutorial.outcome = TUTORIAL_OUTCOME_SPARED;
    else tutorial.outcome = TUTORIAL_OUTCOME_LOST;
    return true;

}
