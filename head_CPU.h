#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include "foundation.h"


int addingInStack (Stack * stack, int * commandsArray, Register * registers, int * ramElements, int amount_commands);
int createRAM (int ** ramElements);
bool createRegisters (Register * registers);
void detectPush (int * commandsArray, int * ip, Register * registers, Stack * stack, int * ramElements);
int exploreRegister (char * arg, Register * registers);
unsigned long FileSize (FILE * compfile);
void InitializeCommamdsArray (int * commandsArray, int amount_commands);
unsigned int InitializeStructRegistersArray (Register ** registers);
bool theEndJB (Register * registers, Stack * stack, int * commandsArray, int * i);
bool theEndJBE (Register * registers, Stack * stack, int * commandsArray, int * i);