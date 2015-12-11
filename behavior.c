#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "SMSlib/src/SMSlib.h"
#include "map.h"
#include "actor.h"
#include "behavior.h"

extern unsigned short kp; // TODO: Find out better way of sharing keypress data

void act_move_keys(actor *p) {
  if (kp & PORT_A_KEY_UP) { move_actor(p, 0, -1); }
  if (kp & PORT_A_KEY_DOWN) { move_actor(p, 0, 1); }
  if (kp & PORT_A_KEY_LEFT) { move_actor(p, -1, 0); }
  if (kp & PORT_A_KEY_RIGHT) { move_actor(p, 1, 0); }
}

void act_move_random(actor *p) {
  char x = 0, y = 0;
  unsigned char dir = rand() & DIR_MASK;
  unsigned char tries = 4;

  do {
    switch (dir) {
      case DIR_UP: y = -1; break;
      case DIR_DOWN: y = 1; break;
      case DIR_LEFT: x = -1; break;
      case DIR_RIGHT: x = 1; break;
    }
  } while(tries-- && !can_move_actor(p, x, y));

  if (x || y) {
    move_actor(p, x, y);
  }
}
