#pragma once

#include <vector>

namespace chipe
{

typedef unsigned char BYTE;
typedef unsigned short int WORD;

// font set for rendering
const BYTE fontset[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

class CPU
{
protected:
    BYTE Memory[0xFFF];
    BYTE Registers[6];
    BYTE Keys[16];
    BYTE Delaytimer;
    BYTE Soundtimer;
    WORD AddressI;
    WORD ProgramCounter;
    std::vector<WORD> stack;
public:
    BYTE ScreenData[64][32];
    int OpcodesPS;
    const char* fp;

    //-------------------------------------------------------------------------------------------------

    CPU(const char* fp): fp(fp), OpcodesPS() {}

    //-------------------------------------------------------------------------------------------------
    
    void InitROM(const char* fp);
    unsigned char Reset();
    void Timer();
    void setKey(BYTE key, BYTE value);
    void clearKeys();

    //-------------------------------------------------------------------------------------------------
    
    WORD getNextOpcode();
    
    //-------------------------------------------------------------------------------------------------
    //--- Opcodes

    void DecodeOpcode(WORD opcode);
    
    void Opcode0NNN(WORD opcode); // 0
    void Opcode00E0(WORD opcode); // 0
    void Opcode00EE(WORD opcode); // 0
    void Opcode1NNN(WORD opcode); // 1
    void Opcode2NNN(WORD opcode); // 2
    void Opcode3XNN(WORD opcode); // 3
    void Opcode4XNN(WORD opcode); // 4
    void Opcode5XY0(WORD opcode); // 5
    void Opcode6XNN(WORD opcode); // 6
    void Opcode7XNN(WORD opcode); // 7
    void Opcode8XY0(WORD opcode); // 8
    void Opcode8XY1(WORD opcode); // 8
    void Opcode8XY2(WORD opcode); // 8
    void Opcode8XY3(WORD opcode); // 8
    void Opcode8XY4(WORD opcode); // 8
    void Opcode8XY5(WORD opcode); // 8
    void Opcode8XY6(WORD opcode); // 8
    void Opcode8XY7(WORD opcode); // 8
    void Opcode8XYE(WORD opcode); // 8
    void Opcode9XY0(WORD opcode); // 9
    void OpcodeANNN(WORD opcode); // A
    void OpcodeBNNN(WORD opcode); // B
    void OpcodeCXNN(WORD opcode); // C
    void OpcodeDXYN(WORD opcode); // D
    void OpcodeEX9E(WORD opcode); // E
    void OpcodeEXA1(WORD opcode); // E
    void OpcodeFX07(WORD opcode); // F
    void OpcodeFX0A(WORD opcode); // F
    void OpcodeFX15(WORD opcode); // F
    void OpcodeFX18(WORD opcode); // F
    void OpcodeFX1E(WORD opcode); // F
    void OpcodeFX29(WORD opcode); // F
    void OpcodeFX33(WORD opcode); // F
    void OpcodeFX55(WORD opcode); // F
    void OpcodeFX65(WORD opcode); // F
};

}