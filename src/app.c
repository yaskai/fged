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

	// Initialize sprite system
	app->sprite_loader = (SpriteLoader){0};
	SpriteLoaderInit(&app->sprite_loader);

	// Initialize map
	app->map = (Map){0};
	MapInit(&app->map, &app->cam, &app->cursor, &app->sprite_loader);

	// Initialize UI
	app->ui = (Ui){0};
	UiInit(&app->ui, app->conf, &app->cam, &app->sprite_loader, &app->map);
}

// Update
void AppUpdate(App *app) {
	app->delta_time = GetFrameTime();
	
	CursorUpdate(&app->cursor, &app->cam, app->delta_time);
	if(app->map.active_buffer > -1) MapUpdate(&app->map);
}

// Draw 
void AppDraw(App *app) {
	BeginDrawing();
	ClearBackground(BLACK);

	// Draw with camera tranformations
	BeginMode2D(app->cam);

	if(app->map.active_buffer > -1) 
		MapDraw(&app->map);

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
	SpriteLoaderClose(&app->sprite_loader);
}

