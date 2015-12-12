#ifndef _MAP_H
#define _MAP_H

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

typedef struct _box {
  unsigned char x1, y1, x2, y2;
} box;

typedef struct _section {
  box c;
} section;

extern section sections[SEC_COUNT_Y][SEC_COUNT_Y];
extern char map[PF_HEIGHT][PF_WIDTH];

extern bool moved_downstairs;

extern void clear_map();
extern void generate_map();
extern void draw_map();

extern bool is_wall(char ch);
extern bool is_ground(char ch);
extern bool is_actor(char ch);

#endif // _MAP_H
