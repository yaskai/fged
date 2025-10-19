#include <stdio.h>
#include <string.h>
#include "config.h"

// Open configuration file for parsing 
void ConfRead(Config *conf, char *file_path) {
	// Open file
	FILE *pf = fopen(file_path, "r"); 

	// Return and log error if file not found
	if(!pf) {
		puts("\e[0;31mERROR: COULD NOT LOAD CONFIGURATION FILE");
		return;
	}

	puts("\e[0;34m");
	puts("READING CONFIGURATION FILE...");

	// Parse line by line 
	char line[128];
	while(fgets(line, sizeof(line), pf)) {
		// Skip commented lines
		if(line[0] == COMMENT_MARKER) continue;
		
		// TODO: parse window flags
		if(line[0] == FLAG_MARKER) {
			continue;
		}
		
		ConfParseLine(conf, line);
	}

	// Display colorful config info 
	printf("refresh_rate=%d\n", conf->refresh_rate);
	printf("window_width=%d\n", conf->window_width);
	printf("window_height=%d\n", conf->window_height);
	printf("gui_style=%d\n", conf->gui_style);
	puts("\e[0;37m");
}

// Parse a line from configuration file
void ConfParseLine(Config *conf, char *line) {
	// Find index of seperator character, return if not found
	char *eq = strchr(line, '=');
	if(!eq) return;

	// Split line into key and value strings
	*eq = '\0';
	char *key = line;
	char *val = eq + 1;

	// Set appropriate values
	if(strcmp(key, "refresh_rate") == 0) 
		sscanf(val, "%d", &conf->refresh_rate);
	else if(strcmp(key, "window_width") == 0) 
		sscanf(val, "%d", &conf->window_width);
	else if(strcmp(key, "window_height") == 0) 
		sscanf(val, "%d", &conf->window_height);
	else if(strcmp(key, "gui_style") == 0)
		sscanf(val, "%d", &conf->gui_style);
}

