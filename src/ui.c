#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "ui.h"
#include "config.h"
#include "cursor.h"
#include "map.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

GuiWindowFileDialogState file_diag_state;
Rectangle file_diag_rect;

// * 
// Dropdown button events: 
// to be stored in the "dropdown_fn" array
// functions are called on button press respective to index
void fn_new(Ui *ui)  { MapAddBuffer(ui->map); }
void fn_save(Ui *ui) { MapWriteBuffer(ui->map, "test.txt"); }
void fn_open(Ui *ui) { ui->flags |= UI_FILE_DIAG;   }
void fn_quit(Ui *ui) { ui->flags ^= UI_QUIT_PROMPT; }

void fn_undo(Ui *ui) { ActionUndo(&ui->map->buffers[ui->map->active_buffer]); }
void fn_redo(Ui *ui) { ActionRedo(&ui->map->buffers[ui->map->active_buffer]); }

void fn_help(Ui *ui) {}

// Dropdown event type definition
typedef void(*DropdownEventFn)(Ui *ui);

// Dropdown event function array
// depth reduced by one as buttons for "file", "edit" and "help" don't need functions
DropdownEventFn dropdown_fn[DROP_COLS][DROP_ROWS-1];

// Custom slider implementation
void SliderUpdate(Slider *slider) {
	// Clear state flags
	slider->flags = 0;
	
	// Track previous x value
	float x_prev = slider->percent;
	
	// Fetch color values from raygui implemention
	Color base_bg = GetColor(GuiGetStyle(SLIDER, BASE_COLOR_NORMAL)); 
	Color base_fg = GetColor(GuiGetStyle(SLIDER, BORDER_COLOR_NORMAL));

	Color handle_bg = GetColor(GuiGetStyle(SLIDER, BASE_COLOR_DISABLED));
	Color handle_fg = GetColor(GuiGetStyle(SLIDER, BORDER_COLOR_NORMAL));

	// Draw base and handle rectangles
	GuiDrawRectangle(slider->base, 2, base_fg, base_bg);

	// Check for hover/press
	if(CheckCollisionPointRec(GetMousePosition(), slider->base)) {
		slider->flags |= SLIDER_BG_HOVERED;

		Vector2 mouse_delta = GetMouseDelta();
		Rectangle handle_rec = slider->handle;
		
		handle_rec.x += mouse_delta.x * 0.5f;
		handle_rec.y += mouse_delta.y * 0.5f;

		handle_rec.width += mouse_delta.x;
		handle_rec.height += mouse_delta.y;
		
		if(CheckCollisionPointRec(GetMousePosition(), handle_rec)) {
			slider->flags |= SLIDER_HOVERED;

			handle_bg = GetColor(GuiGetStyle(SLIDER, BASE_COLOR_FOCUSED));
			handle_fg = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED));

			if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				slider->flags |= SLIDER_PRESSED;

				handle_bg = GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED));
				handle_fg = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED));
			}
		}	
	}

	// Update position on press
	if(slider->flags & SLIDER_PRESSED) {
		slider->handle.x = GetMouseX() - slider->handle.width * 0.5f;
	}

	// Clamp x to base in bounds
	slider->handle.x = 
		Clamp(slider->handle.x, slider->base.x, slider->base.x + slider->base.width - slider->handle.width);

	// Update control value if needed
	if(slider->handle.x != x_prev && 0 == 1) {
		float xrel = slider->handle.x - slider->base.x;
		float percent = (xrel / ((slider->base.width) - slider->handle.width) * 100);

		slider->percent = percent; 

		float val = ((slider->control_bounds[1] - slider->control_bounds[0]) * percent) / 100;
		val = Clamp(val, slider->control_bounds[0], slider->control_bounds[1]);
		*slider->control = val;
	}

	// Draw handle rectangle
	GuiDrawRectangle(slider->handle, 2, handle_fg, handle_bg);
}

// Initialize all elements, set style values, etc.
void UiInit(Ui *ui, Config *conf, Camera2D *cam, SpriteLoader *sl, Map *map) {
	// Load and initialize style StyleInit(ui, conf);
	UiStyleInit(ui, conf);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

	// Load and set font
	ui->font  = LoadFontEx("resources/fonts/source.ttf", 32, 0, 0);
	SetTextureFilter(ui->font.texture, TEXTURE_FILTER_TRILINEAR);
	GuiSetFont(ui->font);

	// Set window dimensions
	ui->ww = conf->window_width;
	ui->wh = conf->window_height;
	
	// Set pointers
	ui->cam = cam;
	ui->sl = sl;
	ui->map = map;
	
	// Initialize elements
	UiPanelsInit(ui);
	UiDropdownsInit(ui);
	UiFileDiagInit(ui);
	UiSlidersInit(ui);
	UiObjectListInit(ui);
}

// Update, manage, and render active elements
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
	if(IsKeyPressed(KEY_ESCAPE) && ui->flags == 0) 
		ui->flags ^= UI_QUIT_PROMPT;

	if(ui->flags & UI_QUIT_PROMPT) UiQuitPrompt(ui);

	// TODO:
	// File buffer tabs

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
	if(ui->active_dropdown > -1 && ui->flags == 0) {
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
	// Scroll sliders for camera movement
	SliderUpdate(&ui->sliders[0]);	
	SliderUpdate(&ui->sliders[1]);

	// Draw camera sliders corner rectangle
	GuiDrawRectangle(ui->cam_slider_corner, 0, BLACK, GRAY);

	// File dialogue (for importing and exporting maps)
	// lock cursor edit actions while dialogue is active
	file_diag_state.windowActive = (ui->flags & UI_FILE_DIAG);
	if(ui->flags & UI_FILE_DIAG) {
		UiFileDiag(ui);
		cursor->flags |= CURSOR_ON_UI;
	}

	// TODO:
	// Object list
	UiObjectList(ui);
}

// Update and render quit prompt screen, user can confirm to exit app or cancel and return 
void UiQuitPrompt(Ui *ui) {
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

// Dialogue window for importing and exporting files 
void UiFileDiag(Ui *ui) {
	// Clear dropdowns
	ui->active_dropdown = -1;

	// Adjust text size
	GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

	// Update state and render
	GuiWindowFileDialog(&file_diag_state);	

	// On file option press:
	// TODO:
	// integrate with file I/O
	if(file_diag_state.SelectFilePressed) {
	}

	if(IsKeyPressed(KEY_ESCAPE))
		file_diag_state.windowActive = 0;

	if(!file_diag_state.windowActive)
		ui->flags &= ~UI_FILE_DIAG;

	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
}

// Update and draw logic for selection from object list
void UiObjectList(Ui *ui) {
	uint16_t entry_count = (sizeof(ui->object_entries) / sizeof(ui->object_entries[0]));

	Color clr_background = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
	Color clr_default = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL));
	//Color clr_hovered = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_FOCUSED));
	//Color clr_pressed = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
	Color clr_hovered = GOLD;
	Color clr_pressed = SKYBLUE;
	
	for(uint16_t i = 0; i < entry_count; i++) {
		// Clear object list entry flags
		ui->object_entries[i].flags = 0;

		Color color = clr_default;

		float rec_pos = (i * (OBJ_ENTRY_H + 20)) + ui->panel_recs[PANEL_TOP].height + 34; 
		float text_pos = rec_pos + OBJ_ENTRY_H; 	

		Rectangle rec = (Rectangle){ ui->panel_recs[PANEL_LFT].x + 10, rec_pos, OBJ_ENTRY_W, OBJ_ENTRY_H }; 
		Rectangle text_rec = (Rectangle){ rec.x, text_pos, OBJ_ENTRY_W, 16 };
		
		// Check for hover and press
		Vector2 mouse_pos = GetMousePosition();
		if(CheckCollisionPointRec(mouse_pos, rec) || CheckCollisionPointRec(mouse_pos, text_rec)) {
			color = clr_hovered;
			ui->object_entries[i].flags |= OBJ_ENTRY_HOVERED;
				
			if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				color = clr_pressed;
				ui->object_entries[i].flags |= OBJ_ENTRY_PRESSED;
			}
		}
		
		// Draw frame
		DrawRectangleRec(rec, clr_background);
		DrawRectangleLinesEx(rec, 2, color);

		// Draw sprite preview frame
		if(ui->object_entries[i].spritesheet != NULL)
			DrawSpriteRec(ui->object_entries[i].spritesheet, 0, rec, 0);

		// Draw label text
		GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
		GuiDrawText(ui->object_entries[i].label, text_rec, TEXT_ALIGN_CENTER, color);
		GuiSetStyle(DEFAULT, TEXT_SIZE, 24); 
	}
}

// Load style set from "options.conf", colorful messages sent to terminal displaying style info
void UiStyleInit(Ui *ui, Config *conf) {
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
void UiStyleLoadFromId(Ui *ui, uint8_t id) {
	char *styles_dir = "resources/styles";
	FilePathList sub_dirs = LoadDirectoryFiles(styles_dir); 

	char *slash = strrchr(sub_dirs.paths[id], '/');
	char *style_name = slash + 1;

	char path[64];
	sprintf(path, "%s/style_%s.rgs", sub_dirs.paths[id], style_name);
	GuiLoadStyle(path);
}

// Load a ".rgs" file from style name
void UiStyleLoadFromName(Ui *ui, char *name) {
	char *styles_dir = "resources/styles";
	FilePathList sub_dirs = LoadDirectoryFiles(styles_dir); 
	
	char path[64];
	sprintf(path, "resources/styles/%s/style_%s.rgs", name, name);
	GuiLoadStyle(path);
}

// Set panel data (rectangles and text)
void UiPanelsInit(Ui *ui) {
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
		"Objects",
		"Right"
	};

	// Copy data to ui struct
	memcpy(ui->panel_recs, recs, sizeof(recs));
	memcpy(ui->panel_text, text, sizeof(text));
}

// Set dropdown menu data (rectangles, text and function pointers)
void UiDropdownsInit(Ui *ui) {
	// Set box values 
	Rectangle recs[] = {
		{0, 0, 100,   24},
		{100, 0, 100, 24},
		{200, 0, 100, 24}
	};

	// Set string values
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

void UiFileDiagInit(Ui *ui) {
	file_diag_state = InitGuiWindowFileDialog(GetWorkingDirectory());
	file_diag_rect = (Rectangle){ ui->ww * 0.5f - 600, ui->wh * 0.5f - 400, 1200, 800 };
	file_diag_state.windowBounds = file_diag_rect;
}

void UiSlidersInit(Ui *ui) {
	ui->sliders[0] = (Slider) {
		.flags = 0,
		.axis = AXIS_HORIZONTAL,

		.val_min = 0, .val_max = 100, .percent = 0,

		.base = (Rectangle) {
			.x = ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
			.y = ui->panel_recs[PANEL_BOT].y - 32,
			.width = ui->ww - (ui->panel_recs[PANEL_LFT].width + ui->panel_recs[PANEL_RGT].width) - 32,
			.height = 32
		},

		.handle = (Rectangle) {
			.x = ui->panel_recs[PANEL_LFT].x + ui->panel_recs[PANEL_LFT].width,
			.y = ui->panel_recs[PANEL_BOT].y - 32,
			.width = 100,
			.height = 32
		},
	};

	ui->sliders[1] = (Slider) {
		.flags = 0,
		.axis = AXIS_VERTICAL,

		.val_min = 0, .val_max = 100, .percent = 0,

		.base = (Rectangle) {
			.x = ui->panel_recs[PANEL_RGT].x - 32,
			.y = ui->panel_recs[PANEL_TOP].y + ui->panel_recs[PANEL_TOP].height,
			.width = 32,
			.height = ui->wh - (ui->panel_recs[PANEL_TOP].height + ui->panel_recs[PANEL_BOT].height) - 32
		},

		.handle = (Rectangle) {
			.x = ui->panel_recs[PANEL_RGT].x - 32,
			.y = ui->panel_recs[PANEL_TOP].y + ui->panel_recs[PANEL_TOP].height,
			.width = 32,
			.height = 100 
		},
	};

	ui->cam_slider_corner = (Rectangle) {
		.x = ui->panel_recs[PANEL_RGT].x - 32,
		.y = ui->panel_recs[PANEL_BOT].y - 32,	
		.width = 32,
		.height = 32
	};
}

// Initalize object list data 
// NOTE:
// Will probably be changed read from a file later
void UiObjectListInit(Ui *ui) {
	ObjectEntry entries[] = {
		(ObjectEntry) {
			.flags = 0,
			.type = ASTEROID,
			.frame_id = 0,
			.spritesheet = &ui->sl->spritesheets[0],
			.label = "asteroid"
		},

		(ObjectEntry) {
			.flags = 0,
			.type = PLAYER,
			.frame_id = 0,
			.spritesheet = &ui->sl->spritesheets[1],
			.label = "player"
		},

		(ObjectEntry) {
			.flags = 0,
			.type = SPAWNER_FISH,
			.frame_id = 0,
			.spritesheet = NULL,
			.label = "fish spawner"
		},

		(ObjectEntry) {
			.flags = 0,
			.type = SPAWNER_ITEM,
			.frame_id = 0,
			.spritesheet = NULL,
			.label = "item spawner"
		},
	};

	// Copy list data to ui struct	
	memcpy(ui->object_entries, entries, sizeof(entries));
	
	// Set list scroll value to top
	ui->object_list_scroll = 0;
}

