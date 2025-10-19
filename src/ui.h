#include <stdint.h>
#include "raylib.h"
#include "config.h"
#include "cursor.h"

#ifndef UI_H_
#define UI_H_

#define UI_QUIT_REQ		0x01
#define UI_QUIT_PROMPT	0x02

#define DROP_COLS	3
#define DROP_ROWS 	8

enum PANELS {
	PANEL_TOP,
	PANEL_BOT,
	PANEL_LFT,
	PANEL_RGT
};

typedef struct {
	uint8_t flags;
	int8_t active_dropdown;

	unsigned int ww, wh;

	Rectangle panel_recs[4];
	char *panel_text[4];
	
	Rectangle dropwdown_recs[DROP_COLS];
	char *dropdown_text[DROP_COLS][DROP_ROWS];

	Rectangle cam_slider_recs[2];
	char *cam_slider_text[4];

	Camera2D *cam;
} Ui;

void UiInit(Ui *ui, Config *conf, Camera2D *cam);
void UiUpdate(Ui *ui, Cursor *cursor, float dt);

void QuitPrompt(Ui *ui);

void StyleInit(Ui *ui, Config *conf);
void StyleLoadFromId(Ui *ui, uint8_t id);
void StyleLoadFromName(Ui *ui, char *name);

void PanelsInit(Ui *ui);
void DropdownsInit(Ui *ui);
void CamSlidersInit(Ui *ui);
void ScrollPanelInit(Ui *ui);

#endif
