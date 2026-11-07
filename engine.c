#include "engine_internal.h"
#include "engine.h"

Engine engine = {0};

void engine_init(_Bool is_ai_bypass_disabled) {

    engine.has_program_been_initialized_with_ai_bypass = !is_ai_bypass_disabled;
    engine.is_ai_bypass_disabled = is_ai_bypass_disabled;
    engine.render = (is_ai_bypass_disabled) ? ENABLED : DISABLED;

}
