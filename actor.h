#ifndef _ACTOR_H
#define _ACTOR_H

#define ACTOR_COUNT 64

typedef struct _actor {
  char ch, ground_ch;
  unsigned char x, y;
  unsigned int hp;
  bool dirty;

  void (*handler)(struct _actor *p);
  void (*on_interact)(struct _actor *self, struct _actor *other);
} actor;

extern actor actors[ACTOR_COUNT];
extern unsigned char actor_count;

extern actor *player;

extern actor *create_actor_somewhere(char ch);
extern void create_player();
extern void create_enemy();
extern void create_down_stairs();

extern void init_actors();
extern void move_actors();
extern void draw_actors();

extern bool can_move_actor(actor *p, char dx, char dy);
extern void move_actor(actor *p, char dx, char dy);

#endif // _ACTOR_H
