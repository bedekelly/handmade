#include <iostream>
#include <SDL.h>

int main(int argc, char *argv[]) {
  // Display a message box to prove SDL is working.
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,  // Type of messagebox
			   "Handmade Hero",             // Box title
			   "This is Handmade Hero",     // Box text
			   NULL                         // Bound window (n/a).
			   );

  int err;
  if (err=SDL_Init(SDL_INIT_EVERYTHING)) {
    // SDL initialization failed for some reason.
    std::cout << "Error code: " << err << ": " << SDL_GetError();
    return err;
  }

  SDL_Window *window = SDL_CreateWindow("Handmade Hero",          // Window title
					SDL_WINDOWPOS_UNDEFINED,  // Upper-left X value
					SDL_WINDOWPOS_UNDEFINED,  // Upper-left Y value
					640,                      // Window width
					480,                      // Window height
					SDL_WINDOW_RESIZABLE      // Window options
					| SDL_WINDOW_BORDERLESS   
					);
}
