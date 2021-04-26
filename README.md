# EEPROM PROGRAMMER
Written for the PIC16f1829 to program the AT28C64B.


Use: Import project or main.c into MPLAB. In the main method, within the main.c, calling writeByteToEEPROM(int, int) will write a single byte to a single memory location on the AT28C64B. Calling writeArrayToEEPROM(int*,int,int) will write bytes to EEPROM from an array in order starting with index 0. Write operations will begin at the specified address and end at address + arrSize (arrSize being the size of the array passed).

Notes: Memory on the PIC16f1829 is limited and integer arrays with over 40 entries might take up too much space, preventing successful compilation.
