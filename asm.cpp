#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>


#define TURNOFFMASKPOP ~(1 << 27)
#define MASKPOP 1 << 27
#define MASKIMMED 1 << 28
#define TURNOFFMASKIMMED ~(1 << 28)
#define MASKREGISTER 1 << 29
#define TURNOFFMASKREGISTER ~(1 << 29)
#define MASKRAM 1 << 30
#define TURNOFFMASKRAM ~(1 << 30)
#define MAXLENCOMMAND 50
#define LENREGISTER 5
#define AMOUNTLABELS 30
#define AMOUNTREGISTERS 15
#define SIGNATURE 0xDEDDED32
#define VERSION 1
#define POISON -228


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
    HLT,
    POP,
    JB,
    JBE,
    JA,
    JAE,
    JE,
    JNE
};

/*----------------------------------*/

enum error_memory {

    NO_ERROR,
    FILE_AREN_T_OPENING,
    MEMORY_NOT_FOUND,
    EMPTY_FILE,
    BAD_EQUATION
};


typedef struct {

    char name [MAXLENCOMMAND];
    int numberStringLabel;
} Node;


typedef struct {

    Node arrayLabels [AMOUNTLABELS];
    int ip;
} Label;


typedef struct {

    char name [LENREGISTER];
} Register;


unsigned int amountOfString (char * mem, unsigned long filesize);
void closeMemoryPointers (FILE * compFile, FILE * fileDecompilation, FILE * binaryFile, char * mem_start,\
                          char ** getAdress, int * commandsArray, Label * labels, Register * registers);
void compile (int * commandsArray, char ** getAdress, unsigned long amount_of_strings, Label * labels, Register * registers);
bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings, Label ** labels);
bool createRegisters (Register * registers);
void decompilation (int * commandsArray, Label * labels, FILE * fileDecompilation, unsigned long sizeCommandsArray, Register * registers);
void decompilationCommand (int command, FILE * fileDecompilation, int * flagDualCommands, Register * registers);
int detect2ndLabel (char * getAdress, Label * labels);
int exploreRegister (char * arg, Register * registers);
unsigned long FileSize (FILE * file);
void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress, Label * labels, int numString, Register * registers);
unsigned int getBuffer (char ** mem_start, unsigned long filesize,\
                            unsigned long * amount_of_string, FILE * file);
unsigned int get2ndArg (char * getAdress, Register * registers, int * commandArray);
unsigned int InitializePointersArray (char *** getAdress, char * mem_start, unsigned long filesize,\
                              unsigned long amount_of_string);
unsigned int InitializeStructRegistersArray (Register ** registers);
void pointerGetStr (char * buffer, char ** getAdress, unsigned long filesize);
bool popEmptyArg (char * getAdress, int * commandsArray);
void recordInBuffer (char * mem_start);

int main (void) {

    FILE * compFile = fopen ("compileFile.txt", "r");
    CHECK_ERROR (compFile == NULL, "Problem with opening compileFile.txt", FILE_AREN_T_OPENING);
    FILE * fileDecompilation = fopen ("afterCompileFile.txt", "w");
    CHECK_ERROR (fileDecompilation == NULL, "Problem with opening afterCompileFile.txt", FILE_AREN_T_OPENING);
    unsigned long filesize = FileSize (compFile), amount_of_strings = 0;
    CHECK_ERROR (filesize == 0, "The compilefile is empty.", EMPTY_FILE);


    char     *  mem_start     = NULL, 
             ** getAdress     = NULL;
    int      *  commandsArray = NULL;
    Label    *  labels        = NULL;
    Register * registers      = NULL;

    CHECK_ERROR (createCommandsArray (&commandsArray, amount_of_strings, &labels) == false, "Problem with allocating memory for commandsArray", MEMORY_NOT_FOUND);
    MAIN_DET (getBuffer (&mem_start, filesize, &amount_of_strings, compFile));
    InitializePointersArray (&getAdress, mem_start, filesize, amount_of_strings);
    InitializeStructRegistersArray (&registers);
    createRegisters (registers);
    pointerGetStr (mem_start, getAdress, filesize);
    compile (commandsArray, getAdress, amount_of_strings, labels, registers);
    decompilation (commandsArray, labels, fileDecompilation, 2 * amount_of_strings, registers);

    FILE * binaryFile = fopen ("binaryFile.bin", "wb");
    CHECK_ERROR (binaryFile == NULL, "Problem with opening binaryFile.txt", FILE_AREN_T_OPENING);
    fwrite (commandsArray, sizeof (int), 2 * amount_of_strings, binaryFile);
    fclose (binaryFile);



    FILE * binaryFile1 = fopen ("binaryFile.bin", "rb");
    fread (commandsArray, sizeof (int), 2 * amount_of_strings, binaryFile1);

    for (int i = 0; i < 2 * amount_of_strings; i++)
        printf ("%d ", commandsArray [i]);
    printf ("\n\n");


    return 0;
}


unsigned int amountOfString (char * mem, unsigned long filesize) {

    unsigned long indexFile = 0, amount = 0;
    for (indexFile = 0; indexFile < filesize; indexFile++)
        if ( * (mem + indexFile) == '\0')
            amount++;

    return amount + 1;
}


void closeMemoryPointers (FILE * compFile, FILE * fileDecompilation, FILE * binaryFile, char * mem_start,\
                          char ** getAdress, int * commandsArray, Label * labels, Register * registers) {

    fclose (compFile);
    fclose (fileDecompilation);
    fclose (binaryFile);
    free (mem_start);
    free (getAdress);
    free (commandsArray);
    free (labels);
    free (registers);
}


bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings, Label ** labels) {

    * bufferNumberCommands = (int * ) calloc (2 * amount_of_strings + 2, sizeof (int));
    * labels = (Label * ) calloc (1, sizeof (Label));
    ** bufferNumberCommands = SIGNATURE;
    * ( * bufferNumberCommands + 1) = VERSION;
    ( * labels)->ip = 2;
    
    if (bufferNumberCommands == NULL)
        return false;
    return true;
}


bool createRegisters (Register * registers) {

    char startRegister [LENREGISTER] = "rax";

    int i = 0;
    for (i = 0; i < AMOUNTREGISTERS; i++) {

        strcpy ((registers + i)->name, startRegister);
        startRegister [1]++;
    }
    
    return true;
}


void compile (int * commandsArray, char ** getAdress, unsigned long amount_of_strings, Label * labels, Register * registers) {

    char capacityBuffer [MAXLENCOMMAND];
    int numString = 0, val = 0, j = 0;

    for (numString = 0; numString < amount_of_strings; numString++) {

        sscanf (getAdress [numString], "%s", capacityBuffer);
        getAssemblerCommands (capacityBuffer, commandsArray, getAdress [numString], labels, numString, registers);
    }
}


void decompilation (int * commandsArray, Label * labels, FILE * fileDecompilation, unsigned long sizeCommandsArray, Register * registers) {

    int flagCommands = 0;
    int i = 0;
    for (i = 2; i < sizeCommandsArray; i++) 
        decompilationCommand (commandsArray [i], fileDecompilation, &flagCommands, registers);
}


void decompilationCommand (int command, FILE * fileDecompilation, int * flagCommands, Register * registers) {

    //--------------SIMPLE COMMANDS--------------//

    if ( * flagCommands == 0) {

        if (command == ADD)
            fprintf (fileDecompilation, "add\n" );

        if (command == SUB)
            fprintf (fileDecompilation, "sub\n" );

        if (command == MUL)
            fprintf (fileDecompilation, "mul\n" );

        if (command == DIV)
            fprintf (fileDecompilation, "div\n" );

        if (command == DUMP)
            fprintf (fileDecompilation, "dump\n");

        if (command == OUT)
            fprintf (fileDecompilation, "out\n" );

        if (command == HLT)
            fprintf (fileDecompilation, "hlt\n" );
    }

    //-------------------------------------------//

    if ( * flagCommands == MASKIMMED) {

        fprintf (fileDecompilation, "%d\n", command);
        * flagCommands = 0;
        return;
    }

    if ( * flagCommands == MASKREGISTER) {

        fprintf (fileDecompilation, "%s\n", (registers + command)->name);
        * flagCommands = 0;
        return;
    }

    if ( * flagCommands == 4) {

        fprintf (fileDecompilation, "[%d]\n", command);
        * flagCommands = 0;
        return;
    }

    if ( * flagCommands == 5) {

        fprintf (fileDecompilation, "[%s]\n", (registers + command)->name);
        * flagCommands = 0;
        return;
    }

    if ( * flagCommands == 2) {

        fprintf (fileDecompilation, "(%d)\n", command);
        * flagCommands = 0;
        return;
    }

    if ( * flagCommands == 3) {

        fprintf (fileDecompilation, "%d\n", command);
        * flagCommands = 0;
        return;
    }

    if ((command & TURNOFFMASKIMMED) == PUSH) {

        fprintf (fileDecompilation, "push ");
        * flagCommands = MASKIMMED;
        return;
    }

    if ((command & TURNOFFMASKREGISTER) == PUSH) {

        fprintf (fileDecompilation, "push ");
        * flagCommands = MASKREGISTER;
        return;
    }

    if ((command & TURNOFFMASKRAM) == (MASKIMMED | PUSH) || (command & TURNOFFMASKRAM) == (MASKREGISTER | PUSH)) {

        command = command & TURNOFFMASKRAM;

        if ((command & TURNOFFMASKIMMED) == PUSH)
            * flagCommands = 4;

        if ((command & TURNOFFMASKREGISTER) == PUSH)
            * flagCommands = 5;

        fprintf (fileDecompilation, "push ");
        return;
    }

    if ((command & TURNOFFMASKPOP) == POP) {

        fprintf (fileDecompilation, "pop\n");
        return;
    }

    if ((command & TURNOFFMASKIMMED) == POP) {

        fprintf (fileDecompilation, "pop ");
        * flagCommands = MASKIMMED;
        return;
    }

    if ((command & TURNOFFMASKREGISTER) == POP) {

        fprintf (fileDecompilation, "pop ");
        * flagCommands = MASKREGISTER;
        return;
    }

    if ((command & TURNOFFMASKRAM) == (MASKIMMED | POP) || (command & TURNOFFMASKRAM) == (MASKREGISTER | POP)) {

        command = command & TURNOFFMASKRAM;

        if ((command & TURNOFFMASKIMMED) == POP)
            * flagCommands = 4;

        if ((command & TURNOFFMASKREGISTER) == POP)
            * flagCommands = 5;

        fprintf (fileDecompilation, "pop ");
        return;
    }


    if (command == IN) {

        fprintf (fileDecompilation, "in ");
        * flagCommands = 2;
    }

    if (command == JMP) {

        fprintf (fileDecompilation, "jmp ");
        * flagCommands = 3;
    }

    if (command == JB) {

        fprintf (fileDecompilation, "jb ");
        * flagCommands = 3;
    }

    if (command == JBE) {

        fprintf (fileDecompilation, "jbe ");
        * flagCommands = 3;
    }

    if (command == JA) {

        fprintf (fileDecompilation, "ja ");
        * flagCommands = 3;
    }

    if (command == JAE) {

        fprintf (fileDecompilation, "jae ");
        * flagCommands = 3;
    }

    if (command == JE) {

        fprintf (fileDecompilation, "je ");
        * flagCommands = 3;
    }

    if (command == JNE) {

        fprintf (fileDecompilation, "jne ");
        * flagCommands = 3;
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


int exploreRegister (char * arg, Register * registers) {

    if ( * arg != 'r' || * (arg + 2) != 'x')
        return -1;

    char secondLetterStartRegister = 'a';

    int i = 0;
    for (i = 0; i < AMOUNTREGISTERS; i++) {

        if ( * (arg + 1) == secondLetterStartRegister)
            return i;

        secondLetterStartRegister++;
    }

    return -1;
}   


unsigned long FileSize (FILE * compfile) {

    struct stat buf = {};
    if (fstat (fileno (compfile), &buf) == 0)
        return buf.st_size;

    return 0;
}


void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress, Label * labels, int numString, Register * registers) {

    int lenNameLabel = strlen (capacityBuffer), val = 0;
    static int j = 2;

    if (capacityBuffer [lenNameLabel - 1] == ':') {

        strncpy ((labels->arrayLabels[labels->ip]).name, capacityBuffer, lenNameLabel - 1);
        labels->arrayLabels[labels->ip].numberStringLabel = j;
        labels->ip++;
        return;
    }

    if (!strcmp ("push", capacityBuffer)   ) {

        commandsArray [j] =   PUSH;
        val = get2ndArg (getAdress, registers, &commandsArray [j]);
        j++; 
        commandsArray [j] = val;  j++;
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

    if (!strcmp ("pop", capacityBuffer)    ) {

        commandsArray [j] = POP;

        if (!popEmptyArg (getAdress, &commandsArray[j])) {

            j++;
            commandsArray [j] = POISON;
            j++;
            return;
        }
        
        val = get2ndArg (getAdress, registers, &commandsArray [j]);
        j++; 
        commandsArray [j] = val;  
        j++;
    }


    //------------JUMP FUNCTION--------------//

    if (!strcmp ("jb", capacityBuffer)     ) {

        commandsArray [j] = JB;        j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("jbe", capacityBuffer)    ) {

        commandsArray [j] = JBE;       j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("ja", capacityBuffer)    ) {

        commandsArray [j] = JA;        j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("jae", capacityBuffer)    ) {

        commandsArray [j] = JAE;       j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("je", capacityBuffer)    ) {

        commandsArray [j] = JE;        j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    if (!strcmp ("jne", capacityBuffer)    ) {

        commandsArray [j] = JNE;       j++;
        val = detect2ndLabel (getAdress, labels);
        commandsArray [j] = val;       j++;
    }

    //---------------------------------------//

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


unsigned int get2ndArg (char * getAdress, Register * registers, int * commandArray) {

    char arg [MAXLENCOMMAND];
    int lenStr = 0, val = 0;

    sscanf (getAdress, "%s%n", arg, &lenStr);
    while (isspace ( * (getAdress + lenStr)))
        lenStr++;


    sscanf (getAdress + lenStr, "%s", arg);

    if ( * arg == '[' && * (arg + strlen (arg) - 1) == ']') {

        * (getAdress + lenStr) = * (getAdress + lenStr + strlen (arg) - 1) = ' ';
        * commandArray = ( * commandArray) | MASKRAM;
    }

    if (sscanf (getAdress, "%s %d", arg, &val) == 2) {

        * commandArray = ( * commandArray) | MASKIMMED;
        return val;
    } 

    if (sscanf (getAdress, "%s %s", arg, arg) == 2) {

        if ((val = exploreRegister (arg, registers)) >= 0)
            * commandArray = ( * commandArray) | MASKREGISTER;

        return val;
    }

    return BAD_EQUATION;
}


unsigned int InitializePointersArray (char *** getAdress, char * mem_start, unsigned long filesize,\
                              unsigned long amount_of_string) {

    * getAdress = (char ** )calloc (amount_of_string, sizeof (char * ));
    CHECK_ERROR (* getAdress == NULL, "Memory not allocated for getAdress.", MEMORY_NOT_FOUND);
    pointerGetStr (mem_start, * getAdress, filesize);

    return NO_ERROR;
}


unsigned int InitializeStructRegistersArray (Register ** registers) {

    * registers = (Register * )calloc (AMOUNTREGISTERS, sizeof (Register));
    CHECK_ERROR (* registers == NULL, "Memory not allocated for registers.", MEMORY_NOT_FOUND);

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


bool popEmptyArg (char * getAdress, int * commandsArray) {

    char arg [MAXLENCOMMAND];
    int lenStr = 0, val = 0;

    sscanf (getAdress, "%s%n", arg, &lenStr);

    while (isspace ( * (getAdress + lenStr)))
        lenStr++;

    if (lenStr == strlen (getAdress)) {

        * commandsArray = ( * commandsArray) | MASKPOP;
        return false;
    }

    return true;
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
