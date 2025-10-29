#include <stdint.h>
#include "raylib.h"
#include "cursor.h"
#include "object.h"
#include "sprites.h"

#ifndef MAP_H_
#define MAP_H_

#define MAP_SHOW_GRID	0x01

enum ACTION_TYPE : uint8_t {
	ACTION_INSERT,
	ACTION_REMOVE,
	ACTION_MODIFY	
};

typedef struct {
	uint8_t type;
	uint16_t ent_count;

	Entity *ents_prev;
	Entity *ents_curr;
} BufferAction;

#define BUF_SAVED	0x01 

#define BUF_ENT_CAP_INIT		32	
#define BUF_ACTION_CAP_INIT		32

typedef struct {
	uint16_t ent_count, ent_cap;
	uint16_t action_count, action_cap;

	int16_t ent_hovered;
	int16_t ent_selected;

	uint16_t curr_action;
	BufferAction *actions;

	Entity ent_prototype;
	Entity *entities;

	uint16_t cb_count;
	Rectangle cb_rec;
	Entity *clipboard;

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
void MapRemoveBuffer(Map *map, short id);

void MapWriteBuffer(Map *map, char *path);
void MapReadBuffer(Map *map, char *path);

void MapDrawGrid(Map *map);

void ActionApply(BufferAction *action, MapBuffer *buffer);
void ActionUndo(MapBuffer *buffer); 
void ActionRedo(MapBuffer *buffer);

void BufAddEntity(uint8_t type, uint8_t properties, float rotation, Vector2 position, Spritesheet *ss, MapBuffer *buffer);
void BufRemoveEntity(MapBuffer *buffer, Entity *entity);

void BufTranslateEntity(MapBuffer *buffer, Entity *entity, Vector2 pos);
void BufScaleEntity(MapBuffer *buffer, Entity *entity, float scale);

void Copy(MapBuffer *buffer, Rectangle rec);
void Paste(MapBuffer *buffer, Vector2 pos);

#endif
