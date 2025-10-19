#include <stdint.h>
#include "raylib.h"
#include "cursor.h"
#include "object.h"

#ifndef MAP_H_
#define MAP_H_

#define MAP_SAVED		0x01
#define MAP_SHOW_GRID	0x02

typedef struct {
	uint16_t object_count_prev;
	uint16_t object_count_curr;

	// Object *objects_prev
	// Object *objects_curr
} BufferAction;

typedef struct {
	uint16_t object_count, object_cap;
	uint16_t action_count, action_cap;

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

#endif
