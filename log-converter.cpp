// log-converter.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "log_enum.h"
#include "log_strings.h"

// Things to help with parsing filenames.
#define DIR_SEPARATORS "\\/"
#define EXT_SEPARATOR "."

// Determine endianness of this machine.
static const uint32_t endianness = 0xdeadbeef; 
#define IS_LITTLE_ENDIAN (*(const char *) &endianness == 0xef)

// This will have been defined in log_strings.h
extern const char *gLogStrings[];

// Print the usage text
static void printUsage(char *pExeName) {
    printf("\n%s: take a binary file of logging data and convert it to human-readable form on stdout.\n", pExeName);
    printf("The binary format is defined here: https://github.com/u-blox/log-client, basically three\n");
    printf("32-bit words, the first being the microsecond timestamp, the second the log event and the\n");
    printf("third a parameter for that event.  In order to convert the log event to a string this binary\n");
    printf("must have been compiled with the right header file, see https://github.com/u-blox/log-converter\n");
    printf("for more information.\n\nUsage:\n");
    printf("  %s file endianness\n\n", pExeName);
    printf("where:\n");
    printf("  file is the name of the binary format logging input file,\n");
    printf("  endianness is the endianness of the data in the binary file, b for big, l for little,\n");
    printf("endianness may be omitted, in which case the endianness of the binary data is assumed to\n");
    printf("be that of this machine (i.e. no conversion is required).\n\n");
    printf("For example:\n");
    printf("  %s binary_data.log l\n\n", pExeName);
}

// Convert endianness of array
static void reverseEndian(unsigned char *pArray, int width)
{
    char c;

    for (int x = 0; x < width; x++ ) {
        c = *(pArray + x);
        *(pArray + width - x - 1) = c;
    }
}

// Parse the input file and write to stdout
static int parse(FILE *pInputFile, bool endianConvert)
{
    int logEntries = 0;
    unsigned int entry[3];
    const char *pEventString;

    // Read an entry at a time from the input file
    while (fread(entry, sizeof(entry[0]), sizeof(entry)/ sizeof(entry[0]), pInputFile) == sizeof(entry)/ sizeof(entry[0])) {
        logEntries++;
        // Convert endianness if required
        if (endianConvert) {
            for (unsigned int x = 0; x < sizeof(entry)/ sizeof(entry[0]); x++) {
                reverseEndian((unsigned char *) entry[x], sizeof (entry[x]));
            }
        }
        // Create an event string
        pEventString = "UNKNOWN_EVENT";
        if (entry[1] < sizeof (gLogStrings)/sizeof(gLogStrings[0])) {
            pEventString = gLogStrings[entry[1]];
        }

        // Print to stdout
        fprintf(stdout, "%10u, %10d, \"%s\", %10d, 0x%08x\n", entry[0], entry[1], pEventString, entry[2], entry[2]);
    }

    return logEntries;
}

// Entry point
int main(int argc, char* argv[])
{
    int retValue = -1;
    bool success = false;
    int x = 0;
    char *pExeName = NULL;
	char *pInputFileName = NULL;
    FILE *pInputFile = NULL;
    char *pEndianness = NULL;
    bool endianConvert = false;
    char *pChar;

    // Find the exe name in the first argument
    pChar = strtok(argv[x], DIR_SEPARATORS);
    while (pChar != NULL) {
        pExeName = pChar;
        pChar = strtok(NULL, DIR_SEPARATORS);
    }
    if (pExeName != NULL) {
        // Remove the extension
        pChar = strtok(pExeName, EXT_SEPARATOR);
        if (pChar != NULL) {
            pExeName = pChar;
        }
    }
    x++;

    // Look for all the command line parameters
    while (x < argc) {
        switch (x) {
            case 1:
                pInputFileName = argv[x];
            break;
            case 2:
                pEndianness = argv[x];
            break;
        }
        x++;
    }

    // Must have the command-line parameter
    if (pInputFileName != NULL) {
        success = true;
        // Check that endianness makes sense
        if ((pEndianness != NULL) &&
            ((strlen (pEndianness) != 1) ||
             ((*pEndianness != 'l') &&
              (*pEndianness != 'b')))) {
            success = false;
            printf("Endianness must be l for little or b for big.\n");
        }
        // Open the left input file
        pInputFile = fopen (pInputFileName, "rb");
        if (pInputFile == NULL) {
            success = false;
            printf("Cannot open input file %s (%s).\n", pInputFileName, strerror(errno));
        }
        if (success) {
            // Determine if endian conversion is required
            if (pEndianness != NULL) {
                endianConvert = ((*pEndianness == 'l') && !IS_LITTLE_ENDIAN) ||
                                ((*pEndianness == 'b') && IS_LITTLE_ENDIAN);
            }
            x = parse(pInputFile, endianConvert);
        } else {
            printUsage(pExeName);
        }
    } else {
        printUsage(pExeName);
    }

    if (success) {
        retValue = 0;
    }

    // Clean up
    if (pInputFile != NULL) {
        fclose(pInputFile);
    }

    return retValue;
}