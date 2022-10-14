#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


/*-----------COMMANDS CPU-----------*/
		

#define PUSH "push"     // ---> 1
#define ADD  "add"      // ---> 2
#define SUB  "sub"      // ---> 3
#define MUL  "mul"      // ---> 4
#define DIV  "div"      // ---> 5
#define IN   "in"       // ---> 6
#define JMP  "jmp"      // ---> 7
#define DUMP "dump"     // ---> 8
#define OUT  "out"      // ---> 9
#define HLT  "hlt"      // ---> 10


/*----------------------------------*/

#define MAXLENCOMMAND 50


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


unsigned int amountOfString (char * mem, unsigned long filesize);
void converter (int * commandsArray, char ** getAdress, unsigned long amount_of_strings);  
bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings);
unsigned long FileSize (FILE * file);
void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress);
unsigned int getBuffer (char ** mem_start, unsigned long filesize,\
                            unsigned long * amount_of_string, FILE * file);
unsigned int InitializePointersArray (char *** getAdress, char * mem_start, unsigned long filesize,\
                              unsigned long amount_of_string);
void pointerGetStr (char * buffer, char ** getAdress, unsigned long filesize);
void recordInBuffer (char * mem_start);
unsigned int skipSpaces (char * commandStr);

int main (void) {

	FILE * compFile = fopen ("compileFile.txt", "r");
	CHECK_ERROR (compFile == NULL, "Problem with opening compileFile.txt", FILE_AREN_T_OPENING);
	FILE * aftercompFile = fopen ("afterCompileFile.txt", "w");
	CHECK_ERROR (aftercompFile == NULL, "Problem with opening afterCompileFile.txt", FILE_AREN_T_OPENING);
	unsigned long filesize = FileSize (compFile), amount_of_strings = 0;
	CHECK_ERROR (filesize == 0, "The compilefile is empty.", EMPTY_FILE);

	char * mem_start     = NULL, 
         ** getAdress    = NULL;
    int  * commandsArray = NULL;
    CHECK_ERROR (createCommandsArray (&commandsArray, amount_of_strings) == false, "Problem with allocating memory.", MEMORY_NOT_FOUND);
    MAIN_DET (getBuffer (&mem_start, filesize, &amount_of_strings, compFile));
    InitializePointersArray (&getAdress, mem_start, filesize, amount_of_strings);
    pointerGetStr (mem_start, getAdress, filesize);
    converter (commandsArray, getAdress, amount_of_strings);

    for (int i = 0; i < amount_of_strings * 2; i++)
        printf ("%d ", commandsArray [i]);
    
    return 0;
}


bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings) {

    * bufferNumberCommands = (int * ) calloc (2 * amount_of_strings, sizeof (int));
    
    if (bufferNumberCommands == NULL)
        return false;
    return true;
}


unsigned int amountOfString (char * mem, unsigned long filesize) {

    unsigned long indexFile = 0, amount = 0;
    for (indexFile = 0; indexFile < filesize; indexFile++)
        if (*(mem + indexFile) == '\0')
            amount++;

    return amount + 1;
}


void converter (int * commandsArray, char ** getAdress, unsigned long amount_of_strings) {

    char capacityBuffer [MAXLENCOMMAND];
    int i = 0, val = 0, j = 0;

    for (i = 0; i < amount_of_strings; i++) {

        sscanf (getAdress [i], "%s", capacityBuffer);
        getAssemblerCommands (capacityBuffer, commandsArray, getAdress [i]);
    }


}


unsigned long FileSize (FILE * compfile) {

    struct stat buf = {};
    if (fstat (fileno (compfile), &buf) == 0)
        return buf.st_size;

    return 0;
}


unsigned int getBuffer (char ** mem_start, unsigned long filesize,\
                            unsigned long * amount_of_string, FILE * file) {

    * mem_start = (char * ) calloc (filesize, sizeof (char));
    CHECK_ERROR (* mem_start == NULL, "Memory not allocated for mem_start.", MEMORY_NOT_FOUND);
    fread (* mem_start, sizeof (char), filesize, file);
    recordInBuffer (* mem_start);
    * amount_of_string = amountOfString (* mem_start, filesize);

    return NO_ERROR;
}


void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress) {

    int val = 0;
    static int j = 0;

    if (!strcmp (PUSH, capacityBuffer)   ) {

        commandsArray [j] =   1;     j++;
        val =     skipSpaces (getAdress);
        commandsArray [j] = val;     j++;
    }

    if (!strcmp (ADD, capacityBuffer)    ) {

        commandsArray [j] =   2;     j++;
    }

    if (!strcmp (SUB, capacityBuffer)    ) {

        commandsArray [j] =   3;     j++;
    }

    if (!strcmp (MUL, capacityBuffer)    ) {

        commandsArray [j] =   4;     j++;
    }

    if (!strcmp (DIV, capacityBuffer)    ) {

        commandsArray [j] =   5;     j++;
    }

    if (!strcmp (IN, capacityBuffer)     ) {

        commandsArray [j] =   6;     j++;

        if (scanf ("%d", &val) == 0)
            exit (EXIT_FAILURE);
        commandsArray [j] = val;     j++;  
    }

    if (!strcmp (JMP, capacityBuffer)    ) {

        commandsArray [j] =   7;     j++;
        val =     skipSpaces (getAdress);
        commandsArray [j] = val;     j++;
    }

    if (!strcmp (DUMP, capacityBuffer)   ) {

        commandsArray [j] =   8;     j++;
    }

    if (!strcmp (OUT, capacityBuffer)    ) {

        commandsArray [j] =   9;     j++;
    }

    if (!strcmp (HLT, capacityBuffer)    ) {

        commandsArray [j] =  10;     j++;
    }
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


unsigned int skipSpaces (char * getAdress) {

    int val = 0, indexNumber = 0;
    while (sscanf (getAdress + indexNumber, "%d", &val) != 1)
        indexNumber++;

    return val;
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
