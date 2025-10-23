#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "map.h"

void MapInit(Map *map, Camera2D *cam, Cursor *cursor) {
	map->cam = cam;
	map->cursor = cursor;
}

void MapUpdate(Map *map) {
}

void MapDraw(Map *map) {
	MapDrawGrid(map);
}

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

	MapBuffer new_buffer = (MapBuffer){0};
	map->buffers[map->buffer_count] = new_buffer;
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
void BufActionApply(BufferAction *action, MapBuffer *buffer) {
	// Increment action count and current action index
	action->id = buffer->curr_action++;
	buffer->action_count++;

	// Resize array if needed
	if(buffer->action_count > buffer->action_cap - 1) {
		buffer->action_cap *= 2;
		
		BufferAction *new_ptr = (BufferAction*)realloc(buffer->actions, buffer->action_cap);
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

	// Add action onto buffer's action history 
	buffer->actions[buffer->curr_action] = *action;	
}

// Undo an action in buffer
void BufActionUndo(BufferAction *action, MapBuffer *buffer) {
}

// Redo an action in buffer
void BufActionRedo(BufferAction *action, MapBuffer *buffer) {
}

