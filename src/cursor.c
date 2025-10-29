#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "include/raygui.h"
#include "cursor.h"

// Initialize cursor
void CursorInit(Cursor *cursor) {
	cursor->flags = 0;
}

// Update cursor every frame
void CursorUpdate(Cursor *cursor, Camera2D *cam, float dt) {
	// Get cursor's world and screen positions
	cursor->screen_pos = GetMousePosition();
	cursor->world_pos = GetScreenToWorld2D(cursor->screen_pos, *cam);
	
	// Skip camera controls and selection if a UI element is hovered
	if(cursor->flags & CURSOR_ON_UI) return;

	// Update camera
	CursorCameraControls(cursor, cam, dt);

	// Update selection box
	if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
		SelectionBox(cursor);

	if(IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
		cursor->selection_rec_final = cursor->selection_rec_edit;

		cursor->box_origin = (Vector2){0};
		cursor->selection_rec_edit = (Rectangle){0};

		// Enforce minimum area
		if(cursor->selection_rec_final.width * cursor->selection_rec_final.height <= (4096 * 2)) {
			cursor->selection_rec_final = (Rectangle){0};
			cursor->flags &= ~CURSOR_BOX_OPEN;
		} else 
			cursor->flags |= CURSOR_BOX_OPEN;

		cursor->flags &= ~CURSOR_SELECTION;
	}
}

// Update camera values with mouse
void CursorCameraControls(Cursor *cursor, Camera2D *cam, float dt) {
	// Track previous cursor position
	Vector2 prev = cursor->world_pos;

	// Zoom in and out with mouse wheel
	float scroll = GetMouseWheelMove();
	if(cursor->flags & CURSOR_LOCK_ZOOM) scroll = 0;
	if(fabs(scroll) > 0) {
		cam->zoom += scroll * 0.25f;
		cam->zoom = Clamp(cam->zoom, 0.4f, 2.0f);

		Vector2 next = GetScreenToWorld2D(cursor->screen_pos, *cam);

		Vector2 diff = Vector2Subtract(prev, next);
		cam->target = Vector2Add(cam->target, diff);
	}

	// Pan camera on middle mouse button press 
	if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
		if(!(cursor->flags & CURSOR_PAN)) cursor->pan_pos = cursor->world_pos;
		cursor->flags |= CURSOR_PAN;

		Vector2 diff = Vector2Subtract(cursor->pan_pos, cursor->world_pos);
		cam->target = Vector2Add(cam->target, diff);
	} else cursor->flags &= ~CURSOR_PAN;
}

// Draw cursor every frame
void CursorDraw(Cursor *cursor) {
	// Draw selection box
	if(cursor->flags & CURSOR_SELECTION) {
		DrawRectangleRec(cursor->selection_rec_edit, ColorAlpha(SKYBLUE, 0.75f));
		DrawRectangleLinesEx(cursor->selection_rec_edit, 2, BLUE);
	} else {
		DrawRectangleRec(cursor->selection_rec_final, ColorAlpha(SKYBLUE, 0.25f));
		DrawRectangleLinesEx(cursor->selection_rec_final, 2, GRAY);
	}
}

// Draw mouse pointer icon
void CursorDrawPointer(Cursor *cursor) {
	Color color = RAYWHITE;

	int icon = ICON_CURSOR_CLASSIC;
	if(cursor->flags & CURSOR_PAN) icon = ICON_CURSOR_HAND;

	GuiDrawIcon(icon, cursor->screen_pos.x, cursor->screen_pos.y, 2, color);
}

// Update cursor selection rectangle
void SelectionBox(Cursor *cursor) {
	// Check and update cursor flags, set box origin 
	if(!(cursor->flags & CURSOR_SELECTION)) {
		cursor->box_origin = cursor->world_pos;
		cursor->flags |= CURSOR_SELECTION;
		cursor->flags &= ~CURSOR_BOX_OPEN;
		return;
	}

	// Calculate upper left corner of box
	Vector2 start = (Vector2) {
		.x = fminf(cursor->box_origin.x, cursor->world_pos.x),
		.y = fminf(cursor->box_origin.y, cursor->world_pos.y)
	};

	// Calculate lower right corner of box
	Vector2 end = (Vector2) {
		.x = fmaxf(cursor->box_origin.x, cursor->world_pos.x),
		.y = fmaxf(cursor->box_origin.y, cursor->world_pos.y)
	};

	// Set selection rectangle values
	cursor->selection_rec_edit = (Rectangle) {
		.x = start.x,
		.y = start.y,
		.width = end.x - start.x,
		.height = end.y - start.y
	};
}

