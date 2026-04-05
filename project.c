#include "spimcore.h"
#include "spimcore.c"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if (PC % 4 != 0 || PC / 4 >= MEMSIZE) {
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

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

