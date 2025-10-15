#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "ui.h"
#include "config.h"

#define RAYGUI_IMPLEMENTATION
#include "./include/raygui.h"
#undef RAYGUI_IMPLEMENTATION

void UiInit(Ui *ui, Config *conf) {
	ui->ww = conf->window_width;
	ui->wh = conf->window_height;
	
	PanelsInit(ui);
}

void UiUpdate(Ui *ui, float dt) {
	for(uint8_t i = 0; i < 4; i++) {
	}
}

void PanelsInit(Ui *ui) {
	Rectangle recs[4] = {
		{0, 0, ui->ww, 100}, 
		{0, ui->wh - 100, ui->ww, 100}, 
		{0, 100, 100, ui->wh - 200}, 
		{ui->ww - 100, 100, 100, ui->wh - 200}, 
	};

	char *text[4] = {
		"top",
		"bottom",
		"left",
		"right"
	};

	memcpy(ui->panel_recs, recs, sizeof(recs));
	memcpy(ui->panel_text, text, sizeof(text));
}

