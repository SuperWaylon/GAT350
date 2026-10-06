#include "Engine.h"
#include "Core/File.h"
#include "SpaceGame/SpaceGame.h"
#include "SpriteGame/SpriteGame.h"

using namespace nu;

int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    std::unique_ptr<Game> game = std::make_unique<SpriteGame>();
    game->Initialize();

    // MAIN LOOP
    bool quit = false;
    while (!quit) 
    {
        // UPDATE
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
            {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                quit = true;
            }
        }

        // ENGINE
        Engine::Instance().Update();
        float dt = Engine::Instance().GetTime().GetDeltaTime();

        // GAME
        game->Update(dt);

        // RENDER
        Engine::Instance().GetRenderer().SetColor(0.0f, 0.0f, 0.0f);
        Engine::Instance().GetRenderer().Clear();

        game->Draw(Engine::Instance().GetRenderer());
        Engine::Instance().GetPS().Draw(Engine::Instance().GetRenderer());

        Engine::Instance().GetRenderer().Present();
    }

    // reset destroys the object (need to delete game before engine shutdown)
    game.reset();

    // SHUTDOWN
    Engine::Instance().Shutdown();    

    return 0;
}

