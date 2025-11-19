#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>

#define INSTR_LEN_LIMIT 6

/* 
	VirtNanoProUni
	- A virtual half-byte sized processing unit that counts a whopping 12 instructions.
	- Written in C programming language
	========================
	Virtual Nano Processing Unit specifications
	- Limited to c.ca 1 Hz or 1 Instruction/Second
	- 2 Registers, Limited to Integers and simple mathematical operations
	  and comparisons - Both (AX, BX) have 4 bits of decimal memory - Though
	  every non-binary assignment operation will result in an instant HALT
	  of the entire system (VNPU)
	- No Random Access Memory, limited to a bi-dimensional array of 4+4 bits
	- No programmable interface, limited to simple instruction calls
	  as '+ 1 1' then '@' which will output '2'
	  An important sidenote that must be made is that after a call like the
	  aforementioned is made to the NPU, the result of said operation will be
	  temporarily stored into the first non-occupied slot of virtual memory
	  (the bi-dimensional array of 4+4 bits) in binary
	========================
	VNPU Instruction Set (v'NIS)
	-----REGISTERS------
	'A': Register AX
	'B': Register BX
	-----OPERATIONS-----
	'+': Adds X by Y. (Example: '+ A B' adds register AX and BX)
	'-': Subtracts X by Y
	'*': Multiplies X by Y
	'/': Divides X by Y (Note: WILL halt if a division by 0 operation is attempted)
	-----DATA/MOVEMENT--
	'M': Almost 1:1 virtual MOV instruction (Example: 'M 5 A' moves 0101 into register AX)
	-----COMPARISON-----
	'?': Compares X to Y (Example: '? A B')
	'>': X GREATER THAN Y CHECK expression
	'<': X LESSER THAN Y CHECK expression
	'!': X NOT EQUAL TO Y CHECK expression
	------CONTROL-------
	'@': Prints X value (Example: '@ A' will print the contents of register AX)
	'.': Halts immediately
*/

// w ( int millisec )
// ⤷ Syntax sugar / Wrapper for usleep() ( from <unistd.h> )
void w(int millisec);

// FindInstruction ( char InstrBuff[] )
// ⤷ Simple helper to check if the user's input contains a valid
//   call to an instruction or not and returns a value.
// NOTE: MISLEADING FUNCTION NAME. VERY MISLEADING.
char FindInstruction(char InstrBuff[]);

// dec2bin ( int DEC_VAL )
// ⤷ Converts a decimal value to binary and returns it
int dec2bin(int DEC_VAL);

// memBin2dec ()
// ⤷ Converts a memory binary to decimal and returns it
int memBin2dec();

// dec2bin2reg ( int DEC_VAL, int reg[4] )
// ⤷ Same as the last function but instead of returning the
//    converted value it stores it into a specific register ( which is: (int)[4] )
void dec2bin2reg(int DEC_VAL, int reg[4]);

// dec2bin2mem ( int DEC_VAL )
// ⤷ Same as the last function but for memory ( which is: (int)[2][4] )
void dec2bin2mem(int DEC_VAL);

// HandleSignalInterrupt ( int sig )
// ⤷ For the signal() call in main()
void SigIntHandler(int sig);

// HandleInstruction ( char instr, 
//                     char com1,
//                     char com2
//                   )
// ⤷ This helper is the one that actually does the calls to the virtual instructions
bool HandleInstruction(char instr, char com1, char com2);

// VIRTUAL INSTRUCTIONS
int AddInstruction(char com1, char com2);
int SubInstruction(char com1, char com2);
int MulInstruction(char com1, char com2);
int DivInstruction(char com1, char com2);
//
int MovInstruction(char com1, char com2);
//
bool CmpInstruction(char com1, char com2);
bool GrThInstruction(char com1, char com2);
bool LsThInstruction(char com1, char com2);
bool NotEqInstruction(char com1, char com2);

void PrntInstruction(char com1);
void HaltInstruction();

// GLOBAL STATE VARIABLES
//
bool HALT = false; // 'false' for ! halted; 'true' for halted
bool log_flag = false;

char EnableLogBuffer = 'n';
char InstructionBuffer[INSTR_LEN_LIMIT] = "\0";
char instr;
char com1;
char com2;

int MEM[2][4] =	
{
	{0,0,0,0},
	{0,0,0,0}
};
int AX[4] = {0,0,0,0};
int BX[4] = {0,0,0,0};

int main(void)
{
	w(337);
	//
	signal(SIGINT, SigIntHandler);

	//
	printf("VNPU => Initialization finished.\n");
	w(337);
	printf("VNPU => Enable logging to console? (y/N)\n: ");
	scanf("%c", &EnableLogBuffer);

	if (EnableLogBuffer == 'y' || EnableLogBuffer == 'Y')
	{
		w(1000);
		log_flag = true;
	}

	//
	if (log_flag) printf("VNPU => Entered phase 1 of runtime.\n");
	while (HALT == false)
	{
		//
		w(337);
		if (log_flag) printf("VNPU => Waiting for instructions.\n");
		w(1000);
		printf("> ");
		// Read instruction (F)
		scanf("%s", InstructionBuffer);
		
		char InstructionFound = FindInstruction(InstructionBuffer);
		if (InstructionFound == 'e')
		{
			w(1000);
			if (log_flag) printf("VNPU => ERROR: An illegal instruction was provided.\n\t=> Halting.\n");
			HALT = true;
		}

		//
		if (log_flag) printf("STATUS => Entered phase 2 of runtime\n");
		w(1000);
		if (log_flag) printf("VNPU => Checking instruction buffer string ..\n");

		//
		if (!isspace(InstructionBuffer[2]) && !isspace(InstructionBuffer[4]))
		{
			w(1000);
			if (log_flag) printf("VNPU => ERROR: An illegal instruction was provided.\nHalting.\n");
			HALT = true;
		}
		if (log_flag) printf("VNPU => First sanity check passed\n");
		//
		if (!isspace(InstructionBuffer[1]) && !isspace(InstructionBuffer[3]))
		{
			instr = InstructionBuffer[1];
			com1  = InstructionBuffer[3];
			if (instr == '@')
			{
				if (log_flag) printf("VNPU => Conditions met. Running PrntInstruction()\n");
				PrntInstruction(com1);
			}
		}
		if (log_flag) printf("VNPU => First edge-case for instructions passed\n");
		//
		if (!isspace(InstructionBuffer[1]) && isspace(InstructionBuffer[2]) &&
            isspace(InstructionBuffer[3]) && isspace(InstructionBuffer[4]) && isspace(InstructionBuffer[5]) )
		{
			instr = InstructionBuffer[1];
			if (instr == '.')
			{
				if (log_flag) printf("VNPU => Conditions met. Running HaltInstruction()\n");
				HaltInstruction();
			}
		}
		if (log_flag) printf("VNPU => Last edge-case for instructions passed\n");
		w(1000);

		//
		if (log_flag) printf("VNPU => All edge-cases passed successfully\n");

		//
		w(1000);
		if (log_flag) printf("VNPU => Running last sanity check\n");
		
		//
		if (log_flag) printf("Sanity check passed\nEntered phase 3 of runtime\n");

		//
		w(1000);
		instr = InstructionBuffer[1];
		com1  = InstructionBuffer[3];
		com2  = InstructionBuffer[5];

		w(1000);
		if (HandleInstruction(instr, com1, com2) == false)
		{
			w(1000);
			printf("VNPU => ERROR: An illegal instruction was provided.\n");
			HALT = true;
		}

	}

	w(1000);
	if (log_flag) printf("VNPU => Exiting with code 0\n");
	// TODO: Clean up if dynamic memory was allocated
	return 0; // Technically frees memory too..? Or maybe just not dynamic one I don't know :o
}

//
void w(int millisec)
{
    int microsec = millisec * 1000;
	usleep(microsec);
}

// Misleading function name
char FindInstruction(char InstrBuff[])
{
    switch (InstrBuff[1])
    {
        case '+': case '-': case '*':
        case '/': case 'M': case '?':
        case '>': case '<': case '!':
            return '0';
        default: 
            return 'e';
    }
}

int dec2bin(int DEC_VAL)
{
	int bin_val = 0;
	int multiplier = 1;
	int number = DEC_VAL;
    int remainder = 0;
	while (DEC_VAL > 0)
	{
		remainder = number % 2;
		bin_val = (bin_val + (remainder * multiplier));
		multiplier = (multiplier * 10);
		number = (number / 2);
	}
	return bin_val;
}

int bin2dec(int reg[4])
{
	int value = 0;
	for (int i = 1; i < 4; ++i)
	{
		value = value * 2 + reg[i];
	}
	return value;
}


int memBin2dec() {
    int val = 0;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            val = val * 2 + MEM[i][j];
        }
    }
    return val;
}

void dec2bin2reg(int DEC_VAL, int reg[4])
{
    for (int i = 0; i < 4; ++i)
    {
        reg[3-i] = DEC_VAL % 2;  // LSB goes to last index
        DEC_VAL = DEC_VAL / 2;
    }
}

void dec2bin2mem(int DEC_VAL) {
    for (int i = 1; i >= 0; --i)
    {
        for (int j = 3; j >= 0; --j) {
            MEM[i][j] = DEC_VAL % 2;
            DEC_VAL /= 2;
        }
    }
}

void SigIntHandler(int sig)
{
    sig = sig; // just to compile with -Wall -Wextra -pedantic with 0 warnings.
               // maybe i am wrong but signal() requires the handler
               // function to have 'int sig' - afaik, obviously.
	printf("VNPU => SIGINT intercepted.\n\t=> Exit? (y|Y[e|E[s|S]]/n|N[o|O]) ");
    char ret[8];
    scanf("%s", ret);
    if (strcmp(ret, "Y") == 0 || strcmp(ret, "ye") == 0 ||
        strcmp(ret, "yES") == 0 || strcmp(ret, "y") == 0 ||
        strcmp(ret, "YES") == 0 || strcmp(ret, "yEs") == 0 ||
        strcmp(ret, "YE") == 0 || strcmp(ret, "YEs") == 0 ||
        strcmp(ret, "yeS") == 0 || strcmp(ret, "Ye") == 0 ||
        strcmp(ret, "YeS") == 0 || strcmp(ret, "yes") == 0 ||
        strcmp(ret, "yE") == 0 || strcmp(ret, "Yes") == 0
    ) {
        HALT = true;
        exit(0);
    }
    else if (strcmp(ret, "n") == 0 || strcmp(ret, "no") == 0 ||
               strcmp(ret, "N") == 0 || strcmp(ret, "No") == 0 ||
               strcmp(ret, "NO") == 0 || strcmp(ret, "nO") == 0
    ) {
        printf("VNPU => Continuing execution.\n");  
    } else
    {
        printf("VNPU => Unknown option entered: \"%s\"\n", ret);
        SigIntHandler(0);
    }
}

bool HandleInstruction(char instr, char com1, char com2)
{
	int code = 1;
	if (instr == '+')
    {
		int code = AddInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '-')
	{
		int code = SubInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '*')
	{
		MulInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '/')
	{
		DivInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == 'M')
	{
		MovInstruction(com1, com2);
		if (code != 0) return false;	
		else return true;
	}
	else if (instr == '?')
	{
		CmpInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	} 
	else if (instr == '>')
	{
		GrThInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '<')
	{
		LsThInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '!')
	{
		NotEqInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	} return false;
}

//
int AddInstruction(char com1, char com2)
{
	w(1000);
	int result = com1 + com2;
	dec2bin2mem(result);
	return result;
}
int SubInstruction(char com1, char com2)
{
	w(1000);
	int result = com1 - com2;
	dec2bin2mem(result);
	return result;
}
int MulInstruction(char com1, char com2)
{
	w(1000);
	int result = com1 * com2;
	dec2bin2mem(result);
	return result;
}
int DivInstruction(char com1, char com2)
{
	w(1000);
	if (com2 != 0)
	{
		int result = com1 / com2;
		dec2bin2mem(result);
		return result;
	}
	else
	{
		// commented out because is implemented in main()
		// here just for reference

		// if (log_flag) printf("VNPU => A division by zero was attempted\nHalting.\n");
		// HALT = true;
		return 2;
	}
}
int MovInstruction(char com1, char com2)
{
	w(1000);
	if (com1 == 'A')
	{
		// case: move AX into BX, though..
		if (com2 != 'B') // ..com2 **MUST** be equal to another register which differs from AX (so BX)
		{				 //   otherwise there would be no place to move our binary data
			return 1;
		}
		// dec2bin2reg() IS A DESTRUCTIVE FUNCTION!!!
		// That's why we label cases so there is *NO* confusion.
		// (And data is safe at runtime)
		
		// Convert binary value that is stored into AX to a decimal integer variable
		// THEN! convert that decimal integer variable to binary once again and drop it into BX
		int ax_dec = bin2dec(AX);
		dec2bin2reg(ax_dec, BX);
		return 0;
	}
	else if (com1 == 'B')
	{
		// case: move BX into AX, same as last case.
		w(1000);
		if (com1 != 'A')
		{
			return 1;
		}
		// Same warning as before applies here too
		int bx_dec = bin2dec(BX);
		dec2bin2reg(bx_dec, AX);
		return 0;
	}
	else
	{
		// case: move <VAL> into <REG>, of which there are two possible cases.
		//       but before, we gotta do some sanity checks:
		
		// sanity check no.1
		if (!isdigit(com1))
		{
			return 1;
		}
		// sanity check no.2
		if (com1 > 9 || com1 < 0)
		{
			return 1;
		}
		// sanity check no.3
		if (com2 != 'A' && com2 != 'B')
		{
			return 1;
		}

		// sanity checks passed,
		// now we kick some values into some regz
		if (com2 == 'A')
		{
			dec2bin2reg(com1, AX);
		}
		else if (com2 == 'B')
		{
			dec2bin2reg(com1, BX);
		}
		// and that was... the MOV instruction with a lot of limits and sanity checks!
	}
	return 1;
}
//
bool CmpInstruction(char com1, char com2)
{
	// just check if com1 == com2 (?)
	if (com1 == com2)
		return true;
	else return false;
}
bool GrThInstruction(char com1, char com2)
{
	// same
	if (com1 > com2)
		return true;
	else return false;
}
bool LsThInstruction(char com1, char com2)
{
	if (com1 < com2)
		return true;
	else return false;
}
bool NotEqInstruction(char com1, char com2)
{
	if (com1 != com2)
		return true;
	else return false;
}
//
void PrntInstruction(char com1)
{
	printf("%c", com1);
}
void HaltInstruction()
{
	HALT = true;
}
