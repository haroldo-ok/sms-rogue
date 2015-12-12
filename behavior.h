#ifndef _BEHAVIOR_H
#define _BEHAVIOR_H

extern void act_move_keys(actor *p);
extern void act_move_random(actor *p);

extern void itr_player(actor *self, actor *other);
extern void itr_enemy(actor *self, actor *other);
extern void itr_down_stairs(actor *self, actor *other);

#endif // _BEHAVIOR_H
