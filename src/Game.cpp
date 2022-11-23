#include "Game.h"
#include "Renderer.h"
#include "SaveManager.h"
#include "Messenger.h"
#include "UIManager.h"
#include "Profiler.h"
#include "Root.h"

Game::Game() : ui(), renderer() {
    Messenger::fire(EventType::ApplicationStarted);
}

Game::~Game() {
    Messenger::fire(EventType::ApplicationEnding);
}

void Game::run() {
    init();

    Messenger::fire(EventType::ApplicationLoaded);

    doLoop();
}

void Game::init() {
    renderer.init();
    ui.init();

    assetManager.loadAssets();
    assetManager.loadWalls();
    assetManager.loadPaths();
    assetManager.loadObjects();

    saveManager.newGame();
}

void Game::doLoop() {
    auto lastTime = GetTime() * 1000;
    double lag = 0;
    while (!WindowShouldClose()) {
        auto currentTime = GetTime() * 1000;
        auto elapsed = currentTime - lastTime;
        lastTime = currentTime;
        lag += elapsed;

        while (lag >= MS_PER_UPDATE) {
            Profiler::begin("tick");

            Profiler::startTimer("preUpdate");
            sceneManager.getCurrentScene()->preUpdate();
            Profiler::stopTimer("preUpdate");

            Profiler::startTimer("update");
            sceneManager.getCurrentScene()->update();
            Profiler::stopTimer("update");

            Profiler::startTimer("postUpdate");
            sceneManager.getCurrentScene()->postUpdate();
            Profiler::stopTimer("postUpdate");

            Profiler::end("tick");

            lag -= MS_PER_UPDATE;

            ticksSinceGameStart++;
        }

        InputManager::processInput();

        render(double(lag) / double(MS_PER_UPDATE));

        ui.postUpdate();
    }
}

void Game::render(double step) {
    renderer.beginDrawing();
    {
        Profiler::begin("render");
        BeginMode2D(renderer.camera);
        {
            Profiler::startTimer("scene");
            sceneManager.getCurrentScene()->render(step);
            Profiler::stopTimer("scene");
            Profiler::startTimer("blits");
            renderer.sortAndDrawBlits();
            Profiler::stopTimer("blits");
            Profiler::startTimer("sceneLate");
            sceneManager.getCurrentScene()->renderLate(step);
            Profiler::stopTimer("sceneLate");
            Profiler::startTimer("blitsLate");
            renderer.sortAndDrawBlits();
            Profiler::stopTimer("blitsLate");
        }
        EndMode2D();

        Profiler::startTimer("ui");
        ui.render();
        Profiler::stopTimer("ui");

        Profiler::end("render");
    }
    renderer.endDrawing();

    get().framesSinceGameStart++;
}

void Game::onInput(InputEvent* event) {
    if (!event->consumed) ui.onInput(event);
    if (!event->consumed) sceneManager.getCurrentScene()->onInput(event);
}

int Game::getTicks() {
    return get().ticksSinceGameStart;
}
