#ifndef FRAME_H
#define FRAME_H

#include <ncurses.h>

#ifndef FRAME_HO
#define FRAME_HO ('~')
#endif // !FRAME_HO

#ifndef FRAME_VE
#define FRAME_VE ('|')
#endif // !FRAME_VE

#ifndef FRAME_CO
#define FRAME_CO ('+')
#endif // !FRAME_CO

#ifndef FRAME_TITLE_PADDING
#define FRAME_TITLE_PADDING (1)
#endif // !FRAME_TITLE_PADDING

#ifndef MAX_FRAMES
#define MAX_FRAMES (5)
#endif // !MAX_FRAMES


struct Frame {
  int x;
  int y;
  int w;
  int h;
  const char *title;
};

void add_frame(struct Frame);
void draw_frames(WINDOW *screen);
void clean_frames();

#endif // !FRAME_H
