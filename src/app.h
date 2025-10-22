#include <stdint.h>
#include "raylib.h"
#include "cursor.h"
#include "ui.h"
#include "map.h"
#include "sprites.h"

#ifndef APP_H_
#define APP_H_

typedef struct {
	uint8_t flags;

	float delta_time;
		
	Config *conf;

	Camera2D cam;
	Cursor cursor;
	SpriteLoader sprite_loader; 
	Map map;
	Ui ui;
} App;

void AppInit(App *app, Config *conf);
void AppUpdate(App *app);
void AppDraw(App *app);
void AppClose(App *app);

#endif
