// a dirty, cheap way to find bugs in disassemblers - compare two good ones

#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "BeaEngine.h"

#include "libudis86/types.h"
#include "libudis86/extern.h"
#include "libudis86/itab.h"

void main()
{
    srand(time(0));

    UCHAR buff[15];


    while(1)
    {
        //
        // randomize buffer
        for(INT i=0; i<15; ++i)
            buff[i] = rand() % 256;

        //
        // BeaEngine
        // 
        BOOL bValid_beaengine = 0;

        DISASM dasm;
        memset(&dasm, 0, sizeof(dasm));
        dasm.EIP = (UINT64) buff;
        dasm.VirtualAddr = 0;
        
        UINT len_beaengine = Disasm(&dasm);

        if(len_beaengine != UNKNOWN_OPCODE)
        {
            /*
            for(INT i=0; i<len; ++i)
                printf("%02X ", buff[i]);

            printf("%s\n", dasm.CompleteInstr);
            */

            bValid_beaengine = 1;
        }

        //
        // udis86
        //
        BOOL bValid_udis86 = 0;

        ud_t ud_obj;
        ud_init(&ud_obj);
        ud_set_input_buffer(&ud_obj, buff, 15);
        ud_set_mode(&ud_obj, 32);
        ud_set_pc(&ud_obj, 0);
        ud_set_syntax(&ud_obj, UD_SYN_INTEL);


        UINT len_udis86 = ud_disassemble(&ud_obj);
        
        if(len_udis86 != 0)
        {
            bValid_udis86 = 1;
        }
        
        //
        // comparison time
        //
        if( bValid_beaengine && 
            bValid_udis86 &&
            len_udis86 != len_beaengine &&
            dasm.Instruction.Opcode != 0xC7 &&
            dasm.Instruction.Opcode != 0xC6 &&
            dasm.Instruction.Opcode != 0x2E &&
            dasm.Instruction.Opcode != 0xCE &&
            buff[0] != 0x0F &&
            buff[1] != 0x0F
        )
        {
            printf("on bytes: ");

            for(INT i=0; i<15; ++i)
                printf("%02X ", buff[i]);

            printf("\n");
            printf("bea engine: %s (len: %d)\n", dasm.CompleteInstr, len_beaengine);
            printf("udis86: %s (len: %d)\n", ud_insn_asm(&ud_obj), len_udis86); 
            printf("\n");
        }
    }
}
