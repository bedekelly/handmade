/*
 * Platform-independent code provided to sdlhandmade.cpp and the like.
 */

#include "handmade.h"


void renderGradient(OffscreenBuffer *buffer, int xoffset, int yoffset);

/*
 * Renders a gradient straight to our pixel buffer.
 */
void renderGradient(OffscreenBuffer *buffer, int xoffset, int yoffset) {
  int height = buffer->height;
  int width = buffer->width;
  uint8 *row = (uint8 *)buffer->memory;
  for (int y=0; y<height; y++) {
    uint32 *pixel = (uint32 *)row;
    for (int x=0; x<width; x++) {
      uint8 blue = x + xoffset;
      uint8 green = y + yoffset;
      *pixel = (green << 8) | blue;
      pixel++;
    }
    row += buffer->pitch;
  }
}
