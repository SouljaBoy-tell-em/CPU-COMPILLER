#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>


#define AMOUNTCOMMANDS 20
#define MAXLENCOMMAND 50
#define MAXLEN 512
#define CANARY 0xDEDDED32
#define HASH_COEFFICIENT 0xDEADF00D
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
    JB
};


void addingInStack (Stack * stack, int * commandsArray);
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
Elem_t StackPop (Stack * stack);
void StackPush (Stack * stack, Elem_t addDataElement);
void StackReSizeDown (Stack * stack);
void StackReSizeUp (Stack * stack, Elem_t addDataElement);
void UninititalizeElements (Stack * stack);


int main (void) {

	Stack stack = {};
	STACKNAME (stack.name, stack);
	dumpFileCleaning ();
	StackCtor (&stack, AMOUNTCOMMANDS);

	FILE * binaryFile = fopen ("binaryFile.bin", "rb");
	int commandsArray [MAXLENCOMMAND];
    fread (commandsArray, sizeof (int), AMOUNTCOMMANDS, binaryFile);

    for (int i = 0; i < AMOUNTCOMMANDS; i++)
    	printf ("%d ", commandsArray [i]);
    printf ("\n\n");

    addingInStack (&stack, commandsArray);

	return 0;
}


void addingInStack (Stack * stack, int * commandsArray) {

	int i = 0, commandFlag = 0, j = 0;
	for (i = 2; i < AMOUNTCOMMANDS; i++) {

		if (commandFlag == 0) {

			if (commandsArray [i] == ADD)
				StackPush (stack, StackPop (stack) + StackPop (stack));

			if (commandsArray [i] == SUB)
				StackPush (stack, StackPop (stack) - StackPop (stack));

			if (commandsArray [i] == MUL) 
				StackPush (stack, StackPop (stack) * StackPop (stack));

			if (commandsArray [i] == DIV)
				StackPush (stack, StackPop (stack) / StackPop (stack));

		}

		if (commandFlag == 1) {

			StackPush (stack, commandsArray [i]);
			commandFlag = 0;
			continue;
		}

		if (commandsArray [i] == PUSH) {

			commandFlag = 1;
		}

		if (commandsArray [i] == JMP)
			i = commandsArray [i + 1] - 1;

	}

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
		fprintf (dump, "*[%d] = %d\n", i, * ((Elem_t * )(getStartData (stack) + i * sizeof (Elem_t))));


}


Elem_t StackPop (Stack * stack) {

	StackError (stack);

	Elem_t save = * ((Elem_t * )(getStartData (stack) + (stack->size - 1) * sizeof (Elem_t)));

	if (stack->capacity > 2 * stack->size)
		StackReSizeDown (stack);

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
