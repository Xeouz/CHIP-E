#pragma once

#include "../Hardware/Include.hpp"
#include "SFML/Graphics.hpp"

namespace chipe
{

class Emulator
{
    CPU* cpu;
    sf::RenderWindow RScreen;
    sf::View view;
    sf::RectangleShape matrix[64][32];
    unsigned char run;

    int resX, resY;
public:
    unsigned long long int MaxOPS;
    Emulator(CPU* cpu) : cpu(cpu), RScreen(sf::VideoMode(200,200),"CHIP-E Emulator"), MaxOPS(6000) {}
    Emulator(const char* path_to_rom) : RScreen(sf::VideoMode(200,200),"CHIP-E Emulator"), MaxOPS(6000)
    {
        this->cpu=new CPU(path_to_rom);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void InitCPU();
    void InitRenderer();
    void Cycle();
    void Render();

    void MakeMatrix(sf::Color color);
    void MakeMatrix();

    void setOPS(unsigned long long int OPS);

    void mapKeyboard(char key, BYTE value);
    void handleInputs(sf::Event event);

    unsigned char Running();
};

}