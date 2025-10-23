#include <stdint.h>
#include "raylib.h"
#include "cursor.h"
#include "object.h"

#ifndef MAP_H_
#define MAP_H_

#define MAP_SHOW_GRID	0x01

typedef struct {
	uint16_t id;

	uint16_t ent_count_prev;
	uint16_t ent_count_curr;

	Entity *ents_prev;
	Entity *ents_curr;
} BufferAction;

#define BUF_SAVED		0x01 

typedef struct {
	uint16_t ent_count, ent_cap;
	uint16_t action_count, action_cap;

	uint16_t curr_action;
	BufferAction *actions;

	char name[32];
} MapBuffer;

typedef struct {
	uint8_t flags;

	short buffer_count;
	short buffer_cap;
	short active_buffer;

	MapBuffer *buffers;

	Camera2D *cam;
	Cursor *cursor;
} Map;

void MapInit(Map *map, Camera2D *cam, Cursor *cursor);
void MapUpdate(Map *map);
void MapDraw(Map *map);
void MapClose(Map *map);

void MapAddBuffer(Map *map);
void MapRemoveBuffer(Map *map);

void MapDrawGrid(Map *map);

void BufActionApply(BufferAction *action, MapBuffer *buffer);
void BufActionUndo(BufferAction *action, MapBuffer *buffer); 
void BufActionRedo(BufferAction *action, MapBuffer *buffer);

#endif
