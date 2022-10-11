#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


/*-----------COMMANDS CPU-----------*/

#define ADD  "add"					
#define DIV  "div"
#define DUMP "dump"					
#define HLT  "hlt"
#define IN   "in"
#define JMP  "jmp"
#define MUL  "mul"
#define OUT  "out"
#define PUSH "push"
#define SUB  "sub"

/*----------------------------------*/

#define MAXLENCOMMAND 20


#define CHECK_ERROR(condition, message_error, error_code) \
            do {                                          \
               if (condition) {                           \
                   printf ("%s", message_error);          \
                   return error_code;                     \
               }                                          \
            } while(false)

#define MAIN_DET(detector) if (detector) {                \
                            exit (EXIT_FAILURE);          \
                            }


enum error_memory {

    NO_ERROR,
    FILE_AREN_T_OPENING,
    MEMORY_NOT_FOUND,
    EMPTY_FILE
};


unsigned int amountOfString (FILE * compFile, unsigned long long filesize);
void ASM (unsigned long amount_of_strings, char ** getAdress, FILE * aftercompFile);
unsigned long FileSize (FILE * file);
unsigned int getBuffer (char ** mem_start, unsigned long filesize,\
                            unsigned long * amount_of_string, FILE * file);
unsigned int InitializePointersArray (char *** getAdress, char * mem_start, unsigned long filesize,\
                              unsigned long amount_of_string);
void pointerGetStr (char * buffer, char ** getAdress, unsigned long filesize);
void recordInBuffer (char * mem_start);


int main (void) {


	FILE * compFile = fopen ("compileFile.txt", "r");
	CHECK_ERROR (compFile == NULL, "Problem with opening compileFile.txt", FILE_AREN_T_OPENING);
	FILE * aftercompFile = fopen ("afterCompileFile.txt", "w");
	CHECK_ERROR (aftercompFile == NULL, "Problem with opening afterCompileFile.txt", FILE_AREN_T_OPENING);
	unsigned long filesize = FileSize (compFile), amount_of_strings = 0;
	CHECK_ERROR (filesize == 0, "The compilefile is empty.", EMPTY_FILE);
	char * mem_start = NULL, ** getAdress = NULL, ** command = NULL;;
    MAIN_DET (getBuffer (&mem_start, filesize, &amount_of_strings, compFile));
    MAIN_DET (InitializePointersArray (&getAdress, mem_start, filesize, amount_of_strings));
    ASM (amount_of_strings, getAdress, aftercompFile);
    

}


unsigned int amountOfString (char * mem, unsigned long filesize) {

    unsigned long indexFile = 0, amount = 0;
    for (indexFile = 0; indexFile < filesize; indexFile++)
        if (*(mem + indexFile) == '\0')
            amount++;

    return amount + 1;
}


void ASM (unsigned long amount_of_strings, char ** getAdress, FILE * aftercompFile) {

	char cmd [MAXLENCOMMAND] = " ";
	int val = 0;

	int i = 0;
	for (i = 0; i < amount_of_strings; i++) {

		sscanf (getAdress [i], "%s", cmd);
		//sscanf (getAdress [i] + 4, "%d", val);
		printf ("%s\n", cmd);
		// printf ("%d", val);
	}
	
}

/*
void converter (char * cmd) {

	int val = 0;

	if (!stricmp (cmd, PUSH))
		sscanf (, "%d", &val)

}

*/

unsigned long FileSize (FILE * compfile) {

    struct stat buf = {};
    if (fstat (fileno (compfile), &buf) == 0)
        return buf.st_size;

    return 0;
}


unsigned int getBuffer (char ** mem_start, unsigned long filesize,\
                            unsigned long * amount_of_strings, FILE * file) {

    * mem_start = (char * ) calloc (filesize, sizeof (char));
    CHECK_ERROR (* mem_start == NULL, "Memory not allocated for mem_start.", MEMORY_NOT_FOUND);
    fread (* mem_start, sizeof (char), filesize, file);
    recordInBuffer (* mem_start);
    * amount_of_strings = amountOfString (* mem_start, filesize);

    return NO_ERROR;
}


unsigned int InitializePointersArray (char *** getAdress, char * mem_start, unsigned long filesize,\
                              unsigned long amount_of_string) {

    * getAdress = (char ** )calloc (amount_of_string, sizeof (char * ));
    CHECK_ERROR (* getAdress == NULL, "Memory not allocated for getAdress.", MEMORY_NOT_FOUND);
    pointerGetStr (mem_start, * getAdress, filesize);

    return NO_ERROR;
}


void pointerGetStr (char * buffer, char ** getAdress, unsigned long filesize) {

    getAdress [0] = &buffer [0];
    
    unsigned long i = 1, j = 1;
    for (i = 0; i < filesize; i++) {
        if (buffer[i] == '\0') {

            getAdress [j] = &buffer [i+1];
            j++;
        }
    }
}


void recordInBuffer (char * mem_start) {

    int amount_of_symbols = strlen (mem_start);
    for (int i = 0; i < amount_of_symbols; i++) {

        if (mem_start[i] == EOF)
            mem_start[i] = '\0';

        if (mem_start [i] == '\n') {

            mem_start[i] = '\0';
            continue;
        }
    }

}