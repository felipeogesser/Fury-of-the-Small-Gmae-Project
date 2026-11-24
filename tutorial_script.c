#include "tutorial_script.h"
#include <stdbool.h>

#define STEP_COUNT(steps) (sizeof(steps) / sizeof((steps)[0]))

const char *const tutorial_speaker = "Sergeant Aldric, Guardsmen";

static const TutorialStep stage_intro[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Welcome, commander. I'm Sergeant Aldric of the Guardsmen. Before you lead a real campaign, I'll walk you through one training battle.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_STAGE_CURRENT, TUTORIAL_BOX_TOP,
        "This is the campaign map. Every box is a battle, and winning one opens the next. The glowing box is where we fight today.", {{0}}},
    {TUTORIAL_WAIT_CHOICE, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Here's the important part: you place your generals, then both armies fight on their own. Your skill is in the plan, not in shouting orders mid-battle. Ready?",
        {{"Ready", "Good. Let's not keep the enemy waiting."},
         {"Who fights for me?", "Each general leads a battalion of fifty men. Both armies are led by the same battle-hardened mind, so the plan you make is what tips the scale."}}},
    {TUTORIAL_WAIT_SCENE_CHANGE, TUTORIAL_TARGET_STAGE_CURRENT, TUTORIAL_BOX_TOP,
        "Click the glowing stage to plan the battle.", {{0}}}
};

static const TutorialStep deploy[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_PLAYER_GRID, TUTORIAL_BOX_BOTTOM,
        "This is the war table. The left grid is your side of the field. Each square is a spot where a general and his battalion start the battle.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_ENEMY_GRID, TUTORIAL_BOX_BOTTOM,
        "On the right is the enemy. You always see their layout before you deploy, so use it.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_PLAYER_HEADER, TUTORIAL_BOX_MIDDLE,
        "You must deploy exactly as many generals as the enemy has. This counter shows how many you have placed so far.", {{0}}},
    {TUTORIAL_WAIT_DRAWER_OPEN, TUTORIAL_TARGET_PLAN_DRAWER_HANDLE, TUTORIAL_BOX_MIDDLE,
        "Your generals wait in the reserve drawer. Click the bar at the bottom of the screen to open it.", {{0}}},
    {TUTORIAL_WAIT_GENERAL_PLACED, TUTORIAL_TARGET_PLAN_PLAYER_GRID, TUTORIAL_BOX_TOP,
        "Press and hold a general in the drawer, then drag him onto a square of your grid.", {{0}}},
    {TUTORIAL_WAIT_GENERAL_INSPECTED, TUTORIAL_TARGET_PLAN_PLAYER_GRID, TUTORIAL_BOX_TOP,
        "Well placed. Now click a general once, without holding, to read his stats.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_STATS, TUTORIAL_BOX_TOP,
        "These are the battalion's numbers. Watch the traits at the bottom: anti large troops hit cavalry half again as hard, armor piercing ignores half the armor, and shields block arrows from the front.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_ENEMY_GRID, TUTORIAL_BOX_BOTTOM,
        "Enemy generals can be clicked too. Click the same general again or press Esc to close the panel.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_DRAWER, TUTORIAL_BOX_MIDDLE,
        "To move a general, drag him to another square. Drag him back into the drawer to take him off the field.", {{0}}},
    {TUTORIAL_WAIT_ARMY_READY, TUTORIAL_TARGET_PLAN_PLAYER_HEADER, TUTORIAL_BOX_BOTTOM,
        "Now deploy the rest of your army until the counter matches the enemy.", {{0}}},
    {TUTORIAL_WAIT_SCENE_CHANGE, TUTORIAL_TARGET_PLAN_START_BUTTON, TUTORIAL_BOX_BOTTOM,
        "Your army is ready. Press the start button in the middle of the table to begin the battle.", {{0}}}
};

static const TutorialStep deploy_retry[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_PLAN_ENEMY_GRID, TUTORIAL_BOX_BOTTOM,
        "Same enemy, same layout. Think about what beat us and place your generals to answer it.", {{0}}},
    {TUTORIAL_WAIT_SCENE_CHANGE, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Deploy your army and press start when you're ready.", {{0}}}
};

static const TutorialStep battle_intro[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "I've held the battle so we can talk. Once it starts, both armies fight on their own. You watch, read the battle, and learn for the next plan.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_FIELD_POWER_BAR, TUTORIAL_BOX_TOP,
        "This is the balance of power: the share of health each army still has on the field. Blue is you on the left, red is the enemy on the right.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_FIELD_CARDS, TUTORIAL_BOX_BOTTOM,
        "Each card is one of your battalions with its health. A darkened card has fled the field or been wiped out.", {{0}}},
    {TUTORIAL_WAIT_ENEMY_CARDS, TUTORIAL_TARGET_FIELD_TABS, TUTORIAL_BOX_BOTTOM,
        "Press Tab, or click the tab, to see the enemy's cards.", {{0}}},
    {TUTORIAL_WAIT_CARD_SELECTED, TUTORIAL_TARGET_FIELD_CARDS, TUTORIAL_BOX_BOTTOM,
        "Click any card to open its stats. Clicking a soldier on the battlefield does the same.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_FIELD_DETAIL, TUTORIAL_BOX_TOP,
        "This panel shows everything about that battalion, and its men are circled on the field. Press Esc or click the card again to close it.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Move the camera with the arrow keys, hold Shift to move faster, and zoom with the mouse wheel. Hold A to circle every soldier in his army's color.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_FIELD_CONTROLS, TUTORIAL_BOX_TOP,
        "Space pauses and resumes the battle. Press 1, 2 or 3 to watch at normal, double or quadruple speed.", {{0}}},
    {TUTORIAL_WAIT_HELP_OPENED, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "You don't have to remember any of this. Press H at any time to see every control. Try it now.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "That's everything. Good luck, commander. Let the battle begin!", {{0}}}
};

static const TutorialStep battle_retry[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_FIELD_CONTROLS, TUTORIAL_BOX_TOP,
        "Here we go again. Remember: Space pauses, 1, 2 and 3 change the speed, and H shows every control.", {{0}}}
};

static const TutorialStep first_loss[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "We lost that one. Don't worry, nobody wins every battle on the first try.", {{0}}},
    {TUTORIAL_WAIT_CHOICE, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "What do you think went wrong?",
        {{"My placement", "Then match your strengths against their weaknesses. Put anti large troops where their cavalry will charge, and keep archers behind a sturdy line."},
         {"They were stronger", "They have as many generals as we do. What changes is who meets whom. Study their layout and change your plan."}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "You have two more tries. Let's go again.", {{0}}},
    {TUTORIAL_WAIT_SCENE_CHANGE, TUTORIAL_TARGET_STAGE_CURRENT, TUTORIAL_BOX_TOP,
        "Click the stage to plan your next attempt.", {{0}}}
};

static const TutorialStep second_loss[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Another loss. That enemy is tougher than it looks.", {{0}}},
    {TUTORIAL_WAIT_CHOICE, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Do you want a tip?",
        {{"Yes, please", "Let your sturdiest battalions take the first blow and keep the fragile ones out of reach. Then pause and watch who flees first."},
         {"I'll figure it out", "That's the spirit. Trust what you saw in the last two battles."}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "One last try. Make it count.", {{0}}},
    {TUTORIAL_WAIT_SCENE_CHANGE, TUTORIAL_TARGET_STAGE_CURRENT, TUTORIAL_BOX_TOP,
        "Click the stage to plan your final attempt.", {{0}}}
};

static const TutorialStep farewell_won[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "Victory! You read the enemy and placed your men well. That is the whole craft.", {{0}}},
    {TUTORIAL_WAIT_CHOICE, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "How did that feel?",
        {{"Easy", "Then the campaign will suit you. The real enemies won't be this gentle."},
         {"A close call", "Close calls teach the most. Remember what turned the battle."}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "The generals you trained with are yours now. They'll be waiting in your drawer for the campaign.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_STAGE_CURRENT, TUTORIAL_BOX_TOP,
        "This is where I leave you, commander. Ten battles stand between you and victory. Good luck out there.", {{0}}}
};

static const TutorialStep farewell_spared[] = {
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "We fell again, but you held them long enough. They've pulled back to lick their wounds, and I'm counting that as a win.", {{0}}},
    {TUTORIAL_WAIT_CHOICE, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "How are you holding up?",
        {{"Frustrated", "Every commander starts there. You've seen how a battle flows, and that matters more than one result."},
         {"Ready for more", "Good. That stubbornness will carry you far."}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_NONE, TUTORIAL_BOX_TOP,
        "The generals you trained with are yours now. They'll be waiting in your drawer for the campaign.", {{0}}},
    {TUTORIAL_WAIT_READ, TUTORIAL_TARGET_STAGE_CURRENT, TUTORIAL_BOX_TOP,
        "This is where I leave you, commander. Ten battles stand between you and victory. Good luck out there.", {{0}}}
};

static const TutorialChapterScript chapters[TUTORIAL_CHAPTER_COUNT] = {
    [TUTORIAL_CHAPTER_STAGE_INTRO] = {stage_intro, STEP_COUNT(stage_intro), false, false},
    [TUTORIAL_CHAPTER_DEPLOY] = {deploy, STEP_COUNT(deploy), false, false},
    [TUTORIAL_CHAPTER_DEPLOY_RETRY] = {deploy_retry, STEP_COUNT(deploy_retry), false, false},
    [TUTORIAL_CHAPTER_BATTLE_INTRO] = {battle_intro, STEP_COUNT(battle_intro), true, false},
    [TUTORIAL_CHAPTER_BATTLE_RETRY] = {battle_retry, STEP_COUNT(battle_retry), true, false},
    [TUTORIAL_CHAPTER_FIRST_LOSS] = {first_loss, STEP_COUNT(first_loss), false, false},
    [TUTORIAL_CHAPTER_SECOND_LOSS] = {second_loss, STEP_COUNT(second_loss), false, false},
    [TUTORIAL_CHAPTER_FAREWELL_WON] = {farewell_won, STEP_COUNT(farewell_won), false, true},
    [TUTORIAL_CHAPTER_FAREWELL_SPARED] = {farewell_spared, STEP_COUNT(farewell_spared), false, true}
};

const TutorialChapterScript *tutorial_chapter_script(enum TutorialChapter chapter) {

    if (chapter <= TUTORIAL_CHAPTER_NONE || chapter >= TUTORIAL_CHAPTER_COUNT) return NULL;
    return &chapters[chapter];

}
