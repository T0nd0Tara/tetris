#ifdef __MINGw32__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <math.h>
#include <stdio.h>
#include <time.h>

#define SCREEN (stdscr)
#define FPS (120.0f)
#define CHAR_RATIO (2.0f)

#define BOARD_WIDTH  (10)
#define BOARD_HEIGHT (20)

static int screen_width, screen_height;

typedef uint8_t color; 

color board[BOARD_HEIGHT][BOARD_WIDTH] = { COLOR_WHITE };

enum colors : uint8_t {
  DEV_DATA = COLOR_WHITE + 1,
};

struct shape {
  color c;
  bool shape[4][4];
};

static struct shape shapes[] = {
  {
    .c = COLOR_BLUE,
    .shape =
    {
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
    }},
  {
    .c = COLOR_YELLOW,
    .shape =
    {
      {0, 0, 0, 0},
      {0, 1, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0},
    }},
  {
    .c = COLOR_RED,
    .shape =
    {
      {0, 0, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 1, 1},
      {0, 0, 0, 0},
    }},
  {
    .c = COLOR_GREEN,
    .shape =
    {
      {0, 0, 0, 0},
      {0, 1, 1, 0},
      {1, 1, 0, 0},
      {0, 0, 0, 0},
    }},
  {
    .c = COLOR_YELLOW,
    .shape =
    {
      {0, 0, 0, 0},
      {0, 0, 1, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0},
    }},
  {
    .c = COLOR_CYAN,
    .shape =
    {
      {0, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 1, 1},
      {0, 0, 0, 0},
    }},
  {
    .c = COLOR_MAGENTA,
    .shape =
    {
      {0, 0, 0, 0},
      {0, 1, 0, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0},
    }},
};

void drawBoard() {
  const int scale_y = screen_height / BOARD_HEIGHT;
  const int scale_x = scale_y * CHAR_RATIO;
  const int diffY = (screen_height - scale_y * BOARD_HEIGHT) / 2;
  const int diffX = (screen_width - scale_x * BOARD_WIDTH) / 2;

  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
      color c = (x + y) % 2 == 0 ? COLOR_WHITE : COLOR_MAGENTA;
      attron(COLOR_PAIR(c));

      for (int scale_y_it = 0; scale_y_it < scale_y; scale_y_it++)
        for (int scale_x_it = 0; scale_x_it < scale_x ; scale_x_it++)
          mvwaddch(SCREEN, y * scale_y + diffY + scale_y_it, x * scale_x + diffX + scale_x_it, ' ');
      attroff(COLOR_PAIR(c));
    }
  }

}
bool loop(float elapsedTime) {
  getmaxyx(SCREEN, screen_height, screen_width);

  attron(COLOR_PAIR(DEV_DATA));

  char buff[50];
  snprintf(buff, 50, "FPS: %f", 1000.0f / elapsedTime);
  mvwprintw(SCREEN, 0, 0, buff);
  attroff(COLOR_PAIR(DEV_DATA));

  drawBoard();

  refresh();

  return true;
}
int main(int argc, char **argv) {
  initscr(); // Init the library.
  noecho();  // Do not echo user input.
  cbreak();  // Do not buffer user input, retain Ctrl-Z & Ctrl-C functions.
  keypad(SCREEN,
         TRUE); // Enable extended character (e.g. F-keys, numpad) input.
  curs_set(0);  // Change cursor appearance. 0 invisible, 1 normal, 2 strong.

  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal doesn't support colors.\n");
    return 1;
  }
  start_color();
  for (color i = COLOR_BLACK; i <= COLOR_WHITE; ++i) 
    init_pair(i, COLOR_BLACK, i); 
  init_pair(DEV_DATA, COLOR_WHITE, COLOR_BLUE); 

  float wantedFrameTimeMS = 1000.0f / FPS;

  bool shouldContinue;
  float prevFrameTime = 0;
  do {
    float sleepNeeded = wantedFrameTimeMS - prevFrameTime;
    if (sleepNeeded > 0)
      napms(sleepNeeded);

    clock_t start = clock();
    shouldContinue = loop( wantedFrameTimeMS < prevFrameTime ? prevFrameTime : wantedFrameTimeMS);
    // shouldContinue = loop(prevFrameTime);
    clock_t end = clock();

    prevFrameTime = 1000.0f * (float)(end - start) / CLOCKS_PER_SEC;
  } while (shouldContinue);

  // getch(); // Wait for key press before exiting.

  endwin(); // Main ncurses clean-up.

  return 0;
}
