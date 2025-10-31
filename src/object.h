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

#define PROP_UNIQUE			0x01
#define PROP_IS_SPAWNER		0x02

typedef struct {
	uint8_t flags, type;

	uint8_t frame_id;
	Spritesheet *spritesheet;

	char *label;
} ObjectEntry;

// Editor entity state flags
#define ENT_ACTIVE			0x01
#define ENT_MOVING			0x02
#define ENT_SCALING			0x04  
#define ENT_SPINNING		0x08
#define ENT_PROPEDIT		0x10

// Spawner rarity property flags
#define ALLOW_COMMON		0x01
#define ALLOW_UNCOMMON		0x02	
#define ALLOW_RARE			0x04	
#define ALLOW_LEGENDARY		0x08	

// Spawner size property flags
#define allow_size_tiny		0x01
#define allow_size_small	0x02	
#define allow_size_reg		0x04	
#define allow_size_large	0x08	
#define allow_size_bmoth	0x10	

typedef struct {
	uint8_t flags;
	uint8_t type;
	uint8_t frame_id;	

	uint8_t rare_props;
	uint8_t size_props; 

	uint16_t id;

	float rotation;
	float scale;

	Vector2 position;

	Spritesheet *spritesheet;
} Entity; 

#endif
