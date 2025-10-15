#include "raylib.h"
#include "include/resource_dir.h"
#include "config.h"
#include "ui.h"

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

	SearchAndSetResourceDir("resources");

	Ui ui = {0};
	UiInit(&ui, &conf);

	while(!exit_window) {
		if(ui.flags & UI_QUIT_REQ || WindowShouldClose()) exit_window = true;
		float delta_time = GetFrameTime();

		BeginDrawing();
		ClearBackground(BLACK);

		UiUpdate(&ui, delta_time);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

