/* vpwspal.h - VPW Studio palette helpers */
#ifndef __VPWSPAL_H__
#define __VPWSPAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* .vpwspal palette entry */
typedef struct {
	int red;
	int green;
	int blue;
	int alpha;
} PalColor;

void MakePalColor(char *in, PalColor *out){
	char *stringVals = strtok(in, " ");
	if(stringVals != NULL){
		out->red = atoi(stringVals);
	}

	stringVals = strtok(NULL, " ");
	if(stringVals != NULL){
		out->green = atoi(stringVals);
	}

	stringVals = strtok(NULL, " ");
	if(stringVals != NULL){
		out->blue = atoi(stringVals);
	}

	out->alpha = 255; /* force alpha; not ideal, but no real solution */
}

char *GetPalColorString(PalColor *in){
	int outVal = 0;
	char *palOut = (char *)calloc(16, sizeof(char)); /* assumes four 3-digit values, with spaces and null terminator */

	if(palOut == NULL){
		return "GetPalColor error";
	}

	outVal = snprintf(palOut, 16, "%d %d %d %d", in->red, in->green, in->blue, in->alpha);

	return palOut;
}

#endif
