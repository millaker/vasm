# HW5 instruction set specs

## Instruction format
32 bit :
|32-28|27-24|23-12|11-0|
|-----|-----|-----|----|
|OPcode|Condition code(4 bits) or srctype(27), dsttype(26)|source  or count(for shift)|destination|

*srctype 0 = reg (mem) 1 = imm
*dsttype 0 = reg (mem)

|Name| Short| Opcode| Format(inst dst, src)|Description|
|----|------|-------|----------------------|-----------|
|NOP| NOP| 0|NOP|No operation|
|Branch|BRA| 1| BRA mem, cc|Branch to destination memory location if the condition code is set(cc)|
|Load|LD|2|LD reg, mem(imm)|Load value from memory and store in specified register|
|Store|STR|3|STR mem, reg|Store value from register to specified memory locaiton|
|Add| ADD|4|ADD reg, src|Add value from src and add to destination register|
|Multiply|MUL|5|MUL reg, src|Multiply value from src with destination register|
|Complement| CMP|6|CMP reg, src|Complement value from source and store in destination register|
|Arithmetic right shift|SHRA|7|SHRA reg, cnt|Shift value right arithmetically in reg `cnt` times|
|Left shift|SHL|8|SHL reg, cnt|Shift value left in reg `cnt` times|
|Logical Right shift|SHR|9|SHR reg, cnt|Shift value right logically in reg `cnt` times|
|And|AND|10|AND reg, src|And value from src with destination register|
|Or|OR|11|OR reg, src|Or value from src with destination register|
|Exclusive or|XOR|12|XOR reg, src|XOR value from src with destination register|
|Halt|HLT|13|HLT| Halt the machine|
|Sub|SUB|14|SUB reg, src| Sub src from destination and store in destination|

*cnt must be less than 32
*Identifiers(Labels) must be all upper cases
*Hex value are all lower cases

## Condition code
|Condition|Short|Machine code|
|---------|-----|------------|
|Always|ALW|0000|
|Carry|CCC|0001|
|Even|CCE|0010|
|Parity|CCP|0011|
|Zero|CCZ|0100|
|Negative|CCN|0101|

## Instruction syntax
Labels: Upper Case letters and numbers, starting with a letter
Hex Value: lower case letters
Label must be followed by an instruction or else the assembler will invoke an error.
