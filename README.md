# CHIP-E Emulator

## A Non-Working CHIP-8 Interpreter/Emulator

I tried to run several roms but none of them seemed to work with my emulator, it usually throws random visual bugs.
You can help me by contributing to my code. 

To run your ROM (for CHIP-8 only, not SuperChip), rename your ROM to `RUNROM` without any file extensions.
Double click on the main.exe file, and it will open a nice window which plays your ROM.

All the Opcodes along with the Opcode Mapper are written in the CPU.cc file in a functional and ordered manner.
To edit an Opcode, simply edit the function. In some very rare cases which include the Opcode Mapper wrongly mapping
the Opcodes, you can edit the Mapper from the function controlling it.
