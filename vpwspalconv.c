/* vpwspalconv - VPW Studio palette converter
 * This program takes a single JASC Paint Shop Pro .pal file
 * and converts it to a VPW Studio .vpwspal file.
 */

#include <stdio.h>
#include <string.h>
#include "vpwspal.h"

#define VERSION_STRING "v1.1"
#define STRING_BUFFER_SIZE 256
#define MAX_PALETTE_ENTRIES 256

static void Usage(char* execName){
	printf("%s - VPW Studio palette converter %s\n", execName, VERSION_STRING);
	printf("Usage: %s JASC_PAL_FILE\n", execName);
}

int main(int argc, char *argv[]){
	FILE *inFile;
	FILE *outFile;
	char *stringBuf;
	char *outFileName;
	int stringCompare = 0;
	int numPalColors = 0;
	PalColor *colors;
	char *temp;

	if(argc <= 1){
		Usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("%s - VPW Studio palette converter %s\n", argv[0], VERSION_STRING);

	if(argc > 2){
		printf("This program does not currently support processing multiple files at one time.\n");
		exit(EXIT_FAILURE);
	}

	inFile = fopen(argv[1], "r");
	if(inFile == NULL){
		printf("Error opening file '%s'\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* read through file and deal with it */
	stringBuf = (char *)calloc(STRING_BUFFER_SIZE, sizeof(char));
	if(stringBuf == NULL){
		printf("Unable to allocate %d bytes for stringBuf.\n",STRING_BUFFER_SIZE);
		exit(EXIT_FAILURE);
	}

	/* look for "JASC-PAL" on first line */
	fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
	stringCompare = strncmp("JASC-PAL", stringBuf, 8);
	if(stringCompare != 0){
		printf("File '%s' does not appear to be a JASC Paint Shop Pro palette file.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* look for "0100" on second line */
	fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
	stringCompare = strncmp("0100", stringBuf, 4);
	if(stringCompare != 0){
		printf("Unexpected version value '%s' (expected '0100') in file '%s'.\n", stringBuf, argv[1]);
		exit(EXIT_FAILURE);
	}

	/* third line has number of colors in palette */
	fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
	numPalColors = atoi(stringBuf);

	if(numPalColors > MAX_PALETTE_ENTRIES){
		printf("Palette '%s' has more than 256 colors, which is unsupported by this program.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	colors = (PalColor*)calloc(numPalColors, sizeof(PalColor));
	if(colors == NULL){
		printf("Unable to allocate space for colors.\n");
		exit(EXIT_FAILURE);
	}

	/* remaining lines are palette entries */
	for(int i = 0; i < numPalColors; i++){
		fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
		MakePalColor(stringBuf, &colors[i]);
	}
	fclose(inFile);

	/* handle output filename */
	temp = strrchr(argv[1], '.');
	if(temp == NULL){
		printf("File '%s' does not have an extension, and I'm a lazy twat who doesn't want to handle that. -f\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	int dotDiff = temp-argv[1];
	outFileName = calloc(dotDiff+9, sizeof(char));
	outFileName = strncpy(outFileName, argv[1], dotDiff);
	strncat(outFileName, ".vpwspal", 9);

	outFile = fopen(outFileName,"w");
	if(outFile == NULL){
		printf("Error attempting to create output file '%s'\n", outFileName);
		exit(EXIT_FAILURE);
	}

	fputs("VPWStudio-PAL\n", outFile);
	/* since this doesn't convert subpalettes, just re-write "0100" */
	fputs("0100\n", outFile);
	fprintf(outFile, "%d\n", numPalColors);

	/* write vpwspal format palette entries */
	for(int i = 0; i < numPalColors; i++){
		fprintf(outFile, "%s\n", GetPalColorString(&colors[i]));
	}

	fclose(outFile);
	printf("Wrote converted palette file to '%s'.\n", outFileName);

	exit(EXIT_SUCCESS);
}
