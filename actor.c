#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "SMSlib/src/SMSlib.h"
#include "map.h"
#include "actor.h"
#include "behavior.h"

actor actors[ACTOR_COUNT];
unsigned char actor_count = 0;

actor *player;

void init_actors() {
  unsigned char i;
  actor *p = actors;

  for (i = ACTOR_COUNT; i; i--) {
    p->hp = 0;
    p++;
  }
}

actor *create_actor(unsigned char x, unsigned char y, char ch) {
  actor *p = actors;
  unsigned int i;

  for (i = 0; i != actor_count && p->hp; i++) {
    p++;
  }

  if (i == actor_count) {
    actor_count++;
  }

  p->x = x;
  p->y = y;

  p->ch = ch;
  p->ground_ch = map[y][x];
  map[y][x] = p->ch;

  p->hp = 1;
  p->dirty = true;
  p->handler = NULL;
  p->on_interact = NULL;

  return p;
}

actor *create_actor_somewhere(char ch) {
  unsigned char x, y;
  section *sec;

  do {
    x = rand() % SEC_COUNT_X;
    y = rand() % SEC_COUNT_Y;
    sec = &sections[y][x];

    x = sec->c.x1 + 1 + (rand() % (sec->c.x2 - sec->c.x1));
    y = sec->c.y1 + 1 + (rand() % (sec->c.y2 - sec->c.y1));
  } while (!is_ground(map[y][x]));

  return create_actor(x, y, ch);
}

actor *actor_at(unsigned char x, unsigned char y) {
  actor *p = actors;
  unsigned int i;

  if (!is_actor(map[y][x])) {
    return NULL;
  }

  for (i = 0; i != actor_count; i++) {
    if (p->hp && p->x == x && p->y == y) {
      return p;
    }
    p++;
  }

  return NULL;
}

bool can_move_actor(actor *p, char dx, char dy) {
  unsigned char x = p->x + dx;
  unsigned char y = p->y + dy;
  return is_ground(map[y][x]);
}

void move_actor(actor *p, char dx, char dy) {
  unsigned char x = p->x + dx;
  unsigned char y = p->y + dy;
  actor *other = actor_at(x, y);

  if (other) {
    if (other->on_interact) {
      other->on_interact(other, p);
    }
    return;
  }

  if (can_move_actor(p, dx, dy)) {
      map[p->y][p->x] = p->ground_ch;
      p->x = x; p->y = y;
      map[p->y][p->x] = p->ch;
  }
}

void move_actors() {
  actor *p;
  unsigned char i;

  for (i = 0, p = actors; i != actor_count; i++, p++) {
    if (p->hp && p->handler) {
      p->handler(p);
    }
  }
}

void draw_actors() {
  actor *p;
  unsigned char i;

  for (i = 0, p = actors; i != actor_count; i++, p++) {
    if (p->hp) {
      SMS_addSprite (p->x << 3, p->y << 3, p->ch - 32);
    }
  }
}

void create_player() {
  player = create_actor_somewhere('@');
  player->handler = act_move_keys;
  player->hp = 5;
}

void create_enemy() {
  actor *enm = create_actor_somewhere('e');
  enm->handler = act_move_random;
  enm->on_interact = itr_enemy;
}
