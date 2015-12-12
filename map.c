#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "SMSlib/src/SMSlib.h"
#include "map.h"

section sections[SEC_COUNT_Y][SEC_COUNT_Y];
char map[PF_HEIGHT][PF_WIDTH];

bool moved_downstairs;

void clear_map() {
  memset(*map, ' ', PF_WIDTH * PF_HEIGHT);
}

void draw_map() {
  char *o;
  unsigned int buffer[PF_WIDTH], *d;
  unsigned int i, j;

  o = map[0];
  for (i = 0; i != PF_HEIGHT; i++) {
    d = buffer;
    for (j = 0; j != PF_WIDTH; j++) {
      *d = *o - 32;
      o++; d++;
    }
    SMS_loadTileMap (0, i, buffer, PF_WIDTH << 1);
  }
}

void draw_room_line(unsigned char x, unsigned char y, unsigned char w, char border, char fill) {
  char *d = map[y] + x;

  *d = border; d++;
  for (w -= 2; w != 0; w--) {
    *d = fill; d++;
  }
  *d = border; d++;
}

void draw_room(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  draw_room_line(x, y, w, '+', '-');
  y++;

  for (h -= 2; h != 0; h--) {
    draw_room_line(x, y, w, '|', '.');
    y++;
  }

  draw_room_line(x, y, w, '+', '-');
}

void draw_corridor_x(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2) {
  char dy = y1 < y2 ? 1 : -1;
  char dx = x1 < x2 ? 1 : -1;
  unsigned char len = x1 < x2 ? x2 - x1 : x1 - x2;
  unsigned char xh = 1 + (len > 1 ? rand() % (len - 1) : 0);
  unsigned char x, y;

  xh = x1 < x2 ? x1 + xh : x1 - xh;

  for (x = x1; x != xh; x += dx) {
      map[y1][x] = '#';
  }

  for (y = y1; y != y2; y += dy) {
      map[y][xh] = '#';
  }

  for (x = xh; x != x2; x += dx) {
      map[y2][x] = '#';
  }

  map[y1][x1] = '*';
  map[y2][x2] = '*';
}

void draw_corridor_y(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2) {
  char dy = y1 < y2 ? 1 : -1;
  char dx = x1 < x2 ? 1 : -1;
  unsigned char len = y1 < y2 ? y2 - y1 : y1 - y2;
  unsigned char yh = 1 + (len > 1 ? rand() % (len - 1) : 0);
  unsigned char x, y;

  yh = y1 < y2 ? y1 + yh : y1 - yh;

  for (y = y1; y != yh; y += dy) {
      map[y][x1] = '#';
  }

  for (x = x1; x != x2; x += dx) {
      map[yh][x] = '#';
  }

  for (y = yh; y != y2; y += dy) {
      map[y][x2] = '#';
  }

  map[y1][x1] = '*';
  map[y2][x2] = '*';
}

void create_section(unsigned char x, unsigned char y, section *sec) {
  unsigned char max_x = x + SEC_WIDTH - 1;
  unsigned char max_y = y + SEC_HEIGHT - 1;

  sec->c.x1 = x + rand() % (SEC_WIDTH - SEC_MIN_W);
  sec->c.y1 = y + rand() % (SEC_HEIGHT - SEC_MIN_H);
  sec->c.x2 = sec->c.x1 + SEC_MIN_W;
  sec->c.y2 = sec->c.y1 + SEC_MIN_H;

  if (sec->c.x2 < max_x) {
    sec->c.x2 += rand() % (max_x - sec->c.x2);
  }
  if (sec->c.y2 < max_y) {
    sec->c.y2 += rand() % (max_y - sec->c.y2);
  }
}

void draw_section(section *sec) {
  draw_room(sec->c.x1, sec->c.y1,
      sec->c.x2 - sec->c.x1 + 1, sec->c.y2 - sec->c.y1 + 1);
}

void create_sections() {
    // Sector number / sector coordinate
    unsigned char secn_x, secn_y, secc_x, secc_y;
    section *sec = sections[0];

    for (secn_y = 0, secc_y = 0; secn_y != SEC_COUNT_Y; secn_y++, secc_y += SEC_HEIGHT) {
      for (secn_x = 0, secc_x = 0; secn_x != SEC_COUNT_X; secn_x++, secc_x += SEC_WIDTH) {
        create_section(secc_x, secc_y, sec);
        draw_section(sec);
        sec++;
      }
    }
}

bool can_go[SEC_COUNT_Y][SEC_COUNT_X][DIR_COUNT];

void create_random_corridor_x(
    unsigned char xa, unsigned char ya1, unsigned char ya2,
    unsigned char xb, unsigned char yb1, unsigned char yb2) {
  unsigned char y1 = ya1 + 1 + rand() % (ya2 - ya1 - 1);
  unsigned char y2 = yb1 + 1 + rand() % (yb2 - yb1 - 1);
  draw_corridor_x(xa, y1, xb, y2);
}

void create_random_corridor_y(
    unsigned char xa1, unsigned char xa2, unsigned char ya,
    unsigned char xb1, unsigned char xb2, unsigned char yb) {
  unsigned char x1 = xa1 + 1 + rand() % (xa2 - xa1 - 1);
  unsigned char x2 = xb1 + 1 + rand() % (xb2 - xb1 - 1);
  draw_corridor_y(x1, ya, x2, yb);
}

void create_corridor(char x1, char y1, char x2, char y2, unsigned char dir) {
  section *sec1, *sec2;

  if (can_go[y1][x1][dir]) {
    x2 = x1; y2 = y1;
    switch (dir) {
      case DIR_UP:
        y2--;
        break;
      case DIR_DOWN:
        y2++;
        break;
      case DIR_LEFT:
        x2--;
        break;
      case DIR_RIGHT:
        x2++;
        break;
    }

    sec1 = &sections[y1][x1];
    can_go[y1][x1][dir] = false;
    sec2 = &sections[y2][x2];
    can_go[y2][x2][(dir + 2) & DIR_MASK] = false;

    switch (dir) {
      case DIR_UP:
        create_random_corridor_y(
            sec1->c.x1, sec1->c.x2, sec1->c.y1,
            sec2->c.x1, sec2->c.x2, sec2->c.y2);
        break;
      case DIR_DOWN:
        create_random_corridor_y(
            sec1->c.x1, sec1->c.x2, sec1->c.y2,
            sec2->c.x1, sec2->c.x2, sec2->c.y1);
        break;
      case DIR_LEFT:
        create_random_corridor_x(
            sec1->c.x1, sec1->c.y1, sec1->c.y2,
            sec2->c.x2, sec2->c.y1, sec2->c.y2);
        break;
      case DIR_RIGHT:
        create_random_corridor_x(
            sec1->c.x2, sec1->c.y1, sec1->c.y2,
            sec2->c.x1, sec2->c.y1, sec2->c.y2);
        break;
    }
  }
}

void random_walk() {
  unsigned char dir, remaining, extra_steps;
  char x1, y1, x2, y2;
  bool visited[SEC_COUNT_Y][SEC_COUNT_X];

  remaining = SEC_COUNT;
  for (y1 = 0; y1 != SEC_COUNT_Y; y1++) {
    for (x1 = 0; x1 != SEC_COUNT_X; x1++) {
      visited[y1][x1] = false;
    }
  }

  x1 = rand() % SEC_COUNT_X;
  y1 = rand() % SEC_COUNT_Y;

  extra_steps = 8;
  while (remaining || extra_steps) {
    // Check if a new room has been visited
    if (!visited[y1][x1]) {
      visited[y1][x1] = true;
      remaining--;
    }

    // After visiting everything, walk some extra steps
    if (!remaining && extra_steps) {
      extra_steps--;
    }

    // Choose a direction to move
    dir = rand() & DIR_MASK;

    // Try to move in the given direction
    x2 = x1; y2 = y1;
    switch (dir) {
      case DIR_UP:
        y2--;
        break;
      case DIR_DOWN:
        y2++;
        break;
      case DIR_LEFT:
        x2--;
        break;
      case DIR_RIGHT:
        x2++;
        break;
    }

    // If the direction is valid, move
    if (x2 >= 0 && x2 < SEC_COUNT_X && y2 >= 0 && y2 < SEC_COUNT_Y) {
      create_corridor(x1, y1, x2, y2, dir);
      x1 = x2; y1 = y2;
    }
  }

}

void create_corridors() {
  unsigned char x, y, i;

  for (y = 0; y != SEC_COUNT_Y; y++) {
    for (x = 0; x != SEC_COUNT_X; x++) {
      for (i = 0; i != DIR_COUNT; i++) {
        can_go[y][x][i] = true;
      }
    }
  }

  for (x = 0; x != SEC_COUNT_X; x++) {
    can_go[0][x][DIR_UP] = false;
    can_go[SEC_COUNT_Y - 1][x][DIR_DOWN] = false;
  }

  for (y = 0; y != SEC_COUNT_Y; y++) {
    can_go[y][0][DIR_LEFT] = false;
    can_go[y][SEC_COUNT_X - 1][DIR_RIGHT] = false;
  }

  random_walk();
}

void generate_map() {
  create_sections();
  create_corridors();
}

bool is_wall(char ch) {
  switch (ch) {
    case '+':
    case '-':
    case '|':
    case ' ':
      return true;
  }
  return false;
}

bool is_ground(char ch) {
  return (ch == '.') || (ch == '#') || (ch == '*');
}

bool is_actor(char ch) {
  return !is_wall(ch) && !is_ground(ch);
}
