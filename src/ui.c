#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "ui.h"
#include "config.h"
#include "cursor.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#undef RAYGUI_IMPLEMENTATION

// * 
// Dropdown button events: 
// to be stored in the "dropdown_fn" array
// functions are called on button press respective to index
void fn_new(Ui *ui)  {}
void fn_save(Ui *ui) {}
void fn_open(Ui *ui) {}
void fn_quit(Ui *ui) { ui->flags ^= UI_QUIT_PROMPT; }

void fn_undo(Ui *ui) {}
void fn_redo(Ui *ui) {}

void fn_help(Ui *ui) {}

// Dropdown event type definition
typedef void(*DropdownEventFn)(Ui *ui);

// Dropdown event function array
// depth reduced by one as buttons for "file", "edit" and "help" don't need functions
DropdownEventFn dropdown_fn[DROP_COLS][DROP_ROWS-1];

void UiInit(Ui *ui, Config *conf, Camera2D *cam) {
	// Load and initialize style
	StyleInit(ui, conf);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

	Font font = LoadFontEx("resources/blex.ttf", 32, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
	GuiSetFont(font);

	// Set window dimensions
	ui->ww = conf->window_width;
	ui->wh = conf->window_height;
	
	// Set pointers
	ui->cam = cam;
	
	// Initialize elements
	PanelsInit(ui);
	DropdownsInit(ui);
	CamSlidersInit(ui);
	//ScrollPanelInit(ui);
}

void UiUpdate(Ui *ui, Cursor *cursor, float dt) {
	cursor->flags &= ~CURSOR_ON_UI;

	// Draw panels
	for(uint8_t i = 0; i < 4; i++) {
		GuiPanel(ui->panel_recs[i], ui->panel_text[i]);
		
		// Update cursor on ui flag
		if(CheckCollisionPointRec(cursor->screen_pos, ui->panel_recs[i]))
	 		cursor->flags |= CURSOR_ON_UI;
	}

	// Quit prompt
	if(IsKeyPressed(KEY_ESCAPE)) ui->flags ^= UI_QUIT_PROMPT;
	if(ui->flags & UI_QUIT_PROMPT) QuitPrompt(ui);

	// Dropdown menus
	// On pressing a dropdown menu of depth 0 ("file", "edit", "help"),
	// set active dropdown to column of pressed dropdown button
	// or none if active is already set to this column.
	// Creates a toggle on/off behaviour
	for(uint8_t i = 0; i < DROP_COLS; i++) {
		// Set icon
		char *icon_str = (ui->active_dropdown == i) ? "#116#" : "#120#";

		// Draw button, check state 
		if(GuiButton(ui->dropwdown_recs[i], TextFormat("%s%s", icon_str, ui->dropdown_text[i][0])))  
			ui->active_dropdown = (ui->active_dropdown == i) ? -1 : i;

		// Update cursor on ui flag
		if(CheckCollisionPointRec(cursor->screen_pos, ui->dropwdown_recs[i]))
			cursor->flags |= CURSOR_ON_UI;
	}

	// Dropdown submenu if active ("active_dropdown" value is non-negative) 
	if(ui->active_dropdown > -1) {
		for(int8_t i = 1; i < DROP_ROWS; i++) {
			// Break on empty strings (char[0] == string terminator)
			// dropdown buttons should be contiguous and labelled,
			// format deemed improper if this is not the case!
			char *text = ui->dropdown_text[ui->active_dropdown][i];
			if(text[0] == '\0') break;

			// Define button bounding box 
			Rectangle rec = (Rectangle) {
				ui->dropwdown_recs[ui->active_dropdown].x,
				ui->dropwdown_recs[ui->active_dropdown].y + (ui->dropwdown_recs[0].height * i),
				ui->dropwdown_recs[ui->active_dropdown].width,
				ui->dropwdown_recs[ui->active_dropdown].height
			};

			// Render button, check for pressed state 
			if(GuiButton(rec, text)) {
				// On button press, function pointed to by index is called 
				if(dropdown_fn[ui->active_dropdown][i-1])
					dropdown_fn[ui->active_dropdown][i-1](ui);

				// Update cursor on ui flag
				if(CheckCollisionPointRec(cursor->screen_pos, rec))
					cursor->flags |= CURSOR_ON_UI;
			}
		}
	}

	// TODO:
	// scroll panel for camera movement
}

// Update and render quit prompt screen, user can confirm to exit app or cancel and return 
void QuitPrompt(Ui *ui) {
	// Set message box rect, should appear in the center of the screen
	Rectangle rec = (Rectangle){ ui->ww * 0.5f - 250, ui->wh * 0.5f - 150, 500, 300 };

	// Get input, draw UI element
	int messagebox_input = GuiMessageBox(rec, "#159#QUIT?", "All unsaved progress will be lost.", "[N]o;[Y]es");

	// Keyboard input overrides: [n] = no, [y] = yes
	if(IsKeyPressed(KEY_N)) messagebox_input = 1;
	if(IsKeyPressed(KEY_Y)) messagebox_input = 2;

	switch(messagebox_input) {
		// On no or 'x', toggle quit prompt flag
		case 0: 
		case 1: 
			ui->flags ^= UI_QUIT_PROMPT;
			break;

		// On yes, request window exit 
		case 2:
			ui->flags |= UI_QUIT_REQ;
			break;
	}
}

// Load style set from "options.conf", colorful messages sent to terminal displaying style info
void StyleInit(Ui *ui, Config *conf) {
	printf("\e[0;33mSetting GUI style...\e[0;37m\n");

	char *styles_dir = "resources/styles";
	FilePathList sub_dirs = LoadDirectoryFiles(styles_dir); 

	for(uint8_t i = 0; i < sub_dirs.count; i++) {
		char *slash = strrchr(sub_dirs.paths[i], '/');
		char *style_name = slash + 1; 

		char path[64];
		sprintf(path, "%s/style_%s.rgs", sub_dirs.paths[i], style_name);

		char marker = (conf->gui_style == i) ? '*' : ' ' ;

		if(conf->gui_style == i) {
			GuiLoadStyle(path);
			printf("\e[0;33m");
		}
		
		printf("%d: ", i);
		if(i < 10) printf(" ");
		printf("%s %c\n", style_name, marker);
		printf("\e[0;37m");
	}
}

// Load a ".rgs" file from order of appearance in styles directory 
void StyleLoadFromId(Ui *ui, uint8_t id) {
	char *styles_dir = "resources/styles";
	FilePathList sub_dirs = LoadDirectoryFiles(styles_dir); 

	char *slash = strrchr(sub_dirs.paths[id], '/');
	char *style_name = slash + 1;

	char path[64];
	sprintf(path, "%s/style_%s.rgs", sub_dirs.paths[id], style_name);
	GuiLoadStyle(path);
}

// Load a ".rgs" file from style name
void StyleLoadFromName(Ui *ui, char *name) {
	char *styles_dir = "resources/styles";
	FilePathList sub_dirs = LoadDirectoryFiles(styles_dir); 
	
	char path[64];
	sprintf(path, "resources/styles/%s/style_%s.rgs", name, name);
	GuiLoadStyle(path);
}

// Set panel data (rectangles and text)
void PanelsInit(Ui *ui) {
	// Set box values 
	Rectangle recs[] = {
		{0, 0, ui->ww, 100}, 
		{0, ui->wh - 100, ui->ww, 100}, 
		{0, 100, 100, ui->wh - 200}, 
		{ui->ww - 100, 100, 100, ui->wh - 200}
	};

	// Set text values
	char *text[] = {
		"Top",
		"Bottom",
		"Left",
		"Right"
	};

	// Copy data to ui struct
	memcpy(ui->panel_recs, recs, sizeof(recs));
	memcpy(ui->panel_text, text, sizeof(text));
}

// Set dropdown menu data (rectangles, text and function pointers)
void DropdownsInit(Ui *ui) {
	// Set box values 
	Rectangle recs[] = {
		{0, 0, 100,   24},
		{100, 0, 100, 24},
		{200, 0, 100, 24}
	};

	// Set text values
	char *text[DROP_COLS][DROP_ROWS] = {
		{ "File", "#008#New", "#001#Open", "#002#Save", "#159#Quit", "", "", "" },
		{ "Edit", "#056#Undo", "#057#Redo", "", "", "", "", "" 	   },
		{ "Help", "", "", "", "", "", "", ""			   }
	};

	// Set event function pointers
	// Depth zero dropdown buttons don't need functions 
	// row count reduced by one to account for them
	DropdownEventFn events[DROP_COLS][DROP_ROWS-1] = {
		// "file":
		{ fn_new, fn_open, fn_save, fn_quit },

		// "edit":
		{ fn_undo, fn_redo },

		// "help":
		{ }
	};

	// Copy data to ui struct
	memcpy(ui->dropwdown_recs, recs, sizeof(recs));
	memcpy(ui->dropdown_text, text, sizeof(text));

	// Copy pointers for events to dropdown functions array
	memcpy(dropdown_fn, events, sizeof(events));

	// Set active dropdown to none
	ui->active_dropdown = -1;
}

// Set dropdown menu data (rectangles, text)
void CamSlidersInit(Ui *ui) {
	Rectangle recs[] = {
		// Horizontal
		(Rectangle) {
			.x = ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
			.y = ui->panel_recs[PANEL_BOT].y, 
			.width = ui->panel_recs[PANEL_RGT].x - ui->panel_recs[PANEL_LFT].width,
			.height = 32
		},

		// Vertical
		(Rectangle) {
			.x = ui->panel_recs[PANEL_RGT].x - ui->panel_recs[PANEL_RGT].width,
			.y = ui->panel_recs[PANEL_TOP].y + 32,
			.width = 32,
			.height = ui->panel_recs[PANEL_BOT].y - ui->panel_recs[PANEL_TOP].height
		}
	};

	memcpy(ui->cam_slider_recs, recs, sizeof(recs));

	// TODO:
	// Set text
}

// Set dropdown menu data (rectangles)
void ScrollPanelInit(Ui *ui) {
	Rectangle view_rec = (Rectangle) {
		.x = ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
		.y = ui->panel_recs[PANEL_TOP].y + ui->panel_recs[PANEL_TOP].height,
		.width = ui->panel_recs[PANEL_RGT].x - ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
		.height = ui->panel_recs[PANEL_BOT].y - ui->panel_recs[PANEL_TOP].y + ui->panel_recs[PANEL_TOP].height
	};

	Rectangle content_rec = (Rectangle) {
		.x = ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
		.y = ui->panel_recs[PANEL_TOP].y + ui->panel_recs[PANEL_TOP].height,
		.width = ui->panel_recs[PANEL_RGT].x - ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
		.height = ui->panel_recs[PANEL_BOT].y - ui->panel_recs[PANEL_TOP].y + ui->panel_recs[PANEL_TOP].height
	};

	ui->cam_slider_recs[0] = view_rec;
	ui->cam_slider_recs[1] = content_rec;
}

