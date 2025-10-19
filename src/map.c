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

		float tile_size = 128;
		DrawRectangleLines(c * tile_size, r * tile_size, tile_size, tile_size, RAYWHITE);
	}
}

