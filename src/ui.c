#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "ui.h"
#include "config.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#include "include/styles/dark.h"

void UiInit(Ui *ui, Config *conf) {
	GuiLoadStyleDark();

	ui->ww = conf->window_width;
	ui->wh = conf->window_height;
	
	PanelsInit(ui);
	DropdownsInit(ui);
}

void UiUpdate(Ui *ui, float dt) {
	// Draw panels
	for(uint8_t i = 0; i < 4; i++) GuiPanel(ui->panel_recs[i], ui->panel_text[i]);

	// Quit prompt
	if(IsKeyPressed(KEY_ESCAPE)) ui->flags ^= UI_QUIT_PROMPT;
	if(ui->flags & UI_QUIT_PROMPT) QuitPrompt(ui);
}

void QuitPrompt(Ui *ui) {
	Rectangle rec = (Rectangle){ ui->ww * 0.5f - 250, ui->wh * 0.5f - 150, 500, 300 };
	int i = GuiMessageBox(rec, "QUIT?", "All unsaved progress will be lost", "no;yes");

	switch(i) {
		case 1: ui->flags ^= UI_QUIT_PROMPT; break;
		case 2: ui->flags |= UI_QUIT_REQ;	 break;
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

void DropdownsInit(Ui *ui) {
	Rectangle recs[3] = {
		{0, 0, 100,   24},
		{100, 0, 100, 24},
		{200, 0, 100, 24},
	};

	char *dd_titles[DROP_COLS][DROP_ROWS] = {
		{ "file", "open", "save", "quit",   "", "", "", "" },
		{ "edit", "undo", "redo", "resize", "", "", "", "" },
		{ "help", "", "", "", "", "", "", ""			   },
	};
}

