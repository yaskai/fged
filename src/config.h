#include <stdint.h>

#ifndef CONFIG_H
#define CONFIG_H 

#define COMMENT_MARKER  '#'
#define FLAG_MARKER 	'_'

typedef struct {
	unsigned int refresh_rate;
	unsigned int window_width, window_height;
	unsigned int window_flags;
} Config;

void ConfRead(Config *conf, char *file_path);
void ConfWrite(Config *conf);
void ConfParseLine(Config *conf, char *line);

#endif
