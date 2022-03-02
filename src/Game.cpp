#include <SDL2/SDL.h>
#include <cassert>

#include "Game.h"
#include "AssetManager.h"
#include "Mediator.h"
#include "renderer/Renderer.h"

#include "constants/events.h"

Game::Game() :
    state{GameState::PLAY},
    stage{std::make_unique<Stage>()},
    input{std::make_unique<InputManager>()} {}

Game::~Game() {
    SDL_Quit();
}

void Game::run() {
    init();
    stage->setup();

    Mediator::getInstance().fire(GameEvent::SETUP_COMPLETE, "");

    doLoop();
}

void Game::init() {
    // Initialise SDL
    auto error = SDL_Init(SDL_INIT_VIDEO);
    assert(error == 0);

    Renderer::init();

    AssetManager::loadAssets();

    // TODO: Move to consts file
    input->registerInput({"Up", {SDL_SCANCODE_UP}});
    input->registerInput({"Down", {SDL_SCANCODE_DOWN}});
    input->registerInput({"Left", {SDL_SCANCODE_LEFT}});
    input->registerInput({"Right", {SDL_SCANCODE_RIGHT}});
    input->registerInput({"W", {SDL_SCANCODE_W}});
    input->registerInput({"A", {SDL_SCANCODE_A}});
    input->registerInput({"S", {SDL_SCANCODE_S}});
    input->registerInput({"D", {SDL_SCANCODE_D}});
    input->registerInput({"ZOOM_IN", {SDL_SCANCODE_COMMA}});
    input->registerInput({"ZOOM_OUT", {SDL_SCANCODE_PERIOD}});
}

void Game::doLoop() {
    auto lastTime = SDL_GetTicks64();
    Uint64 lag = 0;
    while (state != GameState::EXIT) {
        auto currentTime = SDL_GetTicks64();
        auto elapsed = currentTime - lastTime;
        lastTime = currentTime;
        lag += elapsed;

        pollEvents();

        while (lag >= MS_PER_UPDATE) {
            stage->update();
            lag -= MS_PER_UPDATE;
        }

        input->clearKeys();

        render((double) lag / (double) MS_PER_UPDATE);
    }
}

void Game::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                state = GameState::EXIT;
                break;
            case SDL_KEYDOWN:
                input->dispatchKeyDown(event.key);
                break;
            case SDL_KEYUP:
                input->dispatchKeyUp(event.key);
                break;
            default:
                break;
        }
    }
}

void Game::render(const double step) const {
    Renderer::clear();

    // Do all renders in here
    stage->render(step);

    Renderer::doRender();
}
