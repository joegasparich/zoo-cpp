#include "Game.h"

int main(void)
{
    SetTraceLogLevel(LOG_ALL);

    Game::get().run();
    return 0;
}
