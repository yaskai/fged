#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "map.h"
#include "sprites.h"

void MapInit(Map *map, Camera2D *cam, Cursor *cursor, SpriteLoader *sl) {
	map->cam = cam;
	map->cursor = cursor;
	map->sl = sl;

	map->active_buffer = -1;
	map->buffer_cap = 16;
	map->buffers = malloc(sizeof(MapBuffer) * map->buffer_cap);
}

void MapUpdate(Map *map) {
	MapBuffer *buffer = &map->buffers[map->active_buffer];
		
	buffer->ent_hovered = -1;
	for(uint16_t i = 0; i < buffer->ent_count; i++) {
		Entity *ent = &buffer->entities[i];
		if(!(ent->flags & ENT_ACTIVE)) continue;

		Rectangle rec = (Rectangle) {
			.x = ent->position.x,
			.y = ent->position.y,
			.width = ent->spritesheet->frame_w,
			.height = ent->spritesheet->frame_h
		};

		if(CheckCollisionPointRec(map->cursor->world_pos, rec)) 
			buffer->ent_hovered = i;
	}

	// Delete hovered entity
	if(buffer->ent_hovered > -1 && (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)))
		BufRemoveEntity(buffer, &buffer->entities[buffer->ent_hovered]);

	// Add a new entity 
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !(map->cursor->flags & CURSOR_ON_UI)) 
		BufAddEntity(ASTEROID, 0, 0, map->cursor->world_pos, &map->sl->spritesheets[SPR_ASTEROID], buffer);

	if(IsKeyPressed(KEY_Z)) ActionUndo(buffer);
	if(IsKeyPressed(KEY_R)) ActionRedo(buffer);
}

void MapDraw(Map *map) {
	// Draw grid background
	MapDrawGrid(map);

	if(map->active_buffer < 0) return;
	MapBuffer *buffer = &map->buffers[map->active_buffer];
	
	// Draw placed entities
	for(uint16_t i = 0; i < buffer->ent_count; i++) {
		Entity *ent = &buffer->entities[i];
		if(!(ent->flags & ENT_ACTIVE)) continue;	
		DrawSpritePro(ent->spritesheet, ent->frame_id, ent->position, ent->rotation, 0);
	}
}

// Free memory allocated for map
void MapClose(Map *map) {
	if(map->buffers == NULL) return;  
	for(uint8_t i = 0; i < map->buffer_count; i++) free(&map->buffers[i]);
}

void MapAddBuffer(Map *map) {
	map->buffer_count++;

	if(map->buffer_count > map->buffer_cap - 1) {
		map->buffer_cap *= 2;

		MapBuffer *buffers_ptr = (MapBuffer*)realloc(map->buffers, sizeof(MapBuffer) * map->buffer_cap);
		map->buffers = buffers_ptr;
	}

	BufferAction *actions = (BufferAction*)malloc(sizeof(BufferAction) * BUF_ACTION_CAP_INIT);
	Entity *entities = (Entity*)malloc(sizeof(Entity) * BUF_ENT_CAP_INIT);

	MapBuffer new_buffer = (MapBuffer){0};

	new_buffer.action_cap = BUF_ACTION_CAP_INIT;
	new_buffer.actions = actions;

	new_buffer.ent_cap = BUF_ENT_CAP_INIT;
	new_buffer.entities = entities;

	map->buffers[map->buffer_count - 1] = new_buffer;
	map->active_buffer = map->buffer_count - 1;
}

void MapRemoveBuffer(Map *map) {
}

void MapDrawGrid(Map *map) {
	for(uint16_t i = 0; i < (16 * 16); i++) {
		uint16_t c = i % 16;
		uint16_t r = i / 16;

		float tile_size = 256;
		DrawRectangleLines(c * tile_size, r * tile_size, tile_size, tile_size, GRAY);
	}
}

// Apply an action to a buffer
void ActionApply(BufferAction *action, MapBuffer *buffer) {
	// Increment action count and current action index
	buffer->curr_action++;
	buffer->action_count++;

	// Resize actions array if needed
	if(buffer->action_count > buffer->action_cap - 1) {
		buffer->action_cap *= 2;
		
		BufferAction *new_ptr = realloc(buffer->actions, sizeof(BufferAction) * buffer->action_cap);
		buffer->actions = new_ptr;
	}

	// Free all actions ahead of current if necessary
	// eg. 
	// User places entities resulting in 10 buffer actions.    
	// Undo performed 3 times making current action 7 (6 really since count begins at 0).
	// User then places another entity breaking the timeline.  
	// Action count and current place updated to maintain proper history order. 
	if(buffer->curr_action < buffer->action_count) {
		for(uint16_t i = buffer->curr_action; i < buffer->action_count; i++) {
			free(buffer->actions[i].ents_prev);
			free(buffer->actions[i].ents_curr);
		}

		buffer->action_count = buffer->curr_action;
	}

	uint16_t ents_added = (action->type == ACTION_INSERT) ? action->ent_count : 0;
	for(uint16_t i = 0; i < ents_added; i++) {
		uint16_t id = buffer->ent_count++;

		action->ents_prev[i].id = id; 
		action->ents_curr[i].id = id;
	}

	// Resize buffer's entity array if needed
	if(buffer->ent_count > buffer->ent_cap - 1) {
		buffer->ent_cap *= 2;
		
		Entity *ents_ptr = realloc(buffer->entities, sizeof(Entity) * buffer->ent_cap);
		buffer->entities = ents_ptr;
	}

	// Copy action's entity data to buffer
	for(uint16_t i = 0; i < action->ent_count; i++) {
		uint16_t id = action->ents_curr[i].id;
		buffer->entities[id] = action->ents_curr[i];
	}

	// Add action onto buffer's action history 
	buffer->actions[buffer->curr_action] = *action;	
}

// Undo an action in buffer
void ActionUndo(MapBuffer *buffer) {
	// Prevent undoing below first index
	if(buffer->curr_action < 1) return;

	BufferAction *action = &buffer->actions[buffer->curr_action];	

	// Set entities in buffer action to their previous states
	for(uint16_t i = 0; i < action->ent_count; i++) {
		uint16_t id = action->ents_prev[i].id;
		buffer->entities[id] = action->ents_prev[i];
	}

	// Decrement index in history  
	buffer->curr_action--;
}

// Redo an action in buffer
void ActionRedo(MapBuffer *buffer) {
	// Prevent redoing passed last index
	if(buffer->curr_action > buffer->action_count - 1) return;
	
	// Increment index in history  
	buffer->curr_action++;

	BufferAction *action = &buffer->actions[buffer->curr_action];
	
	for(uint16_t i = 0; i < action->ent_count; i++) {
		uint16_t id = action->ents_curr[i].id;
		buffer->entities[id] = action->ents_curr[i];		
	}
}

void BufAddEntity(uint8_t type, uint8_t properties, float rotation, Vector2 position, Spritesheet *ss, MapBuffer *buffer) {
	Entity ent = (Entity){0};
	ent.flags = (ENT_ACTIVE);
	ent.type = type;
	ent.properties = properties;
	ent.spritesheet = ss;

	ent.rotation = rotation;
	ent.position = position = (Vector2){
		position.x - ent.spritesheet->frame_w * 0.5f,
		position.y - ent.spritesheet->frame_h * 0.5f
	};

	Entity *ents_prev = malloc(sizeof(Entity)), *ents_curr = malloc(sizeof(Entity));
	ents_prev[0] = (Entity){0}, ents_curr[0] = ent;

	BufferAction action = (BufferAction) { .type = ACTION_INSERT, .ent_count = 1, .ents_prev = ents_prev, .ents_curr = ents_curr };
	ActionApply(&action, buffer);
}

void BufRemoveEntity(MapBuffer *buffer, Entity *entity) {
	uint16_t id = entity->id;
	
	Entity *ents_prev = malloc(sizeof(Entity)), *ents_curr = malloc(sizeof(Entity));
	ents_prev[0] = *entity, ents_curr[0] = (Entity){ .flags = 0, .id = entity->id };

	BufferAction action = (BufferAction) { .type = ACTION_REMOVE, .ent_count = 1, .ents_prev = ents_prev, .ents_curr = ents_curr };	
	ActionApply(&action, buffer);
}

