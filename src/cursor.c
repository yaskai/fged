#include "raylib.h"
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

	if(IsKeyDown(KEY_A)) cam->target.x--;
	if(IsKeyDown(KEY_D)) cam->target.x++;
	if(IsKeyDown(KEY_W)) cam->target.y--;
	if(IsKeyDown(KEY_S)) cam->target.y++;
}

void CursorDraw(Cursor *cursor) {
}

void CursorDrawPointer(Cursor *cursor) {
	Color color = RAYWHITE;
	GuiDrawIcon(ICON_CURSOR_CLASSIC, cursor->screen_pos.x, cursor->screen_pos.y, 2, color);
}

