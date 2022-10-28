int addingInStack (Stack * stack, int * commandsArray, Register * registers, int * ramElements, int amount_commands) {

	int i = 0;
	for (i = 2; i < amount_commands; i ++) {

		int save = commandsArray [i] & ALLTURNOFF;

		#define DEF_CMD(name, num, arg, ...)      										\
							case num:     		  										\
								__VA_ARGS__		  										\
								break;			  
							

		#define DEF_JMP(name, num, ...)			  										\
							case num:													\
								if (registers->equationRegister <= StackPop(stack)) {	\
									i = commandsArray [i + 1] - 1;		   				\
								}														\
								break;												

		switch (save)  {
			
			#include "cmd.h"						
			#undef DEF_CMD
			#undef DEF_JMP

			default:
				break;
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


void detectPush (int * commandsArray, int * ip, Register * registers, Stack * stack, int * ramElements) {

		if ((commandsArray [ * ip] & TURNOFFMASKIMMED) == CMD_push) {

			( * ip)++;
			StackPush (stack, commandsArray [ * ip]);
			return;
		}

		if ((commandsArray [ * ip] & TURNOFFMASKREGISTER) == CMD_push) {

			( * ip)++;
			StackPush (stack, (registers + commandsArray [ * ip])->equationRegister);
			return;
		}

		if (((commandsArray [ * ip] & TURNOFFMASKRAM) & TURNOFFMASKIMMED) == CMD_push) {

			( * ip)++;
			StackPush (stack, ramElements [commandsArray [ * ip]]);
			return;
		}

		if (((commandsArray [ * ip] & TURNOFFMASKRAM) & TURNOFFMASKREGISTER) == CMD_push) {

			( * ip)++;
			StackPush (stack, ramElements [(registers + commandsArray [ * ip])->equationRegister]);
			return;
		}

		if ((commandsArray [ * ip] & TURNOFFMASKPOP) == CMD_pop) {

			StackPop (stack);
			return;
		}

		if ((commandsArray [ * ip] & TURNOFFMASKREGISTER) == CMD_pop) {

			( * ip)++;
			(registers + commandsArray [ * ip])->equationRegister = StackPop (stack);
			return;
		}

		if (((commandsArray [ * ip] & TURNOFFMASKRAM) & TURNOFFMASKIMMED) == CMD_pop) {

			( * ip)++;
			ramElements [commandsArray [ * ip]] = StackPop (stack);
			return;
		}	

		if (((commandsArray [ * ip] & TURNOFFMASKRAM) & TURNOFFMASKIMMED) == CMD_pop) {

			( * ip)++;
			ramElements [(registers + commandsArray [ * ip])->equationRegister] = StackPop (stack);
			return;
		}

		if (commandsArray [ * ip] == CMD_in) {

			( * ip)++;
			StackPush (stack, commandsArray [ * ip]);
			return;
		}
}


unsigned long FileSize (FILE * compfile) {

    struct stat buf = {};
    if (fstat (fileno (compfile), &buf) == 0)
        return buf.st_size;

    return 0;
}


bool theEndJBE (Register * registers, Stack * stack, int * commandsArray, int * i) {

	int a = registers->equationRegister, b = StackPop (stack);

	if (a <= b) {

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