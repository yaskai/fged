#include <stdint.h>
#include "raylib.h"
#include "ui.h"

#ifndef APP_H_
#define APP_H_

typedef struct {
	uint8_t flags;

	float delta_time;
		
	Camera2D cam;

	Ui ui;

	Config *conf;
} App;

App AppInit(Config *conf);
void AppUpdate(App *app);
void AppDraw(App *app);
void AppClose(App *app);

#endif
