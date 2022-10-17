#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>


//--------STACK LIBRARY-------//

#include <stdint.h>
#include <limits.h>

//----------------------------//


#define MAXLENCOMMAND 50
#define LENREGISTER 5
#define AMOUNTLABELS 30
#define AMOUNTREGISTERS 15
#define SIGNATURE 0xDEDDED32
#define VERSION 1
#define POISON -228


//----------------------------//

#define MAXLEN 512
#define CANARY 0xDEDDED32
#define HASH_COEFFICIENT 0xDEADF00D
#define RATIO_SIZE_STACK 2
#define DETERMINERROR 000000001
#define STACKNAME(INIT, var) strcpy (INIT, (#var))
#define STATUS(codeError) (codeError) ? "error": "ok"

//----------------------------//


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
    POP
};

/*----------------------------------*/

enum error_memory {

    FILE_AREN_T_OPENING = 1,
    MEMORY_NOT_FOUND,
    EMPTY_FILE
};


//----------------------------//

enum errors {

    NO_ERROR = 0,
    BAD_DATA = 1 << 1,
    BAD_SIZE = 1 << 2,
    BAD_CAPACITY = 1 << 3,
    OVERFLOW = 1 << 4,
    BAD_START_ELEM_T_CANARY = 1 << 5,
    BAD_FINISH_ELEM_T_CANARY = 1 << 6,
    BAD_START_STRUCT_CANARY = 1 << 7,
    BAD_FINISH_STRUCT_CANARY = 1 << 8,
    STACK_NULL = 1 << 9
};

//----------------------------//


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
    int equationRegister;
} Register;


//----------------------------//

typedef double Elem_t;
typedef struct {

    long long startStructCanary;
    char name [MAXLEN];
    Elem_t * data;
    int size;
    int capacity;
    unsigned int code_of_error;
    long long hash_stack;
    Elem_t hash_data;
    long long finishStructCanary;
} Stack;     

//----------------------------//


unsigned int amountOfString (char * mem, unsigned long filesize);
void compile (int * commandsArray, char ** getAdress, unsigned long amount_of_strings, Label * labels, Register * registers);
bool createCommandsArray (int ** bufferNumberCommands, unsigned long amount_of_strings, Label ** labels);
bool createRegisters (Register * registers);
void decompilation (int * commandsArray, Label * labels, FILE * fileDecompilation, unsigned long sizeCommandsArray);
void decompilationCommand (int command, FILE * fileDecompilation, int * flagDualCommands);
int detect2ndLabel (char * getAdress, Label * labels);
int exploreRegister (char * arg, Register * registers);
unsigned long FileSize (FILE * file);
void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress, Label * labels, int numString, Register * registers);
unsigned int getBuffer (char ** mem_start, unsigned long filesize,\
                            unsigned long * amount_of_string, FILE * file);
unsigned int get2ndArg (char * getAdress, Register * registers);
unsigned int InitializePointersArray (char *** getAdress, char * mem_start, unsigned long filesize,\
                              unsigned long amount_of_string);
unsigned int InitializeStructRegistersArray (Register ** registers);
void pointerGetStr (char * buffer, char ** getAdress, unsigned long filesize);
void recordInBuffer (char * mem_start);




//----------------------------//

void dumpFileCleaning (void);
void errorsDecoder (Stack * stack, FILE * dump);
unsigned int fullCodeError (Stack * stack);
uint8_t * getStartData (Stack * stack);
unsigned int hashFunction (Stack * stack);
void StackClear (Stack * stack);
void StackCtor (Stack * stack, int capacity);
void StackDump (Stack * stack, int lineStackDump, const char * nameFunctionDump, const char * fileFunctionDump);
void StackError (Stack * stack);
void StackInfoDump (Stack * stack, FILE * dump);
void StackPop (Stack * stack);
void StackPush (Stack * stack, Elem_t addDataElement);
void StackReSizeDown (Stack * stack);
void StackReSizeUp (Stack * stack, Elem_t addDataElement);
void UninititalizeElements (Stack * stack);

//----------------------------//



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
    registers [0].equationRegister = 11;
    registers [1].equationRegister = -2;
    pointerGetStr (mem_start, getAdress, filesize);
    compile (commandsArray, getAdress, amount_of_strings, labels, registers);
    decompilation (commandsArray, labels, fileDecompilation, 2 * amount_of_strings);

    /*

    for (int i = 0; i < amount_of_strings * 2; i++)
       printf ("%d ", commandsArray [i]);
    printf ("\n\n");

    */

    /*
    
    for (int i = 0; i < AMOUNTLABELS; i++) {

        printf ("%s ", (labels->arrayLabels)[i].name);
        printf ("NUM STRING: %d\n", (labels->arrayLabels)[i].numberStringLabel);
    }
    
    */

    // -----------------------STACK CREATE-------------------- //

    Stack stack = {};
    STACKNAME (stack.name, stack);
    dumpFileCleaning ();
    StackCtor (&stack, 45);
    

    for (int i = 0; i < 30; i++)
        StackPush (&stack, (Elem_t) (i + 1));

    //for (int i = 0; i < 3; i++)
    //  StackPop (&stack);

    // ----------------------------------------------------- //




    FILE * binaryFile = fopen ("binaryFile.bin", "wb");
    CHECK_ERROR (binaryFile == NULL, "Problem with opening binaryFile.txt", FILE_AREN_T_OPENING);
    fwrite (commandsArray, sizeof (int), 2 * amount_of_strings, binaryFile);
    fclose (binaryFile);
    FILE * binaryFile1 = fopen ("binaryFile.bin", "rb");
    fread (commandsArray, sizeof (int), 2 * amount_of_strings, binaryFile1);
    for (int i = 0; i < 2 * amount_of_strings; i++)
        printf ("%02x ", commandsArray [i]);
    printf ("\n\n\n");

    createRegisters (registers);
    for (int i = 0; i < AMOUNTREGISTERS; i++){

        printf ("REGISTER:  %s\n", registers[i].name);
        printf ("EQUATION: %d\n", registers[i].equationRegister);
    }


    return 0;
}


unsigned int amountOfString (char * mem, unsigned long filesize) {

    unsigned long indexFile = 0, amount = 0;
    for (indexFile = 0; indexFile < filesize; indexFile++)
        if ( * (mem + indexFile) == '\0')
            amount++;

    return amount + 1;
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
        (registers + i)->equationRegister = POISON;
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


void decompilation (int * commandsArray, Label * labels, FILE * fileDecompilation, unsigned long sizeCommandsArray) {

    int flagDualCommands = 0;
    int i = 0;
    for (i = 2; i < sizeCommandsArray; i++) 
        decompilationCommand (commandsArray [i], fileDecompilation, &flagDualCommands);
}


void decompilationCommand (int command, FILE * fileDecompilation, int * flagDualCommands) {

    //--------------SIMPLE COMMANDS--------------//

    if ( * flagDualCommands == 0) {

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

    if ( * flagDualCommands == 1) {

        fprintf (fileDecompilation, "%d\n", command);
        * flagDualCommands = 0;
        return;
    }

    if ( * flagDualCommands == 2) {

        fprintf (fileDecompilation, "(%d)\n", command);
        * flagDualCommands = 0;
        return;
    }

    if ( * flagDualCommands == 3) {

        fprintf (fileDecompilation, "%d\n", command);
        * flagDualCommands = 0;
        return;
    }

    if (command == PUSH) {

        fprintf (fileDecompilation, "push ");
        * flagDualCommands = 1;
        return;
    }

    if (command == IN) {

        fprintf (fileDecompilation, "in ");
        * flagDualCommands = 2;
    }

    if (command == JMP) {

        fprintf (fileDecompilation, "jmp ");
        * flagDualCommands = 3;
        return;
    }

    if (command == POP) {

        fprintf (fileDecompilation, "pop ");
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
        return POISON;

    char secondLetterStartRegister = 'a';

    int i = 0;
    for (i = 0; i < AMOUNTREGISTERS; i++) {

        if ( * (arg + 1) == secondLetterStartRegister)
            return registers[i].equationRegister;

        secondLetterStartRegister++;
    }

    return POISON;
}   


unsigned long FileSize (FILE * compfile) {

    struct stat buf = {};
    if (fstat (fileno (compfile), &buf) == 0)
        return buf.st_size;

    return 0;
}


void getAssemblerCommands (char * capacityBuffer, int * commandsArray, char * getAdress, Label * labels, int numString, Register * registers) {

    int lenNameLabel = strlen (capacityBuffer);

    if (capacityBuffer [lenNameLabel - 1] == ':') {

        strncpy ((labels->arrayLabels[labels->ip]).name, capacityBuffer, lenNameLabel - 1);
        labels->arrayLabels[labels->ip].numberStringLabel = numString;
        labels->ip++;
        return;
    }

    int val = 0;
    static int j = 2;

    if (!strcmp ("push", capacityBuffer)   ) {

        commandsArray [j] =   PUSH;    j++;
        val =        get2ndArg (getAdress, registers);
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

    if (!strcmp ("pop", capacityBuffer)    ) {

        commandsArray [j] =  POP;      j++;
    }

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


unsigned int get2ndArg (char * getAdress, Register * registers) {

    char arg [MAXLENCOMMAND];
    int lenStr = 0, val = 0;

    sscanf (getAdress, "%s%n", arg, &lenStr);
    while (isspace ( * (getAdress + lenStr)))
        lenStr++;

    if (sscanf (getAdress + lenStr, "%d", &val))
        return val;

    else {

        sscanf (getAdress, "%s %s", arg, arg);
        return exploreRegister (arg, registers);
    }
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











// -----------------------STACK FUCTIONS-----------------------//



void dumpFileCleaning (void) {

    FILE * dumpfile = fopen ("dumpfile.txt", "w");
    fclose (dumpfile);
}


void errorsDecoder (Stack * stack, FILE * dump) {

        if (stack->code_of_error & BAD_DATA)
            fprintf (dump, "Problem with memory for Elem_t * data.\n");
        
        if (stack->code_of_error & BAD_CAPACITY)
            fprintf (dump, "CAPACITY < 0.\n");

        if (stack->code_of_error & BAD_SIZE)
            fprintf (dump, "SIZE < 0.\n");

        if (stack->code_of_error & OVERFLOW)
            fprintf (dump, "CAPACITY < SIZE\n");

        if (stack->code_of_error & BAD_START_ELEM_T_CANARY)
            fprintf (dump, "Incorrect start canary of the Elem_t *.\n");

        if (stack->code_of_error & BAD_FINISH_ELEM_T_CANARY)
            fprintf (dump, "Incorrect final canary of the Elem_t *.\n");

        if (stack->code_of_error & BAD_START_STRUCT_CANARY)
            fprintf (dump, "Incorrect start canary of the stack.\n");

        if (stack->code_of_error & BAD_FINISH_STRUCT_CANARY)
            fprintf (dump, "Incorrect final canary of the stack.\n");

        if (stack->code_of_error & STACK_NULL)
            fprintf (dump, "Bad pointer on the stack.\n");
}


unsigned int fullCodeError (Stack * stack) {

    if (stack == NULL)
        stack->code_of_error = (stack->code_of_error) | STACK_NULL;

    if (stack->data == NULL)
        stack->code_of_error = (stack->code_of_error) | BAD_DATA;

    if (stack->size < 0)
        stack->code_of_error = (stack->code_of_error) | BAD_SIZE;

    if (stack->capacity < 0)
        stack->code_of_error = (stack->code_of_error) | BAD_CAPACITY;

    if (stack->capacity < stack->size)
        stack->code_of_error = (stack->code_of_error) | OVERFLOW;

    if (stack->startStructCanary != CANARY)
        stack->code_of_error = (stack->code_of_error) | BAD_START_ELEM_T_CANARY;

    if (stack->finishStructCanary != CANARY)
        stack->code_of_error = (stack->code_of_error) | BAD_FINISH_ELEM_T_CANARY;

    if ( * ((long long * ) stack->data) != CANARY)
        stack->code_of_error = (stack->code_of_error) | BAD_START_ELEM_T_CANARY;

    if ( * ((long long * )(getStartData (stack) + sizeof (Elem_t) * stack->capacity)) != CANARY)
        stack->code_of_error = (stack->code_of_error) | BAD_FINISH_ELEM_T_CANARY;

    return stack->code_of_error;
}


uint8_t * getStartData (Stack * stack) {

    return (uint8_t * )stack->data + sizeof (long long);
}


unsigned int hashFunction (Stack * stack) {

    uint8_t * data = (uint8_t * ) stack;
    unsigned int len = stack->size * sizeof (Elem_t), save = 0, hash = 0;

    while (len > 4) {

        save =                         * data;
        save = save |  (( * (data + 1)) << 8);
        save = save | (( * (data + 2)) << 16);
        save = save | (( * (data + 3)) << 24);
        save =        save * HASH_COEFFICIENT;
        save =            save ^ (save >> 24);
        save =        save * HASH_COEFFICIENT;
        hash *=              HASH_COEFFICIENT;
        hash ^=                          save;
        data +=                             4;
        len  -=                             4;
    }

    if (hash > UINT_MAX)
        hash = UINT_MAX - hash;

    return hash;
}


void StackClear (Stack * stack) {

    StackError (stack);
    int i = 0;
    for (i = 0; i < stack->capacity; i++)
        * (Elem_t * ) (getStartData (stack) + sizeof (Elem_t) * i) = POISON;
    stack->size = 0;
    StackError (stack);
}


void StackCtor (Stack * stack, int capacity) {

    stack->code_of_error = NO_ERROR;

    if (stack == NULL)
        stack->code_of_error = stack->code_of_error | STACK_NULL;

    if ((stack->data = (Elem_t * ) malloc (capacity * sizeof (Elem_t) + 2 * sizeof (long long))) == NULL)
        stack->code_of_error = stack->code_of_error | BAD_DATA;

    stack->capacity = capacity;
    if (stack->capacity < 0)
        stack->code_of_error = stack->code_of_error | BAD_CAPACITY;

    stack->size                                                                  =      0;
    stack->startStructCanary                                                     = CANARY;
    stack->finishStructCanary                                                    = CANARY;
    * ((long long * ) stack->data)                                               = CANARY;
    * ((long long * )(getStartData (stack) + sizeof (Elem_t) * stack->capacity)) = CANARY;
    UninititalizeElements (stack);
}


void StackDump (Stack * stack, int lineStackDump, const char * nameFunctionDump, const char * fileFunctionDump) {

    FILE * dump = fopen ("dumpfile.txt", "w");
    if (dump == NULL) exit (EXIT_FAILURE);
    fprintf (dump, "Stack [%p] (%s)\nStack called line %d in function %s at %s.\n"
                "\n", stack, STATUS (stack->code_of_error),lineStackDump, nameFunctionDump, fileFunctionDump);
    fprintf (dump, "CODE OF ERRORS: %d. ERRORS:\n", stack->code_of_error);

    stack->hash_stack = hashFunction (stack);
    if (stack->code_of_error) errorsDecoder (stack, dump);
    StackInfoDump (stack, dump);
    fclose (dump);
} 


void StackError (Stack * stack) {

        fullCodeError (stack);
        StackDump (stack, __LINE__, __PRETTY_FUNCTION__, __FILE__);
}


void StackInfoDump (Stack * stack, FILE * dump) {

    fprintf (dump, "\nINFO:                     \n"                                     );
    fprintf (dump, "STACK NAME:             %s  \n",       stack->name                  );
    fprintf (dump, "STACK_HASH:             %lld\n",       stack->hash_stack            );
    //fprintf (dump, "DATA_HASH:              %lf \n",       stack->hash_data             );
    fprintf (dump, "START STRUCT CANARY:    %llx\n",       stack->startStructCanary     );
    fprintf (dump, "FINISH STRUCT CANARY:   %llx\n",       stack->finishStructCanary    );
    fprintf (dump, "START ELEM_T * CANARY:  %llx\n",       * (long long * )(stack->data));
    fprintf (dump, "FINISH ELEM_T * CANARY: %llx\n",       * ((long long * )((uint8_t * )\
              stack->data + sizeof (long long) + sizeof (Elem_t) * stack->capacity))     );
    fprintf (dump, "STACK SIZE:               %d\n",       stack->size                  );
    fprintf (dump, "STACK CAPACITY:           %d\n",       stack->capacity              );

    int i = 0;
    for (i = 0; i < stack->capacity; i++)
        fprintf (dump, "*[%d] = %lf\n", i, * ((Elem_t * )(getStartData (stack) + i * sizeof (Elem_t))));


}


void StackPop (Stack * stack) {

    StackError (stack);

    if (stack->capacity > 2 * stack->size)
        StackReSizeDown (stack);

    * ((Elem_t * )(getStartData (stack) + (stack->size - 1) * sizeof (Elem_t))) = POISON;
    stack->size--;
    
    StackError (stack);
}


void StackPush (Stack * stack, Elem_t addDataElement) {

    StackError (stack);
    if (stack->size > stack->capacity)
        StackReSizeUp (stack, addDataElement);

    * ((Elem_t * )(getStartData (stack) + stack->size * sizeof (Elem_t))) = addDataElement;
    stack->size++;
    StackError (stack);
}


void StackReSizeDown (Stack * stack) {

    StackError (stack);

    UninititalizeElements (stack);
    * (Elem_t * )(getStartData (stack) + (stack->size - 1) * sizeof (Elem_t)) = CANARY;
    stack->capacity /= 2;

    StackError (stack);
}


void StackReSizeUp (Stack * stack, Elem_t addDataElement) {

    StackError (stack);

    uint8_t * dataPointer = getStartData (stack);
    * ((Elem_t * )(dataPointer + sizeof (Elem_t) * stack->capacity)) = addDataElement;
    stack->data = (Elem_t * ) realloc (stack->data, sizeof (Elem_t) * RATIO_SIZE_STACK * stack->capacity + 2 * sizeof (long long));
    stack->capacity =  RATIO_SIZE_STACK * stack->capacity;
    * ((long long * )(dataPointer + sizeof (Elem_t) * stack->capacity)) = CANARY;
    UninititalizeElements (stack);

    StackError (stack);
}


void UninititalizeElements (Stack * stack) {

    StackError (stack);
    int i = 0;
    for (i = stack->size; i < stack->capacity; i++)
        * (Elem_t * ) (getStartData (stack) + sizeof (Elem_t) * i) = POISON;
    StackError (stack);
}
