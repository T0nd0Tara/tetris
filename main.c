#ifdef __MINGw32__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN (stdscr)
#define FPS (120.0f)
#define CHAR_RATIO (2.0f)

#define BOARD_WIDTH  (10)
#define BOARD_HEIGHT (20)

#define NEXT_SHAPES (3)
#define SHAPE_SIZE (4)

#define KEY_UP    ('w')
#define KEY_LEFT  ('a')
#define KEY_DOWN  ('s')
#define KEY_RIGHT ('d')

static int screen_width, screen_height;

static int scale_y, scale_x;
static int board_start_y, board_start_x;
static float game_tic = 300.0f, counting_game_tic = 0;

typedef uint8_t color; 

color board[BOARD_HEIGHT][BOARD_WIDTH] = { COLOR_BLACK };

enum colors : uint8_t {
  DEV_DATA = COLOR_WHITE + 1,
  FRAME,
};

struct shape {
  color c;
  bool shape[SHAPE_SIZE][SHAPE_SIZE];
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

static size_t shapes_len = sizeof(shapes) / sizeof(shapes[0]);

int next_shapes[NEXT_SHAPES];
struct {
  size_t index;
  uint8_t rotation;
  int x, y;
} current_shape;

void draw_scaled_pixel(size_t x, size_t y) {
  for (int scale_y_it = 0; scale_y_it < scale_y; scale_y_it++)
    for (int scale_x_it = 0; scale_x_it < scale_x ; scale_x_it++)
      mvwaddch(SCREEN, y + scale_y_it, x + scale_x_it, ' ');

}

void draw_shape(size_t index, size_t x, size_t y, uint8_t rotation) {
  attron(COLOR_PAIR(shapes[index].c));
  for (size_t y_it = 0; y_it < SHAPE_SIZE; y_it++) {
    for (size_t x_it = 0; x_it < SHAPE_SIZE; x_it++) {
      if (shapes[index].shape[y_it][x_it])
        draw_scaled_pixel(x + x_it * scale_x, y + y_it * scale_y);
    }
  }

  attroff(COLOR_PAIR(shapes[index].c));
}

void next_shape() {
  current_shape.index = next_shapes[0];
  current_shape.rotation = 0;
  current_shape.y = -SHAPE_SIZE;
  current_shape.x = (BOARD_WIDTH - SHAPE_SIZE) / 2;

  for (size_t i = 1; i < NEXT_SHAPES; i++) {
    next_shapes[i - 1] = next_shapes[i];
  }

  next_shapes[NEXT_SHAPES - 1] = rand() % shapes_len;
}

void draw_board_frame() {
  attron(COLOR_PAIR(FRAME));
  for (int x = 0; x < BOARD_WIDTH * scale_x; x++) {
    mvwaddch(SCREEN, board_start_y - 1, x + board_start_x, '-');
    mvwaddch(SCREEN, board_start_y + BOARD_HEIGHT * scale_y, x + board_start_x, '-');
  }
  for (int y = 0; y < BOARD_HEIGHT * scale_y; y++) {
    mvwaddch(SCREEN, y + board_start_y, board_start_x - 1, '|');
    mvwaddch(SCREEN, y + board_start_y, board_start_x + BOARD_WIDTH * scale_x, '|');
  }

  // corners
  mvwaddch(SCREEN, board_start_y - 1, board_start_x - 1, '+');
  mvwaddch(SCREEN, board_start_y + BOARD_HEIGHT * scale_y, board_start_x - 1, '+');
  mvwaddch(SCREEN, board_start_y - 1, board_start_x + BOARD_WIDTH * scale_x, '+');
  mvwaddch(SCREEN, board_start_y + BOARD_HEIGHT * scale_y, board_start_x  + BOARD_WIDTH * scale_x, '+');


  attroff(COLOR_PAIR(FRAME));
}

void draw_board() {
  draw_board_frame();

  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
      // color c = (x + y) % 2 == 0 ? COLOR_WHITE : COLOR_MAGENTA;
      color c = board[y][x];
      attron(COLOR_PAIR(c));
      draw_scaled_pixel(x * scale_x + board_start_x, y * scale_y + board_start_y);

      attroff(COLOR_PAIR(c));
    }
  }

}

void draw_next_shapes() {
  size_t board_end_x = board_start_x + BOARD_WIDTH * scale_x;
  size_t next_shapes_start_x = board_end_x + 2 * scale_x; 

  for (size_t next_shape_it = 0; next_shape_it < NEXT_SHAPES; ++next_shape_it) {
    int next_shape = next_shapes[next_shape_it];
    attron(COLOR_PAIR(shapes[next_shape].c));
    size_t y = board_start_y + next_shape_it * SHAPE_SIZE * scale_y;

    draw_shape(next_shape, next_shapes_start_x, y, 0);
    attroff(COLOR_PAIR(shapes[next_shape].c));
  }
}
void draw_current_shape() {
  draw_shape(current_shape.index, current_shape.x * scale_x + board_start_x, current_shape.y * scale_y + board_start_y, current_shape.rotation);
}
void draw_bg() {
  attroff(COLOR_PAIR(COLOR_BLACK));
  for (int y = 0; y < screen_width; y++)
    for (int x = 0; x < screen_width; x++)
      mvwaddch(SCREEN, y, x, ' ');
  attron(COLOR_PAIR(COLOR_BLACK));
}

void draw_dev_data(float elapsedTime) {
  attron(COLOR_PAIR(DEV_DATA));

  char buff[50];
  snprintf(buff, 50, "FPS: %f", 1000.0f / elapsedTime);
  mvwprintw(SCREEN, 0, 0, buff);
  attroff(COLOR_PAIR(DEV_DATA));
}

void draw(float elapsedTime) {
  draw_bg();
  draw_board();
  draw_next_shapes();
  draw_current_shape();
  draw_dev_data(elapsedTime);
}

void handle_keys() {
  int key_pressed;
  while (key_pressed = wgetch(SCREEN), key_pressed != ERR) {
    switch (key_pressed) {
      case KEY_RIGHT: {
        current_shape.x = current_shape.x + 1;
        break;
      }
      case KEY_LEFT: {
        current_shape.x = current_shape.x - 1;
        break;
      }
    }
  }
}

void tick() {
  current_shape.y++;
}

void update(float elapsedTime) {
  counting_game_tic += elapsedTime;

  while (counting_game_tic > game_tic) {
    counting_game_tic -= game_tic;

    tick();
  }
  
  handle_keys();
}
bool loop(float elapsedTime) {
  getmaxyx(SCREEN, screen_height, screen_width);
  scale_y = screen_height / BOARD_HEIGHT;
  scale_x = scale_y * CHAR_RATIO;

  board_start_y = (screen_height - scale_y * BOARD_HEIGHT) / 2;
  board_start_x = (screen_width - scale_x * BOARD_WIDTH) / 2;


  update(elapsedTime);
  draw(elapsedTime);

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
  nodelay(SCREEN, true); // Don't wait for user input
  // srand(time(0));
  srand(0);

  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal doesn't support colors.\n");
    return 1;
  }
  start_color();
  for (color i = COLOR_BLACK; i <= COLOR_WHITE; ++i) 
    init_pair(i, COLOR_BLACK, i); 
  init_pair(DEV_DATA, COLOR_WHITE, COLOR_BLUE); 
  init_pair(FRAME, COLOR_WHITE, COLOR_BLACK); 

  for (int next_shape_it = 0; next_shape_it < NEXT_SHAPES; next_shape_it++) {
    next_shapes[next_shape_it] = rand() % shapes_len;
  }
  next_shape();

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
