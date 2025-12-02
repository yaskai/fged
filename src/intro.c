#include <stdio.h>
#include "intro.h"

void PrintIntro() { 
	printf("\e[0;34m"); 
	puts(""); \
	puts("------------------------------"); 
	puts("------------------------------"); 
	puts("     ______________________   "); 
	puts("    / ____/ ____/ ____/ __ \\ "); 
	puts("   / /_  / / __/ __/ / / / /  "); 
	puts("  / __/ / /_/ / /___/ /_/ /   "); 
	puts(" /_/    \\____/_____/_____/   "); 
	puts(""); 
	puts("------------------------------"); 
	puts("------------------------------"); 
	printf("\e[0;37m"); 
}

void PrintIntroImg(char *path) {
	FILE *pf = fopen(path, "r");	
	if(!pf) return;

	char line[128];

	while(fgets(line, sizeof(line), pf)) 
		printf("%s", line);

	fclose(pf);
}

