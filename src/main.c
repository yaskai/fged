#include "raylib.h"
#include "include/resource_dir.h"
#include "config.h"
#include "app.h"

int main() {
	bool exit_window = false;

	Config conf = (Config) {
		.refresh_rate = 60,
		.window_width = 1920,
		.window_height = 1080,
		.window_flags = 0
	};

	SetConfigFlags(0);
	SetTraceLogLevel(LOG_ERROR);

	ConfRead(&conf, "options.conf");
	InitWindow(conf.window_width, conf.window_height, "Fishgame Editor");
	SetTargetFPS(conf.refresh_rate);
	SetExitKey(KEY_F4);

	SearchAndSetResourceDir("resources");

	App app = AppInit(&conf);

	while(!exit_window) {
		exit_window = (app.ui.flags & UI_QUIT_REQ || WindowShouldClose());

		AppUpdate(&app);
		AppDraw(&app);
	}

	CloseWindow();
	AppClose(&app);

	return 0;
}

