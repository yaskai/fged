#include <stdint.h>
#include <stdio.h>
#include "raylib.h"
#include "sprites.h"

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

void SpritesheetClose(Spritesheet *spritesheet) {
	UnloadTexture(spritesheet->texture);
}

void DrawSprite(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, uint8_t flags) {
	Rectangle src_rec = GetFrameRec(frame_index, spritesheet);
	if(flags & SPR_FLIP_X) src_rec.width  *= -1;
	if(flags & SPR_FLIP_Y) src_rec.height *= -1;

	DrawTextureRec(spritesheet->texture, src_rec, position, WHITE);	
}

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

uint8_t FrameIndex(Spritesheet *spritesheet, uint8_t c, uint8_t r) {
	return (c + r * spritesheet->cols);
}

Rectangle GetFrameRec(uint8_t idx, Spritesheet *spritesheet) {
	uint8_t c = idx % spritesheet->cols, r = idx / spritesheet->cols;

	return (Rectangle) {
		.x  = c * spritesheet->frame_w,
		.y  = r * spritesheet->frame_h,
		.width  = spritesheet->frame_w,
		.height = spritesheet->frame_h
	};			
}

