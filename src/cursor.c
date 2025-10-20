#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "include/raygui.h"
#include "cursor.h"

void CursorInit(Cursor *cursor) {
	cursor->flags = 0;
}

void CursorUpdate(Cursor *cursor, Camera2D *cam, float dt) {
	// Get cursor's world and screen positions
	cursor->screen_pos = GetMousePosition();
	cursor->world_pos = GetScreenToWorld2D(cursor->screen_pos, *cam);
	
	CursorCameraControls(cursor, cam, dt);
}

void CursorCameraControls(Cursor *cursor, Camera2D *cam, float dt) {
	// Skip camera controls if a UI element is hovered
	if(cursor->flags & CURSOR_ON_UI) return;


	// Track previous cursor position
	Vector2 prev = cursor->world_pos;

	// Zoom in and out with mouse wheel
	float scroll = GetMouseWheelMove();
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

void CursorDraw(Cursor *cursor) {
}

void CursorDrawPointer(Cursor *cursor) {
	Color color = RAYWHITE;
	GuiDrawIcon(ICON_CURSOR_CLASSIC, cursor->screen_pos.x, cursor->screen_pos.y, 2, color);
}

