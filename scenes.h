#ifndef SCENES_H
#define SCENES_H

enum Scene {

    SCENE_NONE = -1,
    MAIN_MENU,
    BATTLEPLAN,
    BATTLEFIELD,
    SETTINGS,
    STAGE_MAP,
    SCENE_COUNT

};

typedef struct SceneState {

    enum Scene scene;

} SceneState;

#endif