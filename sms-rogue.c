#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SMSlib/src/SMSlib.h"
#include "gfx.h"
#include "map.h"
#include "actor.h"
#include "behavior.h"

unsigned short kp;

void putchar (char c) {
	SMS_setTile(c - 32);
}

void draw_char(unsigned char x, unsigned char y, char c) {
  SMS_setTileatXY(x + PF_OFFSET_X, y + PF_OFFSET_Y, c - 32);
}

void title_screen() {
  unsigned int seed = 1;

  SMS_setNextTileatXY(6, 5);
  puts("**** SMS-Rogue ****");
  SMS_setNextTileatXY(6, 7);
  puts("** Press any key **");
  SMS_displayOn();

  while (SMS_getKeysStatus()) {
    SMS_waitForVBlank();
    seed++;
  }
  while (!SMS_getKeysStatus()) {
    SMS_waitForVBlank();
    seed++;
  }
  while (SMS_getKeysStatus()) {
    SMS_waitForVBlank();
    seed++;
  }

  SMS_displayOff();

  srand(seed ? seed : 1);
}

void simple_rl(void)
{
  clear_map();
  init_actors();

  generate_map();
  draw_map();

  create_player();

  create_actor_somewhere('>');
  create_enemy();
  create_enemy();
  create_enemy();
  create_enemy();
  create_enemy();
  create_enemy();

  SMS_displayOn();

  while (true) {
    kp = SMS_getKeysPressed();

    SMS_waitForVBlank();

    if (kp) {
      move_actors();
    }

    SMS_initSprites();

    draw_actors();

    SMS_finalizeSprites();
    SMS_copySpritestoSAT();
  }
}

void load_font (void) {
  unsigned char i, j;
	unsigned char buffer[32], *o, *d;

	o = font_fnt;
	for (i = 0; i != 96; i++) {
		d = buffer;
		for (j = 0; j != 8; j++) {
			*d = *o; d++;
			*d = ~(*o);	d++;
			*d = 0;	d++;
			*d = 0;	d++;
      o++;
		}
		SMS_loadTiles(buffer, i, 32);
    SMS_loadTiles(buffer, i + 256, 32);
	}
}

void main(void) {
  unsigned char i;

  load_font();

  for (i=0;i<16;i++) {
    SMS_setBGPaletteColor(i,0x00);    // black
    SMS_setSpritePaletteColor(i,0x00);    // black
  }
  SMS_setBGPaletteColor(01,0x3f);     // white
  SMS_setSpritePaletteColor(01,0x3f);     // white

  title_screen();
  simple_rl();
}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,7, 2015,12,10, "Haroldo-OK\\2015", "SMS-Rogue",
  "A roguelike for the Sega Master System - https://github.com/haroldo-ok/sms-rogue.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
