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
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !(map->cursor->flags & CURSOR_ON_UI)) {
		Entity *ents = (Entity*)malloc(sizeof(Entity));
		ents[0] = (Entity) {
			.flags = (ENT_ACTIVE),
			.type = ASTEROID,
			.properties = 0,
			.frame_id = 0,
			.rotation = 0,
			.position = map->cursor->world_pos,
			.spritesheet = &map->sl->spritesheets[SPR_ASTEROID],
		};

		ents[0].position.x -= ents[0].spritesheet->frame_w * 0.5f;
		ents[0].position.y -= ents[0].spritesheet->frame_h * 0.5f;

		BufferAction test_action = (BufferAction) {
			.ent_count_prev = 0,
			.ent_count_curr = 1,
			.ents_prev = NULL,
			.ents_curr = ents
		};

		ActionApply(&test_action, &map->buffers[map->active_buffer]);
	}
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

BufferAction ActionMake() {
	BufferAction action;
	return action;
}

// Apply an action to a buffer
void ActionApply(BufferAction *action, MapBuffer *buffer) {
	// Increment action count and current action index
	action->id = buffer->curr_action++;
	buffer->action_count++;

	// Resize array if needed
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

	// Update buffer's entity count
	buffer->ent_count += action->ent_count_curr;
	
	// Copy action's entity data to buffer
	for(uint16_t i = 0; i < action->ent_count_curr; i++) {
		uint16_t id = i + (buffer->ent_count - action->ent_count_curr);	
		buffer->entities[id] = action->ents_curr[i];
	}

	// Add action onto buffer's action history 
	buffer->actions[buffer->curr_action] = *action;	

	printf("Applied action\n");
}

// Undo an action in buffer
void ActionUndo(BufferAction *action, MapBuffer *buffer) {
}

// Redo an action in buffer
void ActionRedo(BufferAction *action, MapBuffer *buffer) {
}

