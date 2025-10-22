#include <stdint.h>
#include "raylib.h"
#include "sprites.h"

#ifndef OBJECT_H_
#define OBJECT_H_

enum OBJECT_TYPES {
	ASTEROID,
	PLAYER,
	SPAWNER_FISH,
	SPAWNER_ITEM
};

#define OBJ_ENTRY_HOVERED 	0x01
#define OBJ_ENTRY_PRESSED	0x02

#define OBJ_ENTRY_W			80
#define OBJ_ENTRY_H			80

typedef struct {
	uint8_t flags;
	uint8_t type;

	uint8_t frame_id;
	Spritesheet *spritesheet;

	char *label;
} ObjectEntry;

#endif
