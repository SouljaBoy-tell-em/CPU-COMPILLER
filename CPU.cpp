#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/stat.h>


#define TURNOFFMASKPOP ~(1 << 27)
#define MASKPOP 1 << 27
#define INTRAMELEMENTS 400
#define MASKIMMED 1 << 28
#define TURNOFFMASKIMMED ~(1 << 28)
#define MASKREGISTER 1 << 29
#define TURNOFFMASKREGISTER ~(1 << 29)
#define MASKRAM 1 << 30
#define TURNOFFMASKRAM ~(1 << 30)
#define TURNOFFMASKRAMANDIMMED (~(1 << 30)) ^ (~(1 << 28))
#define TURNOFFMASKRAMANDREGISTER (~(1 << 30)) ^ (~(1 << 29))
#define AMOUNTREGISTERS 25
#define AMOUNTCOMMANDS 75
#define LENREGISTER 5
#define MAXLENCOMMAND 50
#define MAXLEN 512
#define CANARY 0xDEDDED32
#define RATIO_SIZE_STACK 2
#define DETERMINERROR 000000001
#define POISON -228
#define STACKNAME(INIT, var) strcpy (INIT, (#var))
#define STATUS(codeError) (codeError) ? "error": "ok"
#define CHECK_ERROR(condition, message_error) 		      \
            do {                                          \
               if  (condition) {                          \
                   printf ("%s", message_error);          \
                   return 1;                              \
               }                                          \
            } while(false)


typedef int Elem_t;

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

typedef struct {

    char name [LENREGISTER];
    int equationRegister;
} Register;


enum error_memory {

    FILE_AREN_T_OPENING = 1,
    MEMORY_NOT_FOUND,
    EMPTY_FILE,
    BAD_EQUATION
};


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
    JNE,
    ENDL
};

int addingInStack (Stack * stack, int * commandsArray, Register * registers, int * ramElements, int amount_commands);
int createRAM (int ** ramElements);
bool createRegisters (Register * registers);
bool detectPush (int * commandFlag, int commandsArray, Register * registers, Stack * stack, int * ramElements);
void dumpFileCleaning (void);
void errorsDecoder (Stack * stack, FILE * dump);
int exploreRegister (char * arg, Register * registers);
void firstArgCommands (int commandsArray, Stack * stack);
unsigned long FileSize (FILE * compfile);
unsigned int fullCodeError (Stack * stack);
uint8_t * getStartData (Stack * stack);
void InitializeCommamdsArray (int * commandsArray, int amount_commands);
unsigned int InitializeStructRegistersArray (Register ** registers);
void StackClear (Stack * stack);
void StackCtor (Stack * stack, int capacity);
void StackDump (Stack * stack, int lineStackDump, const char * nameFunctionDump, const char * fileFunctionDump);
void StackError (Stack * stack);
void StackInfoDump (Stack * stack, FILE * dump);
Elem_t StackPop (Stack * stack);
void StackPush (Stack * stack, Elem_t addDataElement);
void StackReSizeDown (Stack * stack);
void StackReSizeUp (Stack * stack, Elem_t addDataElement);
bool theEndJB (Register * registers, Stack * stack, int * upEquationElement, int * commandsArray, int * i);
bool theEndJBE (Register * registers, Stack * stack, int * upEquationElement, int * commandsArray, int * i);
void UninititalizeElements (Stack * stack);


int main (void) {

	FILE * binaryFile = fopen ("binaryFile.bin", "rb");
	int amount_commands = FileSize (binaryFile) / sizeof (int);
	int commandsArray [MAXLENCOMMAND];
	void InitializeCommamdsArray (int * commandsArray, int amount_commands);
    fread (commandsArray, sizeof (int), amount_commands, binaryFile);

	Stack stack = {};
	STACKNAME (stack.name, stack);
	dumpFileCleaning ();
	StackCtor (&stack, amount_commands);

	int * memoryRAM = NULL;
	CHECK_ERROR (createRAM (&memoryRAM) > 0, "memoryRAM didn't allocated a free memory.");
    memoryRAM [3] = -1;

    Register * registers = NULL;
    InitializeStructRegistersArray (&registers);
    createRegisters (registers);
    (registers + 5)->equationRegister = 3;
    addingInStack (&stack, commandsArray, registers, memoryRAM, amount_commands);

	return 0;
}


int addingInStack (Stack * stack, int * commandsArray, Register * registers, int * ramElements, int amount_commands) {

	int i = 0, commandFlag = 0, j = 0, saveVal1 = 0, saveVal2 = 0, upEquationElement = 0;
	for (i = 2; i < amount_commands; i++) {

		if (commandFlag == 0) {

			firstArgCommands (commandsArray [i], stack);

			if (commandsArray [i] == HLT)
				exit (0);
		}

		if (detectPush (&commandFlag, commandsArray [i], registers, stack, ramElements))
			continue;



		if (commandsArray [i] == JBE) {

			if (!theEndJBE (registers, stack, &upEquationElement, commandsArray, &i)) 
				continue;
			continue;
		}

	}

	return 0;
}


int createRAM (int ** ramElements) {

	* ramElements = (int * ) calloc (INTRAMELEMENTS, sizeof (int));
	if (ramElements == NULL)
		return MEMORY_NOT_FOUND;

	int i = 0;
	for (i = 0; i < INTRAMELEMENTS; i++)
		* ( * ramElements + i) = 0;

	return NO_ERROR;
}


bool detectPush (int * commandFlag, int commandsArray, Register * registers, Stack * stack, int * ramElements) {

	if ( * commandFlag == 1) {

			StackPush (stack, commandsArray);
			* commandFlag = 0;
			return true;
		}

		if ( * commandFlag == 2) {

			StackPush (stack, (registers + commandsArray)->equationRegister);
			* commandFlag = 0;
			return true;
		}

		if ( * commandFlag == 3) {

			StackPush (stack, ramElements [commandsArray]);
			* commandFlag = 0;
			return true;
		}

		if ( * commandFlag == 4) {

			StackPush (stack, ramElements [(registers + commandsArray)->equationRegister]);
			* commandFlag = 0;
			return true;
		}

		if ( * commandFlag == 5) {

			(registers + commandsArray)->equationRegister = StackPop (stack);
			* commandFlag = 0;
			return true;
		}

		if ( * commandFlag == 6) {

			ramElements [commandsArray] = StackPop (stack);
			* commandFlag = 0;
			return true;
		}

		if ( * commandFlag == 7) {

			ramElements [(registers + commandsArray)->equationRegister] = StackPop (stack);
			* commandFlag = 0;
			return true;
		}

		if ((commandsArray & TURNOFFMASKIMMED) == PUSH) {

			* commandFlag = 1;
			return true;
		}

		if ((commandsArray & TURNOFFMASKREGISTER) == PUSH) {

			* commandFlag = 2;
			return true;
		}

		if (((commandsArray & TURNOFFMASKRAM) & TURNOFFMASKIMMED) == PUSH) {

			* commandFlag = 3;
			return true;
		}

		if (((commandsArray & TURNOFFMASKRAM) & TURNOFFMASKREGISTER) == PUSH) {

			* commandFlag = 4;
			return true;
		}

		if ((commandsArray & TURNOFFMASKPOP) == POP) {

			StackPop (stack);
			return true;
		}

		if ((commandsArray & TURNOFFMASKREGISTER) == POP) {

			* commandFlag = 5;
			return true;
		}

		if (((commandsArray & TURNOFFMASKRAM) & TURNOFFMASKIMMED) == POP) {

			* commandFlag = 6;
			return true;
		}

		if (((commandsArray & TURNOFFMASKRAM) & TURNOFFMASKIMMED) == POP) {

			* commandFlag = 7;
			return true;
		}		

	return false;
}


void firstArgCommands (int commandsArray, Stack * stack) {

	if (commandsArray == ADD)
		StackPush (stack, StackPop (stack) + StackPop (stack));

	if (commandsArray == SUB)
		StackPush (stack, StackPop (stack) - StackPop (stack));

	if (commandsArray == MUL) 
		StackPush (stack, StackPop (stack) * StackPop (stack));

	if (commandsArray == DIV)
		StackPush (stack, StackPop (stack) / StackPop (stack));

	if (commandsArray == ENDL)
		printf ("\n");

	if (commandsArray == OUT)
		printf ("%d", StackPop (stack));
}


unsigned long FileSize (FILE * compfile) {

    struct stat buf = {};
    if (fstat (fileno (compfile), &buf) == 0)
        return buf.st_size;

    return 0;
}


bool theEndJB (Register * registers, Stack * stack, int * upEquationElement, int * commandsArray, int * i) {

	registers->equationRegister = StackPop (stack);

	if (( * upEquationElement) < registers->equationRegister) {

		* i = commandsArray [( * i) + 1] - 1;
		( * upEquationElement)++;
		return false;
	}

	* upEquationElement = 0;
	( * i)++;

	return true;
}


bool theEndJBE (Register * registers, Stack * stack, int * upEquationElement, int * commandsArray, int * i) {

	if (registers->equationRegister <= StackPop (stack)) {

		* i = commandsArray [( * i) + 1] - 1;
		return false;
	}
	( * i)++;

	return true;
}


bool createRegisters (Register * registers) {

    char startRegister [LENREGISTER] = "rax";

    int i = 0;
    for (i = 0; i < AMOUNTREGISTERS; i++) {

        strcpy ((registers + i)->name, startRegister);
        (registers + i)->equationRegister = 0;
        startRegister [1]++;
    }
    
    return true;
}


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


void InitializeCommamdsArray (int * commandsArray, int amount_commands) {

	int i = 0;
	for (i = 0; i < amount_commands; i++)
		commandsArray [i] = 0;
}


unsigned int InitializeStructRegistersArray (Register ** registers) {

    * registers = (Register * )calloc (AMOUNTREGISTERS, sizeof (Register));
    if (registers == NULL)
    	return MEMORY_NOT_FOUND;

    return NO_ERROR;
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
	//if (dump == NULL) exit (EXIT_FAILURE);
	fprintf (dump, "Stack [%p] (%s)\nStack called line %d in function %s at %s.\n"
				"\n", stack, STATUS (stack->code_of_error),lineStackDump, nameFunctionDump, fileFunctionDump);
	fprintf (dump, "CODE OF ERRORS: %d. ERRORS:\n", stack->code_of_error);

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
	fprintf (dump, "START STRUCT CANARY:    %llx\n",       stack->startStructCanary     );
	fprintf (dump, "FINISH STRUCT CANARY:   %llx\n",       stack->finishStructCanary    );
	fprintf (dump, "START ELEM_T * CANARY:  %llx\n",       * (long long * )(stack->data));
	fprintf (dump, "FINISH ELEM_T * CANARY: %llx\n",       * ((long long * )((uint8_t * )\
		      stack->data + sizeof (long long) + sizeof (Elem_t) * stack->capacity))     );
	fprintf (dump, "STACK SIZE:               %d\n",       stack->size                  );
	fprintf (dump, "STACK CAPACITY:           %d\n",       stack->capacity              );

	int i = 0;
	for (i = 0; i < stack->capacity; i++)
		fprintf (dump, "*[%d] = %d\n", i, * ((Elem_t * )(getStartData (stack) + i * sizeof (Elem_t))));


}


Elem_t StackPop (Stack * stack) {

	StackError (stack);
	Elem_t save = * ((Elem_t * )(getStartData (stack) + (stack->size - 1) * sizeof (Elem_t)));

/*

	if (stack->capacity > 2 * stack->size)
		StackReSizeDown (stack);

*/

	* ((Elem_t * )(getStartData (stack) + (stack->size - 1) * sizeof (Elem_t))) = POISON;
	stack->size--;
	StackError (stack);

	return save;
}


void StackPush (Stack * stack, Elem_t addDataElement) {

	StackError (stack);
	if (stack->size > stack->capacity)
		StackReSizeUp (stack, addDataElement);

	* ((Elem_t * )(getStartData (stack) + stack->size * sizeof (Elem_t))) = addDataElement;
	stack->size++;
	StackError (stack);
}


void StackReSizeUp (Stack * stack, Elem_t addDataElement) {

	StackError (stack);

	uint8_t * dataPointer = getStartData (stack);
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
