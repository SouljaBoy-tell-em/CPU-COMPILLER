#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define MAXLENCOMMAND 50
#define AMOUNTLABELS 30


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


/*-----------COMMANDS CPU-----------*/

enum commands {

    PUSH = 1,
    ADD,
    SUB,
    MUL,
    DIV,
    IN,
    JMP,
    DUMP,
    OUT,
    HLT
};

/*----------------------------------*/

enum error_memory {

    NO_ERROR,
    FILE_AREN_T_OPENING,
    MEMORY_NOT_FOUND,
    EMPTY_FILE
};


typedef struct {

    char name [MAXLENCOMMAND];
    int numberStringLabel;

} Node;


typedef struct {

    Node arrayLabels [AMOUNTLABELS];
    int ip;

} Label;


unsigned int amountOfString (char * mem, unsigned long filesize);
void converter (int * commandsArray, char ** getAdress, unsigned long amount_of_strings, Label * labels);
bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings, Label ** labels);
int detect2ndLabel (char * getAdress, Label * labels);
unsigned long FileSize (FILE * file);
void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress, Label * labels, int numString);
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


    char  *  mem_start     = NULL, 
          ** getAdress     = NULL;
    int   *  commandsArray = NULL;
    Label *  labels        = NULL;


    CHECK_ERROR (createCommandsArray (&commandsArray, amount_of_strings, &labels) == false, "Problem with allocating memory.", MEMORY_NOT_FOUND);
    MAIN_DET (getBuffer (&mem_start, filesize, &amount_of_strings, compFile));
    InitializePointersArray (&getAdress, mem_start, filesize, amount_of_strings);
    pointerGetStr (mem_start, getAdress, filesize);
    converter (commandsArray, getAdress, amount_of_strings, labels);

    for (int i = 0; i < amount_of_strings * 2; i++)
       printf ("%d ", commandsArray [i]);

   printf ("\n\n");
    
    for (int i = 0; i < AMOUNTLABELS; i++) {

        printf ("%s ", (labels->arrayLabels)[i].name);
        printf ("NUM STRING: %d\n", (labels->arrayLabels)[i].numberStringLabel);
    }

    return 0;
}


bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings, Label ** labels) {

    * bufferNumberCommands = (int * ) calloc (2 * amount_of_strings, sizeof (int));
    * labels = (Label * ) calloc (1, sizeof (Label));
    ( * labels)->ip = 0;
    
    if (bufferNumberCommands == NULL)
        return false;
    return true;
}


unsigned int amountOfString (char * mem, unsigned long filesize) {

    unsigned long indexFile = 0, amount = 0;
    for (indexFile = 0; indexFile < filesize; indexFile++)
        if ( * (mem + indexFile) == '\0')
            amount++;

    return amount + 1;
}


void converter (int * commandsArray, char ** getAdress, unsigned long amount_of_strings, Label * labels) {

    char capacityBuffer [MAXLENCOMMAND];
    int numString = 0, val = 0, j = 0;

    for (numString = 0; numString < amount_of_strings; numString++) {

        sscanf (getAdress [numString], "%s", capacityBuffer);
        getAssemblerCommands (capacityBuffer, commandsArray, getAdress [numString], labels, numString);
    }
}


int detect2ndLabel (char * getAdress, Label * labels) {

    int startLabel = (int) (strchr (getAdress, ':') - getAdress);
    char capacityBuffer [MAXLENCOMMAND];
    strcpy (capacityBuffer, getAdress + startLabel + 1);
    
    int i = 0;
    for (i = 0; i < labels->ip; i++)
        if (strcmp (capacityBuffer, (labels->arrayLabels)[i].name) == 0)
            return (labels->arrayLabels)[i].numberStringLabel;

    return -1;
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


void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress, Label * labels, int numString) {

    int lenNameLabel = strlen (capacityBuffer);

    if (capacityBuffer [lenNameLabel - 1] == ':') {

        strncpy ((labels->arrayLabels[labels->ip]).name, capacityBuffer, lenNameLabel - 1);
        labels->arrayLabels[labels->ip].numberStringLabel = numString;
        labels->ip++;
        return;
    }

    int val = 0;
    static int j = 0;

    if (!strcmp ("push", capacityBuffer)   ) {

        commandsArray [j] =   PUSH;    j++;
        val =       skipSpaces (getAdress);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("add", capacityBuffer)    ) {

        commandsArray [j] =   ADD;     j++;
    }

    if (!strcmp ("sub", capacityBuffer)    ) {

        commandsArray [j] =   SUB;     j++;
    }

    if (!strcmp ("mul", capacityBuffer)    ) {

        commandsArray [j] =   MUL;     j++;
    }

    if (!strcmp ("div", capacityBuffer)    ) {

        commandsArray [j] =   DIV;     j++;
    }

    if (!strcmp ("in", capacityBuffer)     ) {

        commandsArray [j] =   IN;      j++;

        if (scanf ("%d", &val) == 0)
            exit (EXIT_FAILURE);

        commandsArray [j] = val;       j++;  
    }

    if (!strcmp ("jmp", capacityBuffer)    ) {

        commandsArray [j] =   JMP;     j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("dump", capacityBuffer)   ) {

        commandsArray [j] =   DUMP;    j++;
    }

    if (!strcmp ("out", capacityBuffer)    ) {

        commandsArray [j] =   OUT;     j++;
    }

    if (!strcmp ("hlt", capacityBuffer)    ) {

        commandsArray [j] =  HLT;      j++;
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
