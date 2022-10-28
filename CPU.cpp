#include "head_CPU.h"
#include "head_stack.h"
#include "CPU.h"
#include "stack.h"


int main (void) {

	FILE * binaryFile 		=                 fopen ("binaryFile.bin", "rb");
	CHECK_ERROR (binaryFile == NULL, "Problem with opening compileFile.txt");

	int amount_commands = FileSize (binaryFile) / sizeof (int);
	int commandsArray [MAXLENCOMMAND];

    fread (commandsArray, sizeof (int), 	   amount_commands, binaryFile);

	Stack stack = {};
	dumpFileCleaning ();
	StackCtor (&stack, amount_commands);

	int * memoryRAM = NULL;
	CHECK_ERROR (createRAM (&memoryRAM) > 0, "memoryRAM didn't allocated a free memory.");

    Register * registers = NULL;
    CHECK_ERROR (InitializeStructRegistersArray (&registers), "memoryRAM didn't allocated a free memory.");

    createRegisters (registers);
    addingInStack (&stack, commandsArray, registers, memoryRAM, amount_commands);

	return 0;
}