#include <stdint.h>
#include "raylib.h"

#ifndef CURSOR_H_
#define CURSOR_H_

#define CURSOR_ON_UI		0x01
#define CURSOR_PAN			0x02
#define CURSOR_SELECTION	0x04

typedef struct {
	uint8_t flags;

	Vector2 screen_pos;
	Vector2 world_pos;

	Vector2 pan_pos;
	Vector2 box_origin;

	Color color;

	Rectangle selection_rec_edit;
	Rectangle selection_rec_final;
} Cursor;

void CursorInit(Cursor *cursor);

void CursorUpdate(Cursor *cursor, Camera2D *cam, float dt);
void CursorCameraControls(Cursor *cursor, Camera2D *cam, float dt);

void CursorDraw(Cursor *cursor);
void CursorDrawPointer(Cursor *cursor);

void SelectionBox(Cursor *cursor);

#endif
