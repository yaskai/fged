#include <stdint.h>
#include "raylib.h"

#ifndef OBJECT_H_
#define OBJECT_H_

typedef struct {
	uint8_t flags;

	Rectangle bounds;
} Object;

#endif
