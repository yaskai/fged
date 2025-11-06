#include <stdint.h>
#include "raylib.h"
#include "config.h"
#include "cursor.h"
#include "object.h"
#include "map.h"

#ifndef UI_H_
#define UI_H_

#define UI_QUIT_REQ		0x01
#define UI_QUIT_PROMPT	0x02
#define UI_FILE_DIAG	0x04
#define UI_BUFNAME_DIAG	0x08

#define DROP_COLS	3
#define DROP_ROWS 	8

enum PANELS {
	PANEL_TOP,	// 0
	PANEL_BOT,	// 1
	PANEL_LFT,	// 2
	PANEL_RGT 	// 3
};

#define SLIDER_ACTIVE		0x01
#define SLIDER_HOVERED		0x02
#define SLIDER_PRESSED 		0x04
#define SLIDER_BG_HOVERED	0x08

enum AXES {
	AXIS_HORIZONTAL,	// 0
	AXIS_VERTICAL		// 1
};

typedef struct {
	uint8_t flags;
	uint8_t axis;

	float percent;
	float val_min;
	float val_max;

	Rectangle base;
	Rectangle handle;

	float *control;
	float control_bounds[2];
} Slider;

void SliderUpdate(Slider *slider);

typedef struct {
	uint8_t flags;
	int8_t active_dropdown;

	unsigned int ww, wh;

	float object_list_scroll;

	Rectangle panel_recs[4];
	char *panel_text[4];
	
	Rectangle dropwdown_recs[DROP_COLS];
	char *dropdown_text[DROP_COLS][DROP_ROWS];

	Rectangle cam_slider_recs[2];
	char *cam_slider_text[4];

	Rectangle cam_slider_corner;

	Camera2D *cam;
	SpriteLoader *sl;
	Map *map;

	Rectangle tab_home_rec;
	Rectangle tab_new_rec;

	Slider sliders[2];
	ObjectEntry object_entries[4];

	Font font;

	Entity prototypes[4];

	char path_import[512];
	char path_export[512];
} Ui;

void UiInit(Ui *ui, Config *conf, Camera2D *cam, SpriteLoader *sl, Map *map);
void UiUpdate(Ui *ui, Cursor *cursor, float dt);

void UiQuitPrompt(Ui *ui);
void UiFileDiag(Ui *ui);
void UiObjectList(Ui *ui);
void UiTabs(Ui *ui);

void UiStyleInit(Ui *ui, Config *conf);
void UiStyleLoadFromId(Ui *ui, uint8_t id);
void UiStyleLoadFromName(Ui *ui, char *name);

void UiPanelsInit(Ui *ui);
void UiDropdownsInit(Ui *ui);
void UiFileDiagInit(Ui *ui);
void UiSlidersInit(Ui *ui);
void UiObjectListInit(Ui *ui);
void UiTabsInit(Ui *ui);

void EntEditProperties(Ui *ui);

#endif
