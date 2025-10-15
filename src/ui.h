#include <stdint.h>
#include "raylib.h"
#include "config.h"

#ifndef UI_H_
#define UI_H_

#define UI_QUIT_REQ		0x01

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
} Ui;

void UiInit(Ui *ui, Config *conf);
void UiUpdate(Ui *ui, float dt);

void PanelsInit(Ui *ui);

#endif
