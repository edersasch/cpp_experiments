#ifndef SRC_SDL_SDLAPP
#define SRC_SDL_SDLAPP

#include "SDL.h"

#include <memory>
#include <cstdint>

class SDLApp
{
public:
    SDLApp();
    ~SDLApp();
    int exec();

private:
    void processEvents(SDL_Event* event);
    void render();
    bool running = false;
    std::int32_t window_width = 640;
    std::int32_t window_height = 480;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window {nullptr, &SDL_DestroyWindow};
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer {nullptr, &SDL_DestroyRenderer};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> window_surface {nullptr, &SDL_FreeSurface};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> board_surface {nullptr, &SDL_FreeSurface};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> back_surface {nullptr, &SDL_FreeSurface};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> cancel_surface {nullptr, &SDL_FreeSurface};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> delete_surface {nullptr, &SDL_FreeSurface};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> depth_surface {nullptr, &SDL_FreeSurface};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> engine_surface {nullptr, &SDL_FreeSurface};
    SDL_Surface* current_surface {nullptr};
};

#endif //SRC_SDL_SDLAPP
