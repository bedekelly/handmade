typedef struct OffscreenBuffer {
  int width;
  int height;
  SDL_Texture *texture;
  void *memory;
  int pitch;
} OffscreenBuffer;

typedef struct GameState {
  int xoffset;
  int yoffset;
} GameState;

