#include "raylib.h"
#include "app.h"

App AppInit(Config *conf) {
	App app = {0};

	app.conf = conf;

	app.cam = (Camera2D) {
		.target = {0, 0},
		.offset = {0, 0},
		.rotation = 0,
		.zoom = 0
	};

	app.ui = (Ui){0};
	UiInit(&app.ui, app.conf);

	return app;
}

void AppUpdate(App *app) {
	app->delta_time = GetFrameTime();
}

void AppDraw(App *app) {
	BeginDrawing();
	ClearBackground(BLACK);
	
	UiUpdate(&app->ui, app->delta_time);

	EndDrawing();
}

void AppClose(App *app) {
}

