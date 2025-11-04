#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "include/raygui.h"
#include "map.h"
#include "sprites.h"

float temp_scale = 1.0f;
float temp_spin  = 0.0f;

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

	// Add a new entity 
	if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_A) && !(map->cursor->flags & CURSOR_ON_UI)) 
		BufAddEntity(buffer->ent_prototype.type, 0, map->cursor->world_pos, buffer->ent_prototype.spritesheet, buffer);

	if(buffer->ent_selected > -1 && buffer->entities[buffer->ent_selected].flags & ENT_ACTIVE) {
		Entity *ent = &buffer->entities[buffer->ent_selected];
		
		if(IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE))
			BufRemoveEntity(buffer, &buffer->entities[buffer->ent_selected]);

		if(IsKeyPressed(KEY_M))
			ent->flags ^= ENT_MOVING;

		if(IsKeyPressed(KEY_S)) {
			ent->flags ^= ENT_SCALING;
			//map->cursor->flags ^= CURSOR_LOCK_ZOOM;

			if(!(ent->flags & ENT_SCALING)) {
				BufScaleEntity(buffer, ent, temp_scale);
				map->cursor->flags ^= CURSOR_LOCK_ZOOM;
			}
		}

		if(ent->flags & ENT_SCALING) {
			map->cursor->flags |= CURSOR_LOCK_ZOOM;

			float scroll = GetMouseWheelMove();
			if(fabsf(scroll) > 0) {
				temp_scale += scroll;
			}

			temp_scale = Clamp(temp_scale, 0.1f, 10.0f);
		}

		if(IsKeyPressed(KEY_A)) {
			ent->flags ^= ENT_SPINNING;
			//map->cursor->flags ^= CURSOR_LOCK_ZOOM;

			if(!(ent->flags & ENT_SPINNING)) {
				Entity ent_modifed = *ent;
				ent_modifed.rotation = temp_spin;

				BufModifyEntity(buffer, ent, ent_modifed);
				map->cursor->flags ^= CURSOR_LOCK_ZOOM;
			}
		}
		
		if(ent->flags & ENT_SPINNING) {
			map->cursor->flags |= CURSOR_LOCK_ZOOM;

			float scroll = GetMouseWheelMove();
			if(fabs(scroll) > 0) {
				temp_spin += (scroll * 2);
			}

			if(temp_spin < 0) temp_spin = 360;
			else if(temp_spin > 360) temp_spin = 0;
		}

		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ent->flags & ENT_MOVING) 
			BufTranslateEntity(buffer, ent, map->cursor->world_pos);
	}

	if(IsKeyPressed(KEY_Z)) ActionUndo(buffer);
	if(IsKeyPressed(KEY_R)) ActionRedo(buffer);

	if(map->cursor->flags & CURSOR_BOX_OPEN) {
		if(IsKeyPressed(KEY_C))
			Copy(buffer, map->cursor->selection_rec_final);
	}

	if(buffer->cb_count > 0 && IsKeyPressed(KEY_V)) 
		Paste(buffer, map->cursor->world_pos);

	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !(map->cursor->flags & CURSOR_ON_UI)) {
		if(buffer->ent_selected > -1)
			buffer->entities[buffer->ent_selected].flags &= ~ENT_SCALING;

		buffer->ent_selected = buffer->ent_hovered;
		
		temp_scale = buffer->entities[buffer->ent_selected].scale;
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

		if(
			(ent->flags & ENT_MOVING) 	||
			(ent->flags & ENT_SCALING)  ||
			(ent->flags & ENT_SPINNING)
		) continue;

		DrawSpritePro(ent->spritesheet, ent->frame_id, ent->position, ent->rotation, ent->scale, WHITE, 0);
	}

	if(buffer->ent_selected > -1) {
		Entity *ent = &buffer->entities[buffer->ent_selected];
		if(!(ent->flags & ENT_ACTIVE)) return;

		Rectangle rec = (Rectangle) {
			.x = ent->position.x,
			.y = ent->position.y,
			.width = ent->spritesheet->frame_w,
			.height = ent->spritesheet->frame_h
		};
		
		Color color = ColorAlpha(ORANGE, 0.8f);
		if(ent->flags & ENT_MOVING) {
			rec.x = map->cursor->world_pos.x - ent->spritesheet->frame_w * 0.5f;
			rec.y = map->cursor->world_pos.y - ent->spritesheet->frame_h * 0.5f;
			color = WHITE;
		}
		
		float scale = (ent->flags & ENT_SCALING) ? temp_scale : ent->scale;
		float rotation = (ent->flags & ENT_SPINNING) ? temp_spin : ent->rotation;

		DrawSpritePro(ent->spritesheet, ent->frame_id, (Vector2){rec.x, rec.y}, rotation, scale, color, 0);
	}
}

// Free memory allocated for map
void MapClose(Map *map) {
	if(map->buffers == NULL) return;  
	for(uint8_t i = 0; i < map->buffer_count; i++) free(&map->buffers[i]);
}

void MapBufferInit(MapBuffer *buffer) {
	buffer->ent_cap = BUF_ENT_CAP_INIT;
	buffer->entities = malloc(sizeof(Entity) * buffer->ent_cap);

	buffer->action_cap = BUF_ACTION_CAP_INIT;
	buffer->actions = malloc(sizeof(BufferAction) * buffer->action_cap);
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

	Entity *cb = (Entity*)malloc(sizeof(Entity) * BUF_ENT_CAP_INIT);	

	MapBuffer new_buffer = (MapBuffer){0};

	new_buffer.action_cap = BUF_ACTION_CAP_INIT;
	new_buffer.actions = actions;

	new_buffer.ent_cap = BUF_ENT_CAP_INIT;
	new_buffer.entities = entities;

	new_buffer.clipboard = cb;

	new_buffer.ent_selected = -1;

	new_buffer.ent_prototype = (Entity) { .type = ASTEROID, .spritesheet = (&map->sl->spritesheets[SPR_ASTEROID]) };

	map->buffers[map->buffer_count - 1] = new_buffer;
	map->active_buffer = map->buffer_count - 1;
}

// Close an active map buffer
void MapRemoveBuffer(Map *map, short id) {
	if(id >= map->buffer_count) return;

	MapBuffer *buffer = &map->buffers[id];

	free(buffer->actions);
	free(buffer->entities);

	for(short i = id; i < map->buffer_count - 1; i++) {
		map->buffers[i] = map->buffers[i + 1];
	}

	map->buffer_count--;
	map->active_buffer = id - 1;
}

// Save map to file
void MapWriteBuffer(Map *map, char *path) {
	FILE *pf = fopen(path, "w"); 
	if(!pf) {
		printf("ERROR: could not write to file path.\n");
		return;
	}

	if(map->active_buffer < 0) return;
	MapBuffer *buffer = &map->buffers[map->active_buffer];

	fprintf(pf, "entity_count:%d\n", buffer->ent_count);

	for(uint16_t i = 0; i < buffer->ent_count; i++) {
		Entity *ent = &buffer->entities[i];
		if(!(ent->flags & ENT_ACTIVE)) continue;

		char *type_str = "";
		switch(ent->type) {
			case ASTEROID:
				type_str = "asteroid";
				break;

			case PLAYER:
				type_str = "player";
				break;

			case SPAWNER_FISH:
				type_str = "spawner_fish";
				break;

			case SPAWNER_ITEM:
				type_str = "spawner_item";
				break;
		}
		
		fprintf(pf, "\n");
		fprintf(pf, "[%s]\n", type_str);
		fprintf(pf, "flags: %d\n", ent->flags);
		fprintf(pf, "type: %d\n", ent->type);
		fprintf(pf, "rare_props: %d\n", ent->rare_props);
		fprintf(pf, "size_props: %d\n", ent->size_props);
		fprintf(pf, "rotation: %f\n", ent->rotation);
		fprintf(pf, "scale: %f\n", ent->scale);
		fprintf(pf, "position: %f, %f\n", ent->position.x, ent->position.y);
		fprintf(pf, "frame: %d\n", ent->frame_id);
		fprintf(pf, "spritesheet: %d\n", ent->spritesheet->id);
		fprintf(pf, "\n");
	}

	fclose(pf);
}

// Read map from file
void MapReadBuffer(Map *map, char *path) {
	FILE *pf = 	fopen(path, "r");
	if(!pf) {
		printf("ERROR: could not read from file path.\n");
		return;
	}

	MapBuffer buffer = (MapBuffer){0};
	MapBufferInit(&buffer);

	uint16_t curr_id = 0;
	Entity *curr_ent = NULL;

	char line[128];
	while(fgets(line, sizeof(line), pf)) {
		if(line[0] == '[') {
			if(curr_id + 1 > buffer.ent_cap) {
				buffer.ent_cap *= 2;
				Entity *new_ptr = realloc(buffer.entities, sizeof(Entity) * buffer.ent_cap);
				buffer.entities = new_ptr;
			}

			curr_ent = &buffer.entities[curr_id++];
		}

		char *split = strchr(line, ':'); 
		if(!split) continue;
		
		*split = '\0';
		char *key = line;
		char *val = split + 1;

		if(!strcmp(key, "flags")) {
			int flags;
			sscanf(val, "%d", &flags);
			curr_ent->flags = flags;
		} else if(!strcmp(key, "type")) {
			int type;
			sscanf(val, "%d", &type);
			curr_ent->type = type;
		} else if(!strcmp(key, "rare_props")) {
			int rare_props;
			sscanf(val, "%d", &rare_props);
			curr_ent->rare_props = rare_props;
		} else if(!strcmp(key, "size_props")) {
			int size_props;
			sscanf(val, "%d", &size_props);
			curr_ent->size_props = size_props;
		} else if(!strcmp(key, "rotation")) {
			float rotation;
			sscanf(val, "%f", &rotation);
			curr_ent->rotation = rotation;
		} else if(!strcmp(key, "scale")) {
			float scale;
			sscanf(val, "%f", &scale);
			curr_ent->scale = scale;
		} else if(!strcmp(key, "position")) {
			Vector2 position;
			sscanf(val, "%f, %f", &position.x, &position.y);
			curr_ent->position = position;
		} else if(!strcmp(key, "spritesheet")) {
			int spritesheet_id;
			sscanf(val, "%d", &spritesheet_id);
			curr_ent->spritesheet = &map->sl->spritesheets[spritesheet_id];
		} else if(!strcmp(key, "frame")) {
			int frame_id;
			sscanf(val, "%d", &frame_id);
			curr_ent->frame_id = frame_id;
		}
	}

	buffer.ent_count = curr_id + 1;

	map->buffer_count++;

	if(map->buffer_count > map->buffer_cap - 1) {
		map->buffer_cap *= 2;

		MapBuffer *buffers_ptr = (MapBuffer*)realloc(map->buffers, sizeof(MapBuffer) * map->buffer_cap);
		map->buffers = buffers_ptr;
	}

	map->buffers[map->buffer_count - 1] = buffer;

	fclose(pf);
}

// Write map buffer to binary file
void MapWriteBufferBin(Map *map, char *path) {
}

// Read map buffer from binary file
void MapReadBufferBin(Map *map, char *path) {
}

// Draw a grid of lines
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

		Entity *cb_ptr = realloc(buffer->clipboard, sizeof(Entity) * buffer->ent_cap);
		buffer->clipboard = cb_ptr;
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

void BufAddEntity(uint8_t type, float rotation, Vector2 position, Spritesheet *ss, MapBuffer *buffer) {
	Entity ent = (Entity){0};
	ent.flags = (ENT_ACTIVE);
	ent.type = type;
	ent.spritesheet = ss;

	ent.rotation = rotation;
	ent.scale = 1;

	ent.position = position = (Vector2) {
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

void BufTranslateEntity(MapBuffer *buffer, Entity *entity, Vector2 pos) {
	entity->flags &= ~ENT_MOVING;

	BufferAction move_action = (BufferAction) {
		.type = ACTION_MODIFY,
		.ent_count = 1,
		.ents_prev = malloc(sizeof(Entity)),
		.ents_curr = malloc(sizeof(Entity)),
	};

	Entity new_ent = *entity;
	new_ent.position = (Vector2){pos.x - entity->spritesheet->frame_w * 0.5f, pos.y - entity->spritesheet->frame_h * 0.5f};

	move_action.ents_prev[0] = *entity;
	move_action.ents_curr[0] = new_ent;

	ActionApply(&move_action, buffer);	
}

void BufScaleEntity(MapBuffer *buffer, Entity *entity, float scale) {
	entity->flags &= ~ENT_MOVING;

	BufferAction scale_action = (BufferAction) {
		.type = ACTION_MODIFY,
		.ent_count = 1,
		.ents_prev = malloc(sizeof(Entity)),
		.ents_curr = malloc(sizeof(Entity)),
	};

	Entity new_ent = *entity;
	new_ent.scale = scale;

	scale_action.ents_prev[0] = *entity;
	scale_action.ents_curr[0] = new_ent;

	ActionApply(&scale_action, buffer);	
}

void BufModifyEntity(MapBuffer *buffer, Entity *original, Entity modified) {
	BufferAction action = (BufferAction) {
		.type = ACTION_MODIFY,	
		.ent_count = 1,
		.ents_prev = malloc(sizeof(Entity)),
		.ents_curr = malloc(sizeof(Entity))
	};

	action.ents_prev[0] = *original;
	action.ents_curr[0] = modified;

	ActionApply(&action, buffer);
}

void Copy(MapBuffer *buffer, Rectangle rec) {
	buffer->cb_count = 0;

	for(uint16_t i = 0; i < buffer->ent_count; i++) {
		Entity *ent = &buffer->entities[i];
		if(!(ent->flags & ENT_ACTIVE)) continue;

		Rectangle ent_rec = (Rectangle) {
			.x = ent->position.x,
			.y = ent->position.y,
			.width = ent->spritesheet->frame_w * ent->scale,
			.height = ent->spritesheet->frame_h * ent->scale
		};	

		if(CheckCollisionRecs(rec, ent_rec)) {
			buffer->clipboard[buffer->cb_count++] = *ent;

			Vector2 pos_local = (Vector2) { ent->position.x - rec.x, ent->position.y - rec.y };
			buffer->clipboard[buffer->cb_count - 1].position = pos_local;
		}
	}
}

void Paste(MapBuffer *buffer, Vector2 pos) {
	Entity *ents_prev = malloc(sizeof(Entity) * buffer->cb_count);
	for(uint16_t i = 0; i < buffer->cb_count; i++) ents_prev[i] = (Entity){0};

	Entity *ents_curr = malloc(sizeof(Entity) * buffer->cb_count);
	for(uint16_t i = 0; i < buffer->cb_count; i++) {
		ents_curr[i] = buffer->clipboard[i];
		ents_curr[i].position = (Vector2){ents_curr[i].position.x + pos.x, ents_curr[i].position.y + pos.y};
	}

	BufferAction paste_action = (BufferAction) {
		.type = ACTION_INSERT,
		.ent_count = buffer->cb_count,
		.ents_prev = ents_prev,
		.ents_curr = ents_curr
	};

	ActionApply(&paste_action, buffer);
}

