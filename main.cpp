#include <iostream>

#include "Hardware/Include.hpp"
#include "Emulator/Include.hpp"

int main()
{
    chipe::Emulator emulator("RUNROM");

    emulator.InitCPU();
    emulator.InitRenderer();
    emulator.setOPS(600);
   
    while(1)
    {
        if(!emulator.Running()) break;
        emulator.Cycle();
    }
}