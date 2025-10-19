#include "raylib.h"
#include "app.h"

// Initialize app struct:
// set values, load data, etc. 
void AppInit(App *app, Config *conf) {
	// Set configuration values 
	app->conf = conf;

	// Initialize camera
	app->cam = (Camera2D) {
		.target = {100, 100},
		.offset = {0, 0},
		.rotation = 0,
		.zoom = 1
	};

	// Initialize cursor
	app->cursor = (Cursor){0};
	CursorInit(&app->cursor);

	// Initialize map
	app->map = (Map){0};
	MapInit(&app->map, &app->cam, &app->cursor);

	// Initialize UI
	app->ui = (Ui){0};
	UiInit(&app->ui, app->conf, &app->cam);
}

// Update
void AppUpdate(App *app) {
	app->delta_time = GetFrameTime();
	
	CursorUpdate(&app->cursor, &app->cam, app->delta_time);
}

// Draw 
void AppDraw(App *app) {
	BeginDrawing();
	ClearBackground(BLACK);

	// Draw with camera tranformations
	BeginMode2D(app->cam);
	MapDrawGrid(&app->map);
	CursorDraw(&app->cursor);
	EndMode2D();
	
	// UI update and draw are done simultaneously (Immediate mode GUI) 
	UiUpdate(&app->ui, &app->cursor, app->delta_time);

	// Draw mouse pointer over everything else
	CursorDrawPointer(&app->cursor);

	EndDrawing();
}

// Unload data
void AppClose(App *app) {
}

