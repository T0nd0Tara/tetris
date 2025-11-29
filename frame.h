#ifndef FRAME_H
#define FRAME_H

#include <ncurses.h>

struct Frame {
  int x;
  int y;
  int w;
  int h;
};

void add_frame(struct Frame);
void draw_frames(WINDOW *screen);
void clean_frames();

#endif // !FRAME_H
