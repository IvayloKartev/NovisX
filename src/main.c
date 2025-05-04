#include "core/engine.h"

int main(int argc, char* argv[]) {
    if (!engine_init()) return 1;
    engine_run();
    engine_shutdown();
    return 0;
}
