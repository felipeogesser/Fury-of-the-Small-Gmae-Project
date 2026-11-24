#ifndef TUTORIAL_SCRIPT_H
#define TUTORIAL_SCRIPT_H

#include "tutorial_dialog.h"

enum TutorialChapter {
    TUTORIAL_CHAPTER_NONE = -1,
    TUTORIAL_CHAPTER_STAGE_INTRO,
    TUTORIAL_CHAPTER_DEPLOY,
    TUTORIAL_CHAPTER_DEPLOY_RETRY,
    TUTORIAL_CHAPTER_BATTLE_INTRO,
    TUTORIAL_CHAPTER_BATTLE_RETRY,
    TUTORIAL_CHAPTER_FIRST_LOSS,
    TUTORIAL_CHAPTER_SECOND_LOSS,
    TUTORIAL_CHAPTER_FAREWELL_WON,
    TUTORIAL_CHAPTER_FAREWELL_SPARED,
    TUTORIAL_CHAPTER_COUNT
};

enum TutorialWait {
    TUTORIAL_WAIT_READ,
    TUTORIAL_WAIT_CHOICE,
    TUTORIAL_WAIT_SCENE_CHANGE,
    TUTORIAL_WAIT_DRAWER_OPEN,
    TUTORIAL_WAIT_GENERAL_PLACED,
    TUTORIAL_WAIT_GENERAL_INSPECTED,
    TUTORIAL_WAIT_ARMY_READY,
    TUTORIAL_WAIT_ENEMY_CARDS,
    TUTORIAL_WAIT_CARD_SELECTED,
    TUTORIAL_WAIT_HELP_OPENED
};

enum TutorialTarget {
    TUTORIAL_TARGET_NONE,
    TUTORIAL_TARGET_STAGE_CURRENT,
    TUTORIAL_TARGET_PLAN_PLAYER_GRID,
    TUTORIAL_TARGET_PLAN_ENEMY_GRID,
    TUTORIAL_TARGET_PLAN_PLAYER_HEADER,
    TUTORIAL_TARGET_PLAN_DRAWER_HANDLE,
    TUTORIAL_TARGET_PLAN_DRAWER,
    TUTORIAL_TARGET_PLAN_START_BUTTON,
    TUTORIAL_TARGET_PLAN_STATS,
    TUTORIAL_TARGET_FIELD_POWER_BAR,
    TUTORIAL_TARGET_FIELD_CARDS,
    TUTORIAL_TARGET_FIELD_TABS,
    TUTORIAL_TARGET_FIELD_DETAIL,
    TUTORIAL_TARGET_FIELD_CONTROLS
};

typedef struct TutorialChoice {

    const char *label;
    const char *reply;

} TutorialChoice;

typedef struct TutorialStep {

    enum TutorialWait wait;
    enum TutorialTarget target;
    enum TutorialBoxPlace place;
    const char *text;
    TutorialChoice choices[TUTORIAL_DIALOG_CHOICES];

} TutorialStep;

typedef struct TutorialChapterScript {

    const TutorialStep *steps;
    unsigned int step_count;
    _Bool holds_battle;
    _Bool ends_tutorial;

} TutorialChapterScript;

extern const char *const tutorial_speaker;

const TutorialChapterScript *tutorial_chapter_script(enum TutorialChapter chapter);

#endif
