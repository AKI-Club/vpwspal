/* vpwspalmerge - VPW Studio palette merge tool
 * This program takes in multiple JASC Paint Shop Pro .pal files and
 * converts them to a single VPW Studio .vpwspal file with sub-palettes.
 *
 * WARNING: Only meant to be used with 4bpp palettes (16 colors per palette).
 * Note: The maximum number of palette files this program can deal with is 16.
 *
 * The merged .vpwspal will be named after the first file passed in to the program.
 *
 * Number of sub-palettes is (number of files)-1.
 */

#include <stdio.h>
#include <string.h>
#include "vpwspal.h"

#define STRING_BUFFER_SIZE 256
#define MAX_PALETTE_FILES 16

int AppendPalette(char *palFileName, FILE *outFile){
	FILE *inFile;
	char *stringBuf;
	int stringCompare = 0;
	int numPalColors;
	PalColor *colors;
	char *temp;

	inFile = fopen(palFileName, "r");
	if(inFile == NULL){
		printf("Unable to open palette file '%s' for reading.\n", palFileName);
		return -1;
	}

	stringBuf = (char *)calloc(STRING_BUFFER_SIZE, sizeof(char));
	if(stringBuf == NULL){
		printf("Unable to allocate %d bytes for stringBuf.\n",STRING_BUFFER_SIZE);
		return -1;
	}

	fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
	stringCompare = strncmp("JASC-PAL", stringBuf, 8);
	if(stringCompare != 0){
		printf("File '%s' does not appear to be a JASC Paint Shop Pro palette file.\n", palFileName);
		return -1;
	}

	fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
	stringCompare = strncmp("0100", stringBuf, 4);
	if(stringCompare != 0){
		printf("Unexpected version value '%s' (expected '0100') in file '%s'.\n", stringBuf, palFileName);
		return -1;
	}

	/* third line has number of colors in palette */
	fgets(stringBuf, STRING_BUFFER_SIZE, inFile);
	numPalColors = atoi(stringBuf);
	if(numPalColors != 16){
		printf("This program only supports 4bpp/16 entries; found %d entries in files '%s'.\n", numPalColors, palFileName);
		return -1;
	}

	colors = (PalColor*)calloc(numPalColors, sizeof(PalColor));
	if(colors == NULL){
		printf("Unable to allocate space for colors.\n");
		return -1;
	}

	for(int i = 0; i < numPalColors; i++){
		fgets(stringBuf, STRING_BUFFER_SIZE, inFile);

		temp = strchr(stringBuf, '\r');
		if(temp != NULL){
			stringBuf[temp-stringBuf] = '\0';
		}

		temp = strchr(stringBuf, '\n');
		if(temp != NULL){
			stringBuf[temp-stringBuf] = '\0';
		}

		stringBuf = strncat(stringBuf, " 255", 6);
		fprintf(outFile, "%s\n", stringBuf);
	}
	fclose(inFile);

	free(colors);
	free(stringBuf);
	return 0;
}

static void Usage(char* execName){
	printf("%s - VPW Studio palette merge tool\n", execName);
	printf("Usage: %s JASC_PAL_FILES\n", execName);
	printf("The output filename will be based on the first file passed in.\n");
}

int main(int argc, char *argv[]){
	FILE *outFile;
	char *outFileName;
	char *temp;
	int numFiles;

	if(argc <= 1){
		Usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("%s - VPW Studio palette merge tool\n", argv[0]);

	if(argc < 3){
		printf("%s requires more than one palette file to be passed in.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* calculate output filename based on first file passed in */
	temp = strrchr(argv[1], '.');
	if(temp == NULL){
		printf("File '%s' does not have an extension, and I'm a lazy twat who doesn't want to handle that. -f\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	int dotDiff = temp-argv[1];
	outFileName = calloc(dotDiff+9, sizeof(char));
	outFileName = strncpy(outFileName, argv[1], dotDiff);
	strncat(outFileName, ".vpwspal", 9);

	numFiles = argc-1;
	if(numFiles > MAX_PALETTE_FILES){
		printf("This program only supports merging up to 16 palettes at once.\n");
		exit(EXIT_FAILURE);
	}

	outFile = fopen(outFileName,"w");
	if(outFile == NULL){
		printf("Error attempting to create output file '%s'\n", outFileName);
		exit(EXIT_FAILURE);
	}

	/* write the junk */
	fputs("VPWStudio-PAL\n", outFile);
	fprintf(outFile, "01%02d\n", numFiles-1);
	/* hardcoded bullshit because this assumes 4bpp palettes */
	fputs("16\n", outFile);

	/* handle each file */
	for(int i = 0; i < numFiles; i++){
		AppendPalette(argv[1+i], outFile);
	}

	fclose(outFile);
	return 0;
}
