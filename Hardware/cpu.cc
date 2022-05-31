#pragma once

#include "cpu.hpp"

#include <cstring>
#include <iostream>
#include <cstdio>

namespace chipe
{
    void CPU::InitROM(const char* fp)
    {
        std::FILE* file=NULL;
        if((file=std::fopen(this->fp,"rb"))==NULL)
        {
            std::cout << "Couldn't open source hex file: " << this->fp << std::endl;
        }

        std::fread(&this->Memory[0x200],0xfff,1,file);
        std::fclose(file);
    }

    unsigned char CPU::Reset()
    {
        this->OpcodesPS=600;
        this->Delaytimer=(BYTE)0;
        this->Soundtimer=(BYTE)0;
        this->AddressI=0;
        this->ProgramCounter=0x200;
        std::memset(this->Registers,0,sizeof(this->Registers));
        std::memset(this->Keys,0,sizeof(this->Keys));
        std::memset(this->ScreenData,0, 64*32);
        std::memset(this->Registers,0, 16);

        for(int i=0; i<80; ++i)
        {
            this->Memory[i]=fontset[i];
        }

        // Load the game
        this->InitROM(this->fp);

        return 0;
    }   

    WORD CPU::getNextOpcode()
    {
        WORD code=0;

        if(this->ProgramCounter+1 >= 0xFFFF)
        {
            std::cout << "File ended" << std::endl;
            return 0;
        }

        code=this->Memory[this->ProgramCounter];      // get the first opcode
        code <<= 8;                                   // make space for the second opcode
        code |= this->Memory[this->ProgramCounter+1]; // merge with the next opcode
        this->ProgramCounter+=2;

        return code;
    }

    void CPU::Timer()
    {
        if(this->Delaytimer > 0)
            --this->Delaytimer;
        
        if(this->Soundtimer > 0)
        {
            if(this->Soundtimer == 1)
            {
                // play sound
            }
            --this->Soundtimer;
        }
    }

    void CPU::setKey(BYTE key, BYTE value)
    {
        this->Keys[key]=value;
    }

    void CPU::clearKeys()
    {
        for(int i=0x0; i<=0xF; ++i)
        {
            this->Keys[i]=0;
        }
    }

    //-------------------------------------------------------------------------------------------------
    //--- Opcodes

    void CPU::DecodeOpcode(WORD opcode) 
    {
        switch(opcode & 0xF000)
        {
            case 0x1000: this->Opcode1NNN(opcode); break; // Jump
            case 0x2000: this->Opcode2NNN(opcode); break; // Call subroutine 
            case 0x3000: this->Opcode3XNN(opcode); break; // Condition
            case 0x4000: this->Opcode4XNN(opcode); break; // Condition
            case 0x5000: this->Opcode5XY0(opcode); break; // Condition
            case 0x6000: this->Opcode6XNN(opcode); break; // Set VX to NN
            case 0x7000: this->Opcode7XNN(opcode); break; // Add NN to VX
            case 0x9000: this->Opcode9XY0(opcode); break; // Skip next instruction if VX != VY
            case 0xA000: this->OpcodeANNN(opcode); break; // Set I to address NNN
            case 0xB000: this->OpcodeBNNN(opcode); break; // Jumps to address NNN+V0
            case 0xC000: this->OpcodeCXNN(opcode); break; // Set VX to rand() & NN
            case 0xD000: this->OpcodeDXYN(opcode); break; // Draw sprite at (VX,VY), width of 8 pixels and N height
            
            case 0x0000:{
                switch(opcode & 0x000F) // get the last part of opcode
                {
                    case 0x0000: this->Opcode00E0(opcode); break; // Clear screen
                    case 0x000E: this->Opcode00EE(opcode); break; // Returns from subroutine
                }
            } break;

            case 0x8000:{
                switch(opcode & 0x000F) // get the last part of opcode
                {
                    case 0x0000: this->Opcode8XY0(opcode); break; // Set VX to value of VY
                    case 0x0001: this->Opcode8XY1(opcode); break; // Set VX to VX | VY
                    case 0x0002: this->Opcode8XY2(opcode); break; // Set VX to VX & VY
                    case 0x0003: this->Opcode8XY3(opcode); break; // Set VX to VY ^ VY
                    case 0x0004: this->Opcode8XY4(opcode); break; // Add VY to VX
                    case 0x0005: this->Opcode8XY5(opcode); break; // Subtract VY from VX
                    case 0x0006: this->Opcode8XY6(opcode); break; // Store LSB of VX in VF, shift VX to right by 1
                    case 0x0007: this->Opcode8XY7(opcode); break; // Set VX to VY-VX
                    case 0x000E: this->Opcode8XYE(opcode); break; // Store LSB of VX in VF, shift VX to left by 1
                }
            } break;

            case 0xE000:{
                switch(opcode & 0x000F) // get the last part of opcode
                {
                    case 0x000E: this->OpcodeEX9E(opcode); break; // Skip next instruction if VX is pressed
                    case 0x0001: this->OpcodeEXA1(opcode); break; // Skip next instruction if VX isn't pressed
                }
            } break;

            case 0xF000:{
                switch(opcode & 0x000F) // get the last part of opcode
                {
                    case 0x0007: this->OpcodeFX07(opcode); break; // Set VX to value of delay timer
                    case 0x000A: this->OpcodeFX0A(opcode); break; // Await keypress, then store in VX
                    case 0x0008: this->OpcodeFX18(opcode); break; // Set sound timer to VX
                    case 0x000E: this->OpcodeFX1E(opcode); break; // Add VX to I
                    case 0x0009: this->OpcodeFX29(opcode); break; // Set I to location of sprite at VX
                    case 0x0003: this->OpcodeFX33(opcode); break; // Store bin of VX, with MS of 3 digs at I, middle at I+1, LSD at I+2
                    case 0x0005:{
                        switch(opcode & 0x00F0) // get the third part of opcode
                        {
                            case 0x0010: this->OpcodeFX15(opcode); break; // Set delay timer to VX
                            case 0x0050: this->OpcodeFX55(opcode); break; // Store from V0 to VX in Memory starting at I
                            case 0x0060: this->OpcodeFX65(opcode); break; // Fill from V0 to VX from Memory starting at I
                        }
                    } break;
                }
            } break;

            default: std::cout << "Unknown Opcode - 0x" << std::hex << opcode << std::endl; break;
        }
    }

    void CPU::Opcode0NNN(WORD opcode)
    {

    } // 0
    void CPU::Opcode00E0(WORD opcode)
    {
        for(int i=0; i<64; ++i)
        {
            for(int j=0; j<32; ++j)
            {
                this->ScreenData[i][j]=0x0;
            }
        }
    } // 0
    void CPU::Opcode00EE(WORD opcode)
    {   
        this->ProgramCounter=this->stack.back();
        this->stack.pop_back();
    } // 0
    void CPU::Opcode1NNN(WORD opcode)
    {
        this->ProgramCounter=opcode & 0x0FFF;
    } // 1
    void CPU::Opcode2NNN(WORD opcode)
    {
        this->stack.push_back(this->ProgramCounter);
        this->ProgramCounter=opcode & 0x0FFF;
    } // 2
    void CPU::Opcode3XNN(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8; // shift to the right
        int NN=opcode & 0x00FF; // mask off NN

        if(this->Registers[regx]==NN)
            this->ProgramCounter+=2;
    } // 3
    void CPU::Opcode4XNN(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8; // shift to right
        int NN=opcode & 0x00FF; // mask off NN

        if(this->Registers[regx] != NN)
            this->ProgramCounter+=2;
    } // 4
    void CPU::Opcode5XY0(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        if(this->Registers[regx] == this->Registers[regy])
            this->ProgramCounter+=2;
    } // 5
    void CPU::Opcode6XNN(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int NN=opcode & 0x00FF; // mask off NN
        regx >>= 8;
        
        this->Registers[regx]=NN;
    } // 6
    void CPU::Opcode7XNN(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int NN=opcode & 0x00FF; // mask of NN
        regx >>= 8; 

        this->Registers[regx] += NN;
    } // 7
    void CPU::Opcode8XY0(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        this->Registers[regx]=this->Registers[regy];
    } // 8
    void CPU::Opcode8XY1(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        this->Registers[regx] |= this->Registers[regy];
    } // 8
    void CPU::Opcode8XY2(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        this->Registers[regx] &= this->Registers[regy];
    } // 8
    void CPU::Opcode8XY3(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        this->Registers[regx] ^= this->Registers[regy];
    } // 8
    void CPU::Opcode8XY4(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        this->Registers[0xF] = 1;
        if(this->Registers[regy] > this->Registers[regx])
            this->Registers[0xF] = 0;

        this->Registers[regx] += this->Registers[regy];
    } // 8
    void CPU::Opcode8XY5(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        this->Registers[0xF] = 1;
        if(this->Registers[regy] > this->Registers[regx])
            this->Registers[0xF] = 0;

        this->Registers[regx] -= this->Registers[regy];
    } // 8
    void CPU::Opcode8XY6(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;
        
        this->Registers[0xF] = this->Registers[regx] & 0x1;
        this->Registers[regx] >>= 1;
    } // 8
    void CPU::Opcode8XY7(WORD opcode)
    {
        int regx=opcode & 0x0F00;
        int regy=opcode & 0x00F0;
        regx >>= 8;
        regy >>= 4;

        this->Registers[0xF] = 1;
        if(this->Registers[regx] > this->Registers[regy])
            this->Registers[0xF] = 0;
        
        this->Registers[regx] = this->Registers[regy] - this->Registers[regx];
    } // 8
    void CPU::Opcode8XYE(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        this->Registers[0xF] = this->Registers[regx] >> 7;
        this->Registers[regx] <<= 1;
    } // 8
    void CPU::Opcode9XY0(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        regx >>= 8;
        regy >>= 4;

        if(this->Registers[regx] != this->Registers[regy])
            this->ProgramCounter+=2;
    } // 9
    void CPU::OpcodeANNN(WORD opcode)
    {
        int NNN=opcode & 0x0FFF; // mask off NNN

        this->AddressI=NNN;
    } // A
    void CPU::OpcodeBNNN(WORD opcode)
    {
        int NNN=opcode & 0x0FFF; // mask off NNN
        this->ProgramCounter=this->Registers[0x0] + NNN;
    } // B
    void CPU::OpcodeCXNN(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int NN=opcode & 0x00FF; // mask off NN
        regx >>= 8;

        this->Registers[regx] = (rand() & 0xFF) & NN;
    } // C
    void CPU::OpcodeDXYN(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        int regy=opcode & 0x00F0; // mask off Y
        int height=opcode & 0x000F; // mask off N
        regx >>= 8;
        regy >>= 4;

        int posx=this->Registers[regx];
        int posy=this->Registers[regy];

        this->Registers[0xF] = 0;

        for(int yline=0; yline<height; ++yline)
        {
            BYTE pixdata=this->Memory[AddressI+yline];
            for(int xpixel=0; xpixel<8; ++xpixel)
            {
                int mask=1 << (7-xpixel);
                if(pixdata & mask)
                {
                    int x=posx+xpixel;
                    int y=posy+yline;
                    if(this->ScreenData[x][y]==1)
                        this->Registers[0xF] = 1; // it collided with already-on pixels
                    this->ScreenData[x][y] ^= 1;
                }
            }
        }
    } // D
    void CPU::OpcodeEX9E(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        if(this->Keys[this->Registers[regx]] != 0)
            this->ProgramCounter+=2;
    } // E
    void CPU::OpcodeEXA1(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X

        if(this->Keys[this->Registers[regx]] == 0)
            this->ProgramCounter+=2;
    } // E
    void CPU::OpcodeFX07(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        this->Registers[regx] = this->Delaytimer;
    } // F
    void CPU::OpcodeFX0A(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        bool pressed=false;
        for(int i=0; i<16; ++i)
        {
            if(this->Keys[i]!=0)
            {
                this->Registers[regx]=i;
                pressed=true;  
            }
        }
        
        if(!pressed)
            return; // skip cycle & try again, block everything till next keypress
    } // F
    void CPU::OpcodeFX15(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        this->Delaytimer=this->Registers[regx];
    } // F
    void CPU::OpcodeFX18(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        this->Soundtimer=this->Registers[regx];
    } // F
    void CPU::OpcodeFX1E(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        this->Registers[0xF] = 0;
        if(this->AddressI + this->Registers[regx] > 0xFFF)
            this->Registers[0xF] = 1;

        this->AddressI += this->Registers[regx];
    } // F
    void CPU::OpcodeFX29(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        this->AddressI = this->Registers[regx] * 0x5;
    } // F
    void CPU::OpcodeFX33(WORD opcode)
    {
        int regx = opcode & 0x0F00; // mask off X
        regx >>= 8;

        int value=this->Registers[regx];

        int hundreds, tens, units;
        hundreds=value/100;
        tens=(value/10)%10;
        units=value%10;

        this->Memory[this->AddressI] = hundreds;
        this->Memory[this->AddressI+1] = tens;
        this->Memory[this->AddressI+2] = units; 
    } // F
    void CPU::OpcodeFX55(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        for(int i=0; i<regx+1; ++i)
            this->Memory[this->AddressI+i] = this->Registers[i]; 

        this->AddressI = (WORD)this->AddressI+regx+1;
    } // F
    void CPU::OpcodeFX65(WORD opcode)
    {
        int regx=opcode & 0x0F00; // mask off X
        regx >>= 8;

        for(int i=0; i<regx+1; ++i)
            this->Registers[i] = this->Memory[this->AddressI+i];

        this->AddressI = (WORD)this->AddressI+regx+1;
    } // F
}
