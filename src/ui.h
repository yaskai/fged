#include <stdint.h>
#include "raylib.h"
#include "config.h"

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

	unsigned int ww, wh;

	Rectangle panel_recs[4];
	char *panel_text[4];
	
	Rectangle dropwdown_recs[DROP_COLS];
	char *dropdown_text[DROP_COLS][DROP_ROWS];
} Ui;

void UiInit(Ui *ui, Config *conf);
void UiUpdate(Ui *ui, float dt);

void QuitPrompt(Ui *ui);

void PanelsInit(Ui *ui);
void DropdownsInit(Ui *ui);

#endif
