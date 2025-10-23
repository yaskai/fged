#include <stdint.h>
#include "raylib.h"

#ifndef SPRITES_H_ 
#define SPRITES_H_

#define SPR_FLIP_X	   	0x01
#define SPR_FLIP_Y	   	0x02

typedef struct {
	uint8_t flags;
	uint8_t frame_count;		// Number of frames
	uint8_t cols, rows;			// Number of columns and rows

	uint8_t frame_w, frame_h;	// Width and height of frames

	Texture2D texture;			// Source image
} Spritesheet;

Spritesheet SpritesheetCreate(char *texture_path, Vector2 frame_dimensions);
void SpritesheetClose(Spritesheet *spritesheet);

void DrawSprite(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, uint8_t flags);
void DrawSpritePro(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, float rotation, uint8_t flags);
void DrawSpriteRec(Spritesheet *spritesheet, uint8_t frame_index, Rectangle dest, uint8_t flags);

uint8_t FrameIndex(Spritesheet *spritesheet, uint8_t c, uint8_t r);
Rectangle GetFrameRec(uint8_t idx, Spritesheet *spritesheet);

#define SPR_LOADER_INIT_CAP	32

enum SPRITESHEET_NAMES {
	SPR_ASTEROID,
	SPR_PLAYER,
	SPR_FISH,
	SPR_ITEM,
};

typedef struct {
	uint16_t count;
	uint16_t capacity;

	Spritesheet *spritesheets;
} SpriteLoader;

void SpriteLoaderInit(SpriteLoader *sl);
void SpriteLoaderClose(SpriteLoader *sl);

void LoadSpriteheet(SpriteLoader *sl, char *path, Vector2 frame_dimensions);
void LoadAllSprites(SpriteLoader *sl);

#endif 
