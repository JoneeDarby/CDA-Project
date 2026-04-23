#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0: // add
            *ALUresult = A + B;
            break;
        case 1: // subtract
            *ALUresult = A - B;
            break;
        case 2: // slt
            *ALUresult = (A < B) ? 1 : 0;
            break;
        case 3: // sltu
            *ALUresult = ((unsigned)A < (unsigned)B) ? 1 : 0;
            break;
        case 4: // and
            *ALUresult = A && B;
            break;
        case 5: // or
            *ALUresult = A | B;
            break;
        case 6: // lui
            *ALUresult = B << 16; // Shift B left by 16 bits to load it into the upper half of the register
            break;
        case 7: // not
            *ALUresult = ~A;
            break;

    /* Set Zero flag */
    *Zero = (*ALUresult == 0) ? 1 : 0; // Set Zero flag if ALU result is zero, otherwise clear it
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if (PC % 4 != 0 || PC / 4 >= (65536 >> 2)) {
        return 1; // PC is not word-aligned
    }

    *instruction = Mem[PC / 4]; // Fetch the instruction from memory

    return 0; // Works
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    /*1. Partition instruction into several parts (op, r1, r2, r3, funct, offset, jsec).
    2. Read line 41 to 47 of spimcore.c for more information.*/

    /*op: need to right shift instruction by 26 to get the correct bits to the
    LSB and then AND it by 6 bits so there's no trailing 1's to get the
    correct op*/
    *op = (instruction >> 26) & 0x3F; //right shift by 26

    /*r1,r2,r3: the register values are all going to be AND'ed by 5 bits to get
    rid of trailing 1's. Their shift values all vary bc of their bit locations
    differ from one another*/
    *r1 = (instruction >> 21) & 0x1F; //right shift by 21
    *r2 = (instruction >> 16) & 0x1F; //right shift by 16
    *r3 = (instruction >> 11) & 0x1F; //right shify by 11

    /*funct,offset,jsec: since they all go to the 0(LSB) there is not shifting
    needed so the only thing that varies are the AND values*/
    *funct = instruction & 0x3F; //6 bit
    *offset = instruction & 0xFFFF; //16 bit
    *jsec = instruction & 0x3FFFFFF; //26 bit
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    /*Decode the operation code and set the control signals accordingly*/
    
    switch(op) {
        case 0x00: // R-Type
            controls->RegDst = 1;   // Does the instruction write to the rd register?
            controls->Jump = 0;     // Is the instruction a jump instruction?
            controls->Branch = 0;   // Is the instruction a branch instruction?
            controls->MemRead = 0;  // Does the instruction read from memory?
            controls->MemtoReg = 0; // Does the instruction write the memory data to the register?
            controls->ALUOp = 7;    // ALU operation code
            controls->MemWrite = 0; // Does the instruction write to memory?
            controls->ALUSrc = 0;   // Does the instruction use an immediate value as the second ALU operand?
            controls->RegWrite = 1; // Does the instruction write to a register?
            break;
        
        case 0x02: // J-Type (j)
            controls->RegDst = 2;
            controls->Jump = 1;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 0;
            break;
        
        case 0x04: // I-Type (beq)
            controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 1;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 0;
            break;
        
        case 0x08: // I-Type (addi)
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
        
        case 0x0a: // I-Type (slti)
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 2;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        case 0x0b: // I-Type (sltiu)
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 3;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        case 0x0f: // I-Type (lui)
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 6;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        case 0x23: // I-Type (lw)
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        case 0x2b: // I-Type (sw)
            controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0;
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            break;
        
        default: // Unsupported instruction
            return 1;
    }
    return 0; // Works
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    /*Read the registers addressed by r1 and r2 from Reg, and 
    write the read values to data1 and data2 respectively.*/

    /*Check if the data in r1/r2 is 0 and if it is set data1/data2 to 0, since 0 in the register is 0 constant.
    If r1/r2 are not 0 go to that index in the register and place the info in data1/data2.*/
    if (r1 == 0) { 
        *data1 = 0;
    } else {
        *data1 = Reg[r1];
    }

    if (r2 == 0) {
        *data2 = 0;
    } else {
        *data2 = Reg[r2];
    }
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //1. Assign the sign-extended value of offset to extended_value.

    /*offset is a 16 bit right now so I need to check if the 16th bit is 0 or 1.
    If its 1 then I need to make the upper 16 bits 1's as well to make it 32 bits
    and keep the negative number. If it's 0 then I can keep the upper 16 bits as
    0's to keep the positive number*/

    if ((offset & 0x8000) != 0) {
        *extended_value = offset | 0xFFFF0000; //If its 1 make all the 16 upper bits 1 (negative)
    } else {
        *extended_value = offset; //If its 0 make all the 16 upper bits 0 (positive)
    }

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    unsigned B;
    /*1. Determine the second ALU operand based on ALUSrc from instruction_decode. If ALUSrc is 0, the second ALU operand is data2. If ALUSrc is 1, the second ALU operand is extended_value.*/
    if (ALUSrc == 0) {
        B = data2;
    } else {
        B = extended_value;
    }

    /*2. If ALUOp is not 7, go straight to ALU func*/
    if (ALUOp != 7) {
        ALU(data1, B, ALUOp, ALUresult, Zero);
        return 0;
    }
    
    /*3. If ALUOp is 7, determine the ALU operation based on funct.*/
    if (ALUOp == 7) {
        switch (funct) {
            case 0x20: // add
                ALU(data1, B, 0, ALUresult, Zero);
                break;
            case 0x22: // sub
                ALU(data1, B, 1, ALUresult, Zero);
                break;
            case 0x24: // and
                ALU(data1, B, 4, ALUresult, Zero);
                break;
            case 0x25: // or
                ALU(data1, B, 5, ALUresult, Zero);
                break;
            case 0x2a: // slt
                ALU(data1, B, 2, ALUresult, Zero);
                break;
            case 0x2b: // sltu
                ALU(data1, B, 3, ALUresult, Zero);
                break;
            default:
                return 1; // Unsupported operation
        }
    }
    return 0; // Works
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    /*1. Use the value of MemWrite or MemRead to determine if a memory write
    operation or memory read operation or neither is occurring.
    2. If reading from memory, read the content of the memory location addressed by
    ALUresult to memdata.
    3. If writing to memory, write the value of data2 to the memory location
    addressed by ALUresult.
    4. Return 1 if a halt condition occurs; otherwise, return 0.

    Mem is the memory array
    If MemWrite = 1, check word alignment and write into memory
    If MemRead = 1, check word alignment and read from memory*/

    /*I need to check if we are reading or writing to memory. IF we are
    then I need to see if the address(AlUresult is aligned). If not give
    a halt (return 1)*/
    if (MemRead == 1 || MemWrite == 1) { //we are accessing memory
        if (ALUresult % 4 != 0) { //check if the address (ALUresult) is not aligned
            return 1; //give a halt
        }
    }

    /*For MemWrite: If it is equal to 1 then I need to get the correct location
    of ALUresult and make this data equal to data2*/
    /*For MemRead: If it is equal to 1 then I need to get the data that is 
    addressed by AlUresult into memdata*/
    if (MemWrite == 1) {
        Mem[ALUresult >> 2] = data2;
    } else if (MemRead == 1) {
        *memdata = Mem[ALUresult >> 2];
    } 
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
   unsigned dest;
   unsigned value;

    /* If we are not supposed to write, just return */
    if (RegWrite != 1)
        return;

    /* Choose destination register: r2 or r3 */
    if (RegDst == 1)
        dest = r3;      /* R-type: rd */
    else
        dest = r2;      /* I-type/lw: rt */

    /* Choose value to write: from memory or ALU */
    if (MemtoReg == 1)
        value = memdata;    /* lw */
    else
        value = ALUresult;  /* R-type, addi, etc. */

    /* In MIPS, register 0 is always 0; do not overwrite it */
    if (dest != 0)
        Reg[dest] = value;
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    unsigned nextPC = *PC + 4; // Default case : next PC is the next instruction

    /* If it's a jump instruction, we need to calculate the jump target address */
    if (Jump) {
        nextPC = (nextPC & 0xF0000000) | (jsec << 2); // Combine upper 4 bits of nextPC with jsec shifted left by 2 to convert from word to byte address
    }

    /* If it's a branch instruction and the Zero flag is set, we need to calculate the branch target address */
    if (Branch && Zero) {
        nextPC = nextPC + (extended_value << 2); // Branch target address is calculated by adding the sign-extended offset (shifted left by 2 to convert from word to byte address) to the nextPC
    }   
}

