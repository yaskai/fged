#include <stdint.h>
#include "raylib.h"
#include "cursor.h"
#include "object.h"
#include "sprites.h"

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

#define BUF_ENT_CAP_INIT	32	
#define BUF_ACTION_CAP_INIT	32

typedef struct {
	uint16_t ent_count, ent_cap;
	uint16_t action_count, action_cap;

	uint16_t curr_action;
	BufferAction *actions;

	Entity *entities;

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
	SpriteLoader *sl;
} Map;

void MapInit(Map *map, Camera2D *cam, Cursor *cursor, SpriteLoader *sl);
void MapUpdate(Map *map);
void MapDraw(Map *map);
void MapClose(Map *map);

void MapAddBuffer(Map *map);
void MapRemoveBuffer(Map *map);

void MapDrawGrid(Map *map);

BufferAction ActionMake();
void ActionApply(BufferAction *action, MapBuffer *buffer);
void ActionUndo(BufferAction *action, MapBuffer *buffer); 
void ActionRedo(BufferAction *action, MapBuffer *buffer);

#endif
