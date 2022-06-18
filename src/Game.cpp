#include <cassert>

#include "Game.h"
#include "AssetManager.h"
#include "Mediator.h"
#include "gfx/Renderer.h"
#include "Debug.h"
#include "ui/UIManager.h"
#include "SceneManager.h"
#include "SaveManager.h"

Game::Game() : m_state{GameState::PLAY} {
    Mediator::fire(EventType::ApplicationStarted);
}

Game::~Game() {
    Mediator::fire(EventType::ApplicationEnding);

    SDL_Quit();
}

void Game::run() {
    init();

    Mediator::fire(EventType::ApplicationLoaded);

    doLoop();
}

void Game::init() {
    // Initialise SDL
    auto error = SDL_Init(SDL_INIT_VIDEO);
    assert(error == 0);

    Renderer::init();
    UIManager::init();

    m_input = std::make_unique<InputManager>();

    AssetManager::loadAssets();
    AssetManager::loadWalls();
    AssetManager::loadObjects();

    // TODO: Move to consts file
    m_input->registerInput({"Up", {SDL_SCANCODE_UP}});
    m_input->registerInput({"Down", {SDL_SCANCODE_DOWN}});
    m_input->registerInput({"Left", {SDL_SCANCODE_LEFT}});
    m_input->registerInput({"Right", {SDL_SCANCODE_RIGHT}});
    m_input->registerInput({"W", {SDL_SCANCODE_W}});
    m_input->registerInput({"A", {SDL_SCANCODE_A}});
    m_input->registerInput({"S", {SDL_SCANCODE_S}});
    m_input->registerInput({"D", {SDL_SCANCODE_D}});
    m_input->registerInput({"ZOOM_IN", {SDL_SCANCODE_COMMA}});
    m_input->registerInput({"ZOOM_OUT", {SDL_SCANCODE_PERIOD}});

    Debug::setup();

    SaveManager::newGame();
}

void Game::doLoop() {
    auto lastTime = SDL_GetTicks64();
    Uint64 lag = 0;
    while (m_state != GameState::EXIT) {
        auto currentTime = SDL_GetTicks64();
        auto elapsed = currentTime - lastTime;
        lastTime = currentTime;
        lag += elapsed;

        pollEvents();

        while (lag >= MS_PER_UPDATE) {
            Debug::preUpdate();
            SceneManager::getCurrentScene()->preUpdate();
            SceneManager::getCurrentScene()->update();
            SceneManager::getCurrentScene()->postUpdate();
            lag -= MS_PER_UPDATE;
            m_input->clearKeys();
        }


        render((double) lag / (double) MS_PER_UPDATE);
    }
}

void Game::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        bool mouseOverUI = UIManager::processInput(&event);

        switch (event.type) {
            case SDL_QUIT:
                m_state = GameState::EXIT;
                break;
            case SDL_KEYDOWN:
                m_input->dispatchKeyDown(event.key);
                break;
            case SDL_KEYUP:
                m_input->dispatchKeyUp(event.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (mouseOverUI) break;
                m_input->dispatchMouseDown(event.button);
                break;
            case SDL_MOUSEBUTTONUP:
//                if (mouseOverUI) break;
                m_input->dispatchMouseUp(event.button);
                break;
            case SDL_MOUSEMOTION:
                if (mouseOverUI) break;
                m_input->dispatchMouseMove(event.motion);
                break;
            case SDL_MOUSEWHEEL:
                m_input->dispatchMouseScroll(event.wheel);
            default:
                break;
        }
    }
}

void Game::render(const double step) const {
    Renderer::clear();

    SceneManager::getCurrentScene()->render(step);
    Renderer::renderBlits();
    Debug::render();
    UIManager::render();

    Renderer::doRender();

}
