#include <iostream>
#include <SDL.h>
#include <cstdlib>
#include "sdlhandmade.h"
#include "handmade.cpp"

bool handleEvent(SDL_Event *event, OffscreenBuffer *buffer, GameState *gamestate);
void SDLResizeTexture(OffscreenBuffer *buffer, SDL_Renderer *renderer, int width, int height);
void SDLUpdateWindow(OffscreenBuffer *buffer, SDL_Renderer *renderer);


int main(int argc, char *argv[]) {
  // Display a message box to prove SDL is working.
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,  // Type of messagebox
			   "Handmade Hero",             // Box title
			   "This is Handmade Hero",     // Box text
			   NULL                         // Bound window (n/a).
			   );

  // Initialise the SDL framework, and print error messages if we don't succeed.
  int err;
  if (err=SDL_Init(SDL_INIT_EVERYTHING)) {
    // SDL initialization failed for some reason.
    std::cout << "Error code: " << err << ": " << SDL_GetError();
    return err;
  }
  
  // Create a window for our program to run in, using the SDL framework.
  SDL_Window *window = SDL_CreateWindow("Handmade Hero",          // Window title
					SDL_WINDOWPOS_UNDEFINED,  // Upper-left X value
					SDL_WINDOWPOS_UNDEFINED,  // Upper-left Y value
					640,                      // Window width
					480,                      // Window height
					SDL_WINDOW_RESIZABLE      // Window options
					| SDL_WINDOW_BORDERLESS   
					);

  // Check nothing has gone wrong in the window-creation process.
  if (!window) {
    std::cout << "Something's gone wrong with creating a window." << std::endl;
    std::cout << SDL_GetError() << std::endl;
    return -1;
  }

  // Create a renderer for our window to use.
  SDL_Renderer *renderer = SDL_CreateRenderer(window,  // Our "window handle".
					      -1,      /* Initialise the first rendering driver which
					                * supports the flags we want. */
					      0        /* SDL_RendererFlags: flags to pass to our
							* renderer regarding acceleration etc. */
					      );

  // Check nothing has gone wrong in the renderer-creation process.
  if (!renderer) {
    std::cout << "Something's gone wrong with creating a renderer." << std::endl;
    std::cout << SDL_GetError() << std::endl;
    return -1;
  }

  // We don't get a "window changed size message when the program starts, so here we manually
  // "resize" our texture to match the window's size.
  int width, height, xoffset, yoffset;
  SDL_GetWindowSize(window, &width, &height);

  // Create our own offscreen buffer structure.
  OffscreenBuffer buffer;
  buffer.memory = NULL;  // Otherwise it's an "invalid pointer" somehow.

  // Allocate a new pixel buffer and texture.
  SDLResizeTexture(&buffer, renderer, width, height);

  // Create a "game state" - where we are in the gradient.
  GameState gamestate = {0, 0};

  // Record the current 'counter' (time) for performance analysis.
  uint64 perfCountFreq = SDL_GetPerformanceFrequency();
  uint64 lastCounter = SDL_GetPerformanceCounter();



  // Enter an loop inside which our program should handle events.
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (handleEvent(&event, &buffer, &gamestate)) {
	running = false;
	break;
      }
    }

    // Lock the texture for write-only access.
    SDL_LockTexture(buffer.texture,
		  NULL,
		  &buffer.memory,
		  &buffer.pitch);

    // Render our gradient to the texture.
    renderGradient(&buffer, gamestate.xoffset, gamestate.yoffset);

    // Unlock the texture, saving our changes.
    SDL_UnlockTexture(buffer.texture);

    // Display our changes in the current window.
    SDLUpdateWindow(&buffer, renderer);

    // Benchmarking:
    uint64 currentCounter = SDL_GetPerformanceCounter();
    uint64 counterElapsed = currentCounter - lastCounter;
    real64 msPerFrame = (1000.0f * (real64)counterElapsed) / (real64)perfCountFreq;
    real64 framesPerSec = (real64)perfCountFreq / (real64)counterElapsed;
    
    std::cout << "Frame rendered in " << msPerFrame << "ms." << std::endl;
    std::cout << "That's " << framesPerSec << "fps to you and me." << std::endl;

    lastCounter = currentCounter;
  }
}


/*
 * Respond to all SDL events, including the signal to shut down.
 */
bool handleEvent(SDL_Event *event, OffscreenBuffer *buffer, GameState *gamestate) {
  bool shouldQuit = false;

  switch(event->type) {
  case SDL_QUIT:
    {
      std::cout << "Quitting program." << std::endl;
      shouldQuit = true;
      break;
    }
  case SDL_KEYDOWN:
  case SDL_KEYUP:
    {
      SDL_Keycode keycode = event->key.keysym.sym;
      if (keycode == SDLK_q)
	shouldQuit = true;
      else if (keycode == SDLK_w)
	gamestate->yoffset++;
      else if (keycode == SDLK_s)
	gamestate->yoffset--;
      else if (keycode == SDLK_a)
	gamestate->xoffset++;
      else if (keycode == SDLK_d)
	gamestate->xoffset--;
      break;
    }

  case SDL_WINDOWEVENT:
    // SDL wraps all its window events like this.
    {
      switch (event->window.event) {
      case SDL_WINDOWEVENT_EXPOSED:
	{
	  // The window's exposed, so we should re-render it.
	  //std::cout << "Window 'exposed'; re-rendering." << std::endl;
	  SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
	  SDL_Renderer *renderer = SDL_GetRenderer(window);
	  SDLUpdateWindow(buffer, renderer);
	  break;
	}
      case SDL_WINDOWEVENT_RESIZED:
	{
          // std::cout << "Window resized to " << event->window.data1 << " by " << event->window.data2
	  // 	    << std::endl;

	  SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
	  SDL_Renderer *renderer = SDL_GetRenderer(window);

	  int width = event->window.data1;
	  int height = event->window.data2;
	  
	  SDLResizeTexture(buffer, renderer, width, height);

	  break;
	}
      default:
	{
	  // std::cout << "[INFO] Unhandled Window Event: " << event->window.event << std::endl;
	  break;
	}
      }
    }
  default:
    {
      // std::cout << "[INFO] Unhandled event: " << event->type << std::endl;
      break;
    }
    
  }

  return shouldQuit;
}


/*
 * This is really ResizeTextureAndPixelBuffer.
 * Given a height and a width, create a new texture of that size.
 * Reallocate our own pixel buffer to accomodate the new size.
 */
void SDLResizeTexture(OffscreenBuffer *buffer, SDL_Renderer *renderer, int width, int height) {
  // Make a new texture of given size.
  if (buffer->texture) SDL_DestroyTexture(buffer->texture);
  buffer->texture = SDL_CreateTexture(renderer,
				      SDL_PIXELFORMAT_ARGB8888,     // Pixel format
				      SDL_TEXTUREACCESS_STREAMING,  // Hint for how we're usinpg the texture
				      width,
				      height
				      );
  // Update our global bitmap sizes with the new sizes passed in.
  buffer->width = width;
  buffer->height = height;
}


/*
 * Stretch our texture over the renderer.
 * Then display the renderer.
 */
void SDLUpdateWindow(OffscreenBuffer *buffer, SDL_Renderer *renderer) {
  // Stretch our texture over the renderer's entire screen.
  SDL_RenderCopy(renderer,
		 buffer->texture,
		 NULL, NULL  // Stretch the texture over the entire screen.
		 );

  // Make the renderer display its current screen.
  SDL_RenderPresent(renderer);
}
