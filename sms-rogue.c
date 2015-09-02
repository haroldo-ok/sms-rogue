#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SMSlib/src/SMSlib.h"
#include "gfx.h"

#define PF_OFFSET_X 0
#define PF_OFFSET_Y 0

#define PF_WIDTH 32
#define PF_HEIGHT 24

#define SEC_COUNT_X 3
#define SEC_COUNT_Y 3
#define SEC_COUNT (SEC_COUNT_X * SEC_COUNT_Y)
#define SEC_WIDTH (PF_WIDTH / SEC_COUNT_X)
#define SEC_HEIGHT (PF_HEIGHT / SEC_COUNT_Y)
#define SEC_MIN_W 4
#define SEC_MIN_H 4

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_COUNT 4
#define DIR_MASK (DIR_COUNT - 1)

struct box {
  unsigned char x1, y1, x2, y2;
};

struct section {
  struct box c;
};

struct section sections[SEC_COUNT_Y][SEC_COUNT_Y];
char map[PF_HEIGHT][PF_WIDTH];

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
  unsigned char xh = rand() % len;
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
  unsigned char yh = rand() % len;
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

void create_section(unsigned char x, unsigned char y, struct section *sec) {
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

void draw_section(struct section *sec) {
  draw_room(sec->c.x1, sec->c.y1,
      sec->c.x2 - sec->c.x1 + 1, sec->c.y2 - sec->c.y1 + 1);
}

void create_sections() {
    // Sector number / sector coordinate
    unsigned char secn_x, secn_y, secc_x, secc_y;
    struct section *sec = sections[0];

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
    unsigned char xa1, unsigned char xa2, unsigned char ya,
    unsigned char xb1, unsigned char xb2, unsigned char yb) {
  unsigned char x1 = xa1;
  unsigned char x2 = xb2;
  draw_corridor_x(x1, ya, x2, yb);
}

void create_corridor(unsigned char x, unsigned char y) {
  unsigned char dir = DIR_DOWN //= rand() & DIR_MASK;
  unsigned char x2, y2;
  struct section *sec1, *sec2;

  if (true || can_go[y][x][dir]) {
    x2 = x; y2 = x;

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

    sec1 = &sections[y][x];
    can_go[y][x][dir] = false;
    sec2 = &sections[y2][x2];
    can_go[y2][x2][(dir + 2) & DIR_MASK] = false;

    switch (dir) {
      case DIR_UP:
        draw_corridor_x(sec1->c.x1, sec1->c.y1, sec2->c.x2, sec2->c.y2);
        create_random_corridor_x(
            sec1->c.x1, sec1->c.x2, sec1->c.y1,
            sec2->c.x2, sec2->c.x2, sec2->c.y2);
        break;
      case DIR_DOWN:
        draw_corridor_y(sec1->c.x1, sec1->c.y1, sec2->c.x2, sec2->c.y2);
      /*
        create_random_corridor_x(
            sec1->c.x1, sec1->c.x2, sec1->c.y2,
            sec2->c.x2, sec2->c.x2, sec2->c.y1);
            */
        break;
      case DIR_LEFT:
        draw_corridor_x(sec1->c.x1, sec1->c.y1, sec2->c.x2, sec2->c.y2);
        break;
      case DIR_RIGHT:
        draw_corridor_x(sec1->c.x1, sec1->c.y1, sec2->c.x2, sec2->c.y2);
        break;
    }
  }
}

void create_corridors() {
  unsigned char x, y;
  memset(**can_go, true, SEC_COUNT * DIR_COUNT);

  for (x = 0; x != SEC_COUNT_X; x++) {
    can_go[0][x][DIR_UP] = false;
    can_go[SEC_COUNT_Y - 1][x][DIR_DOWN] = false;
  }

  for (y = 0; y != SEC_COUNT_Y; y++) {
    can_go[y][0][DIR_LEFT] = false;
    can_go[y][SEC_COUNT_X - 1][DIR_RIGHT] = false;
  }

/*
  for (y = 0; y != SEC_COUNT_Y; y++) {
    for (x = 0; x != SEC_COUNT_X; x++) {
      create_corridor(x, y);
    }
  }
  */
  create_corridor(1, 1);
}

void draw_char(unsigned char x, unsigned char y, char c) {
  SMS_setTileatXY(x + PF_OFFSET_X, y + PF_OFFSET_Y, c - 32);
}

int px = 1, py = 1;
void move_to(int x, int y) {
  switch (map[y][x]) {
    case '.':
    case '*':
    case '#':
      px = x; py = y;
  }
}

void simple_rl(void)
{
  unsigned short kp;

  create_sections();
  create_corridors();
  px = sections[0][0].c.x1 + 2;
  py = sections[0][0].c.y1 + 2;

  draw_map();
  draw_char(px, py, '@');
  SMS_displayOn();

  while (true) {
    kp = SMS_getKeysPressed();

    SMS_waitForVBlank();

    draw_char(px, py, map[py][px]);

    if (kp & PORT_A_KEY_UP) { move_to(px, py - 1); }
    if (kp & PORT_A_KEY_DOWN) { move_to(px, py + 1); }
    if (kp & PORT_A_KEY_LEFT) { move_to(px - 1, py); }
    if (kp & PORT_A_KEY_RIGHT) { move_to(px + 1, py); }

    draw_char(px, py, '@');
  }
}

void load_font (void) {
	unsigned char i, j;
	unsigned char buffer[32], *o, *d;

	o = font_fnt;
	for (i = 0; i != 96; i++) {
		d = buffer;
		for (j = 0; j != 8; j++) {
			*d = *o; d++;	o++;
			*d = 0;	d++;
			*d = 0;	d++;
			*d = 0;	d++;
		}
		SMS_loadTiles(buffer, i, 32);
	}
}

void main(void) {
  unsigned char i;

  load_font();

  for (i=0;i<16;i++)
    SMS_setBGPaletteColor(i,0x00);    // black
  SMS_setBGPaletteColor(01,0x3f);     // white

  simple_rl();
}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,1, 2015,8,27, "Haroldo-OK\\2015", "SimpleRL for SSM",
  "This is a port of SimpleRL to Sega Master System - https://github.com/haroldo-ok/SimpleRL.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
