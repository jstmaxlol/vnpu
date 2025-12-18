#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#define INSTR_LEN_LIMIT 7
#define VNPU_WORD_SIZE 32
void w(int millisec);
char FindInstruction(char InstrBuff[]);
int dec2bin(int DEC_VAL);
int memBin2dec();
void dec2bin2reg(int reg[VNPU_WORD_SIZE], int DEC_VAL);
void dec2bin2mem(int DEC_VAL);
void SigIntHandler(int sig);
bool HandleInstruction(char instr, char com1, char com2);
int AddInstruction(char com1, char com2);
int SubInstruction(char com1, char com2);
int MulInstruction(char com1, char com2);
int DivInstruction(char com1, char com2);
int MovInstruction(char com1, char com2);
bool CmpInstruction(char com1, char com2);
bool GrThInstruction(char com1, char com2);
bool LsThInstruction(char com1, char com2);
bool NotEqInstruction(char com1, char com2);
void PrntInstruction(char com1);
void HaltInstruction();
void printUsage();
int ResolveOperand(char c);
bool HALT = false; 
bool log_flag = false;
char EnableLogBuffer = 'n';
char InstructionBuffer[INSTR_LEN_LIMIT];
char instr;
char com1;
char com2;
int MEM[2][VNPU_WORD_SIZE] = 
{
	{0},
	{0}
};
int AX[VNPU_WORD_SIZE] = {0};
int BX[VNPU_WORD_SIZE] = {0};
int main(void)
{
    w(337);
    signal(SIGINT, SigIntHandler);
    printf("VNPU => Initialization finished.\n");
    w(337);
    printf("VNPU => Enable logging to console? (y/N)\n: ");
    scanf(" %c", &EnableLogBuffer);
    if (EnableLogBuffer == 'y' || EnableLogBuffer == 'Y')
        log_flag = true;
    if (log_flag)
        printf("VNPU => Entered phase 1 of runtime.\n");
    while (!HALT)
    {
        if (log_flag)
            printf("VNPU => Waiting for instructions.\n");
        printf("> ");
        if (!fgets(InstructionBuffer, INSTR_LEN_LIMIT, stdin))
            break;
        if (InstructionBuffer[0] == '\n' || InstructionBuffer[0] == '\0') continue;
        char InstructionFound = FindInstruction(InstructionBuffer);
        if (InstructionFound == 'e')
        {
            printf("VNPU => ERROR: An illegal instruction was provided.\n");
            HALT = true;
            break;
        }
        
        if (InstructionBuffer[0] != '\0' &&
            (InstructionBuffer[1] == '\n' || InstructionBuffer[1] == '\0'))
        {
            instr = InstructionBuffer[0];
            if (instr == '.')
            {
                HaltInstruction();
                break;
            }
            else if (instr == 'H')
            {
                printUsage();
                continue;
            }
        }
        
        instr = InstructionBuffer[0];
        com1  = InstructionBuffer[2];
        com2  = InstructionBuffer[4];
        if (HandleInstruction(instr, com1, com2) == false)
        {
            printf("VNPU => ERROR: An illegal instruction was provided.\n");
            HALT = true;
            break;
        }
    }
    if (log_flag)
        printf("VNPU => Exiting with code 0\n");
    return 0;
}
void w(int millisec)
{
    int microsec = millisec * 1000;
	usleep(microsec);
}
char FindInstruction(char InstrBuff[])
{
    switch (InstrBuff[0])
    {
        case '+': case '-': case '*':
        case '/': case 'M': case '?':
        case '>': case '<': case '!':
        case '@': case '.': case 'H':
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
    while (number > 0)
    {
        remainder = number % 2;
        bin_val += remainder * multiplier;
        multiplier *= 10;
        number /= 2;
    }
    return bin_val;
}
int bin2dec(int reg[VNPU_WORD_SIZE])
{
    int value = 0;
    for (int i = 0; i < VNPU_WORD_SIZE; ++i)
    {
        value = value * 2 + reg[i];
    }
    return value;
}
int memBin2dec(void)
{
    int val = 0;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < VNPU_WORD_SIZE; ++j)
        {
            val = (val << 1) | MEM[i][j];
        }
    }
    return val;
}
void dec2bin2reg(int reg[VNPU_WORD_SIZE], int DEC_VAL) 
{
    for (int i = VNPU_WORD_SIZE - 1; i >= 0; --i)
    {
        reg[i] = DEC_VAL & 1;
        DEC_VAL >>= 1;
    }
}
void dec2bin2mem(int DEC_VAL) {
    for (int i = 1; i >= 0; --i)
    {
        for (int j = VNPU_WORD_SIZE - 1; j >= 0; --j) {
            MEM[i][j] = DEC_VAL & 1;
            DEC_VAL >>= 1;
        }
    }
}
void SigIntHandler(int sig)
{
    sig = sig; 
               
               
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
int ResolveOperand(char c)
{
    if (isdigit((unsigned char)c))
        return c - '0';
    if (c == 'A')
        return bin2dec(AX);
    if (c == 'B')
        return bin2dec(BX);
    return -1; 
}
bool HandleInstruction(char instr, char com1, char com2)
{
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
		int code =MulInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '/')
	{
		int code = DivInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == 'M')
	{
		int code = MovInstruction(com1, com2);
		if (code != 0) return false;	
		else return true;
	}
	else if (instr == '?')
	{
		int code = CmpInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	} 
	else if (instr == '>')
	{
		int code = GrThInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '<')
	{
		int code = LsThInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
	else if (instr == '!')
	{
		int code = NotEqInstruction(com1, com2);
		if (code != 0) return false;
		else return true;
	}
    else if (instr == '@')
    {
        PrntInstruction(com1);
        return true;
    }
    else if (instr == 'H')
    {      
        printUsage();
        return true;
    }
    return false;
}
int AddInstruction(char com1, char com2)
{
    w(337);
    int v1 = ResolveOperand(com1);
    int v2 = ResolveOperand(com2);
    if (v1 < 0 || v2 < 0) return 1;
    int result = v1 + v2;
    dec2bin2reg(AX, result);
    dec2bin2mem(result);
    return 0;
}
int SubInstruction(char com1, char com2)
{
    w(337);
    int v1 = ResolveOperand(com1);
    int v2 = ResolveOperand(com2);
    if (v1 < 0 || v2 < 0) return 1;
    int result = v1 - v2;
    dec2bin2reg(AX, result);
    dec2bin2mem(result);
    return 0;
}
int MulInstruction(char com1, char com2)
{
    w(337);
    int v1 = ResolveOperand(com1);
    int v2 = ResolveOperand(com2);
    if (v1 < 0 || v2 < 0) return 1;
    int result = v1 * v2;
    dec2bin2reg(AX, result);
    dec2bin2mem(result);
    return 0;
}
int DivInstruction(char com1, char com2)
{
	w(337);
    int v1 = ResolveOperand(com1);
    int v2 = ResolveOperand(com2);
    if (v1 < 0 || v2 <= 0) return 1;
    int result = v1 / v2;
    dec2bin2reg(AX, result);
    dec2bin2mem(result);
    return 0;
}
int MovInstruction(char com1, char com2)
{
    w(337);
    
    if (com1 == 'A' && com2 == 'B') {
        int ax_dec = bin2dec(AX);
        dec2bin2reg(BX, ax_dec);
        return 0;
    }
    if (com1 == 'B' && com2 == 'A') {
        int bx_dec = bin2dec(BX);
        dec2bin2reg(AX, bx_dec);
        return 0;
    }
    
    if (com1 < '0' || com1 > '9') return 1;
    if (com2 != 'A' && com2 != 'B') return 1;
    int val = com1 - '0';
    if (com2 == 'A')
        dec2bin2reg(AX, val);
    else
        dec2bin2reg(BX, val);
    return 0;
}
bool CmpInstruction(char com1, char com2)
{
	
	if (com1 == com2)
		return true;
	else return false;
}
bool GrThInstruction(char com1, char com2)
{
	
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
void PrntInstruction(char com1)
{
    if (com1 == 'A')
    {
        int num = bin2dec(AX);
        printf("%d\n", num);
    }
    else if (com1 == 'B')
    {
        int num = bin2dec(BX);
        printf("%d\n", num);
    }
    else
    {
	    printf("%c\n", com1);
    }
}
void HaltInstruction()
{
	HALT = true;
}
void printUsage()
{
    printf
    (
        "========================\n"
        "VNPU Instruction Set (v'NIS)\n"
        "-----REGISTERS------\n"
        "'A': Register AX\n"
        "'B': Register BX\n"
        "-----OPERATIONS-----\n"
        "'+': Adds X by Y. (Example: '+ A B' adds register AX and BX)\n"
        "'-': Subtracts X by Y\n"
        "'*': Multiplies X by Y\n"
        "'/': Divides X by Y (Note: WILL halt if a division by 0 operation is attempted)\n"
        "-----DATA/MOVEMENT--\n"
        "'M': Almost 1:1 virtual MOV instruction (Example: 'M 5 A' moves 0101 into register AX)\n"
        "-----COMPARISON-----\n"
        "'?': Compares X to Y (Example: '? A B')\n"
        "'>': X GREATER THAN Y CHECK expression\n"
        "'<': X LESSER THAN Y CHECK expression\n"
        "'!': X NOT EQUAL TO Y CHECK expression\n"
        "------CONTROL-------\n"
        "'@': Prints X value (Example: '@ A' will print the contents of register AX)\n"
        "'.': Halts immediately\n"
        "'H': Used to print this IS.\n"
    );
}
