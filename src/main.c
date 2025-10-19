#include "raylib.h"
#include "config.h"
#include "app.h"

int main() {
	// Main loop runs while false
	bool exit_window = false;

	// Initialize config struct with some default values
	Config conf = (Config) {
		.refresh_rate = 60,
		.window_width = 1920,
		.window_height = 1080,
		.window_flags = 0,
		.gui_style = 0
	};

	// Read configuration file
	// if not found, defaults are used
	ConfRead(&conf, "options.conf");

	// Set window flags
	SetConfigFlags(conf.window_flags | FLAG_WINDOW_HIGHDPI);

	// Disable Raylib log messages
	SetTraceLogLevel(LOG_NONE);

	// Open window, set refresh rate
	InitWindow(conf.window_width, conf.window_height, "Fishgame Editor");
	SetTargetFPS(conf.refresh_rate);

	// Set exit key to f4 
	// default is escape key which is confusing in GUI context
	SetExitKey(KEY_F4);

	DisableCursor();

	// Initialize app struct
	App app = (App){0};
	AppInit(&app, &conf);

	// Main loop:
	// update and render everything
	// interrupted by OS or on quit request
	while(!exit_window) {
		exit_window = (app.ui.flags & UI_QUIT_REQ || WindowShouldClose());

		AppUpdate(&app);
		AppDraw(&app);
	}

	// Exit:
	// close window context, unload data
	CloseWindow();
	AppClose(&app);

	return 0;
}

