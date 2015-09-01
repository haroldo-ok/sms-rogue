#include <stdlib.h>
#include <stdbool.h>
#include "SMSlib/src/SMSlib.h"
#include "gfx.h"

#define PF_OFFSET_X 0
#define PF_OFFSET_Y 0

char map[24][32];

void draw_map() {
  char *o;
  unsigned int buffer[32], *d;
  unsigned int i, j;

  o = map[0];
  for (i = 0; i != 24; i++) {
    d = buffer;
    for (j = 0; j != 32; j++) {
      *d = *o - 32;
      o++; d++;
    }
    SMS_loadTileMap (0, i, buffer, 64);
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

  draw_room(0, 0, 4, 5);
  draw_room(4, 8, 7, 6);
  draw_room(12, 1, 6, 5);
  draw_room(12, 9, 7, 5);
  draw_corridor_y(2, 4, 6, 8);
  draw_corridor_y(13, 9, 16, 5);
  draw_corridor_x(3, 1, 12, 4);
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
