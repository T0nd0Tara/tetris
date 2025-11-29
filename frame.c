#include "frame.h"

#ifndef FRAME_HO
#define FRAME_HO ('~')
#endif // !FRAME_HO

#ifndef FRAME_VE
#define FRAME_VE ('|')
#endif // !FRAME_VE

#ifndef FRAME_CO
#define FRAME_CO ('+')
#endif // !FRAME_CO

#define MAX_FRAMES (5)

static struct Frame frames[MAX_FRAMES] = {};
static int frame_count = 0;

struct Corner {
  int x;
  int y;
};

void draw_frame_sides(struct Frame frame, WINDOW *screen) {
  for (int x = frame.x; x < frame.x + frame.w; x++) {
    mvwaddch(screen, frame.y, x, FRAME_HO);
    mvwaddch(screen, frame.y + frame.h, x, FRAME_HO);
  }
  for (int y = frame.y; y < frame.y + frame.h; y++) {
    mvwaddch(screen, y, frame.x, FRAME_VE);
    mvwaddch(screen, y, frame.x + frame.w, FRAME_VE);
  }
}

void add_frame(struct Frame frame) { frames[frame_count++] = frame; }
void draw_frames(WINDOW *screen) {
  int total_corners = frame_count * 4;
  struct Corner corners[total_corners];
  int corners_count = 0;
  for (int frame_ind = 0; frame_ind < frame_count; frame_ind++) {
    struct Frame frame = frames[frame_ind];
    struct Corner frame_corners[] = {
        {frame.x, frame.y},
        {frame.x + frame.w, frame.y},
        {frame.x, frame.y + frame.h},
        {frame.x + frame.w, frame.y + frame.h},
    };

    for (int frame_corner = 0; frame_corner < 4; frame_corner++) {
      corners[corners_count++] = frame_corners[frame_corner];
    }

    draw_frame_sides(frame, screen);
  }

  for (int corner_ind = 0; corner_ind < corners_count; corner_ind++) {
    struct Corner corner = corners[corner_ind];
    mvwaddch(screen, corner.y, corner.x, FRAME_CO);
  }
}
void clean_frames() { frame_count = 0; }
