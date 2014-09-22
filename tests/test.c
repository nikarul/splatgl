#include "splat.h"
#include <SDL.h>

int main(int argc, char **argv) {
  SDL_Window *window = SDL_CreateWindow("Splat Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  if (!window) {
	printf("SDL Window creation failed!  SDL error message:\n\t%s\n", SDL_GetError());
	return 1;
  }

  int result = Splat_Prepare(window, 640, 480);
  if (result < 0) {
	printf("Initialization failed!  Splat error message:\n\t%s\n", Splat_GetError());
	return 1;
  }

  SDL_Delay(5000);

  Splat_Finish();
  return 0;
}
