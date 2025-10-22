#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "sprites.h"

// Make a spritesheet, slice frames
Spritesheet SpritesheetCreate(char *texture_path, Vector2 frame_dimensions) {
	Texture2D texture = LoadTexture(texture_path);

	if(!IsTextureValid(texture)) {
		printf("file missing: %s\n", texture_path);	
		return (Spritesheet){0};
	}

	uint8_t cols = texture.width  / frame_dimensions.x;
	uint8_t rows = texture.height / frame_dimensions.y;
	
	return (Spritesheet) {
		.flags = (0),
		.frame_w = frame_dimensions.x,
		.frame_h = frame_dimensions.y,
		.cols = cols,
		.rows = rows,
		.frame_count = (cols * rows),
		.texture = texture
	};
}

// Unload spritesheet texture
void SpritesheetClose(Spritesheet *spritesheet) {
	UnloadTexture(spritesheet->texture);
}

// Draw a spritesheet frame
void DrawSprite(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, uint8_t flags) {
	Rectangle src_rec = GetFrameRec(frame_index, spritesheet);
	if(flags & SPR_FLIP_X) src_rec.width  *= -1;
	if(flags & SPR_FLIP_Y) src_rec.height *= -1;

	DrawTextureRec(spritesheet->texture, src_rec, position, WHITE);	
}

// Draw a spritesheet frame (with rotation)
void DrawSpritePro(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, float rotation, uint8_t flags) {
	Rectangle src_rec = GetFrameRec(frame_index, spritesheet);
	if(flags & SPR_FLIP_X) src_rec.width  *= -1;
	if(flags & SPR_FLIP_Y) src_rec.height *= -1;

	DrawTexturePro(
		spritesheet->texture,
		src_rec,

		(Rectangle) {
			position.x + spritesheet->frame_w * 0.5f,
			position.y + spritesheet->frame_h * 0.5f,
			spritesheet->frame_w,
			spritesheet->frame_h
		},

		(Vector2){spritesheet->frame_w * 0.5f, spritesheet->frame_h * 0.5f},
		rotation,
		WHITE
	);
}

void DrawSpriteRec(Spritesheet *spritesheet, uint8_t frame_index, Rectangle dest, uint8_t flags) {
	Rectangle source = GetFrameRec(frame_index, spritesheet);
	DrawTexturePro(spritesheet->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
}

// Get index of a frame from column and row values
uint8_t FrameIndex(Spritesheet *spritesheet, uint8_t c, uint8_t r) {
	return (c + r * spritesheet->cols);
}

// Get frame rectangle from index 
Rectangle GetFrameRec(uint8_t idx, Spritesheet *spritesheet) {
	uint8_t c = idx % spritesheet->cols, r = idx / spritesheet->cols;

	return (Rectangle) {
		.x  = c * spritesheet->frame_w,
		.y  = r * spritesheet->frame_h,
		.width  = spritesheet->frame_w,
		.height = spritesheet->frame_h
	};			
}

// Initialize sprite loader
void SpriteLoaderInit(SpriteLoader *sl) {
	sl->count = 0;
	sl->capacity = SPR_LOADER_INIT_CAP;
	sl->spritesheets = (Spritesheet*)malloc(sizeof(Spritesheet) * sl->capacity);

	LoadAllSprites(sl);
}

// Unload textures, free allocated memory
void SpriteLoaderClose(SpriteLoader *sl) {
	
	free(sl->spritesheets);
}

// load spritesheet and add to sprite loader sheet array
void LoadSpriteheet(SpriteLoader *sl, char *path, Vector2 frame_dimensions) {
	// Initialize new spritesheet
	Spritesheet spritesheet = SpritesheetCreate(path, frame_dimensions);
	
	// Increment count
	sl->count++;	

	// Resize array if over capacity 
	if(sl->count > sl->capacity - 1) {
		sl->capacity *= 2;
		
		Spritesheet *ptr_spritesheets;
		ptr_spritesheets = realloc(sl->spritesheets, sizeof(Spritesheet) * sl->capacity);	

		sl->spritesheets = ptr_spritesheets;	
	}

	// Add new spritesheet to array
	sl->spritesheets[sl->count-1] = spritesheet;
}

void LoadAllSprites(SpriteLoader *sl) {
	LoadSpriteheet(sl, "resources/sprites/asteroid00.png", (Vector2){128, 128});
	LoadSpriteheet(sl, "resources/sprites/player_sheet.png", (Vector2){64, 64});
}

