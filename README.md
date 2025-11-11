
# VirtNanoProUni
A virtual half-byte sized processing unit that counts a whopping 12 instructions. \
Written in tha C programming language

## Virtual Nano Processing Unit specifications
- Limited to c.ca 1 Hz or 1 Instruction/Second
- 2 Registers, Limited to Integers and simple mathematical operations \
  and comparisons - Both (AX, BX) have 4 bits of decimal memory - Though \
  every non-binary assignment operation will result in an instant HALT \
  of the entire system (VNPU) \
- No Random Access Memory, limited to a bi-dimensional array of 4+4 bits \
- No programmable interface, limited to simple instruction calls \
  as `+ 1 1` then `@` which will output `2` \
  An important sidenote that must be made is that after a call like the \
  aforementioned is made to the NPU, the result of said operation will be \
  temporarily stored into the first non-occupied slot of virtual memory \
  (the bi-dimensional array of 4+4 bits) in binary

## VNPU Instruction Set (v'NIS)
### REGISTERS
`A`: Register AX \
`B`: Register BX
### OPERATIONS
`+`: Adds X by Y. (Example: `+ A B` adds register AX and BX) \
`-`: Subtracts X by Y \
`*`: Multiplies X by Y \
`/`: Divides X by Y (Note: WILL halt if a division by 0 operation is attempted) \
### DATA/MOVEMENT
`M`: Almost 1:1 virtual MOV instruction (Example: `M 5 A` moves 0101 into register AX)
### COMPARISON
`?`: Compares X to Y (Example: `? A B`) \
`>`: X GREATER THAN Y CHECK expression \
`<`: X LESSER THAN Y CHECK expression \
`!`: X NOT EQUAL TO Y CHECK expression
### CONTROL
`@`: Prints X value (Example: `@ A` will print the contents of register AX) \
`.`: Halts immediately

<!--
// w ( int sec )
// ⤷ Syntax sugar / Wrapper for sleep() ( from <unistd.h> )

// FindInstruction ( char InstrBuff[] )
// ⤷ Simple helper to check if the user's input contains a valid
//   call to an instruction or not and returns a value.
// NOTE: MISLEADING FUNCTION NAME. VERY MISLEADING.

// dec2bin ( int DEC_VAL )
// ⤷ Converts a decimal value to binary and returns it

// memBin2dec ( int mem[2][4] )
// ⤷ Converts a memory binary to decimal and returns it

// dec2bin2reg ( int DEC_VAL, int reg[4] )
// ⤷ Same as the last function but instead of returning the
//    converted value it stores it into a specific register ( which is: (int)[4] )

// dec2bin2mem ( int DEC_VAL, int mem[2][4] )
// ⤷ Same as the last function but for memory ( which is: (int)[2][4] )

// HandleSignalInterrupt ( int sig )
// ⤷ For the signal() call in main()

// HandleInstruction ( char instr, 
//                     char com1, char com2
//					   int AX, int BX)
// ⤷ This helper is the one that actually does the calls to the virtual instructions
-->
