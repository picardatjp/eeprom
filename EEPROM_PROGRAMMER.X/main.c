/*
 * File:   main.c
 * Author: Jacob Picardat
 * 4/24/2021
 * 
 * Chip: PIC16f1829
 * 
 * EEPROM: AT28C64B
 * 
 * Function: EEPROM PROGRAMMER
 * 
 * This program can be used to write data into an AT28C64B EEPROM. First, an address
 * is shifted out to 2 74HC595 8-bit Shift Registers. Once the address has been latched,
 * the I/O pins are pulled high or low to represent the value to be written. Then the
 * write enable signal is pulsed low causing a write of the value from the I/O pins to
 * AT28C64B.
 * 
 * NOTES: Methods accept integers for data, but the AT28C64B stores bytes. Also, remember, 
 * memory is in short supply, thus arrays cannot be that big. Around 40 bytes worth.
 *
 * Created on April 24, 2021, 8:14 AM
 */

#include <xc.h>

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


// Definitions for interaction with pins
#define _XTAL_FREQ  500000
#define SHIFT_DATA  PORTAbits.RA4
#define SHIFT_LATCH PORTCbits.RC3
#define SHIFT_CLOCK PORTCbits.RC6
#define WRITE_EN    PORTCbits.RC7
#define IO_0        PORTAbits.RA0
#define IO_1        PORTAbits.RA1
#define IO_2        PORTAbits.RA2
#define IO_3        PORTCbits.RC0
#define IO_4        PORTCbits.RC1
#define IO_5        PORTCbits.RC2
#define IO_6        PORTBbits.RB4
#define IO_7        PORTBbits.RB5

// Method Definitions
void shiftOutByte(int);
void pulseLatch();
void pulseClock();
void pulseWriteEn();
void setAddress(int);
void writeByteToEEPROM(int,int);
void writeArrayToEEPROM(int*,int,int);

// Set up for registers and pins
void init()
{
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    SHIFT_CLOCK = 0;
    SHIFT_LATCH = 0;
    WRITE_EN = 1;
    
}

int main() 
{
    init();    
    __delay_ms(1000);
        
    
    // WRITE TO EEPROM HERE 
    
    int arr[] = {0x55,0xaa,0x55,0xaa};
    int size = 4;
    int startingAddress = 32;    
    int *arrP = &arr[0];
    
    writeArrayToEEPROM(arrP,size,startingAddress);
    
    
    
    
    // FINISH WRITING TO EEPROM HERE
    
    // clear outputs for safe removal of EEPROM
    PORTC = 0;
    PORTB = 0;
    PORTA = 0;
    
    // Sit and wait to prevent repeating program
    while(1)
    {
    
    }
    
    return 0;
}

// Sends byte to 74HC595
void shiftOutByte(int data)
{
    int i;
    for(i=0;i<8;i++)
    {
        SHIFT_DATA = (0b10000000 & data) >> 7;
        data = data << 1;
        pulseClock();
    }
    pulseLatch();
}

// Sends memory address to 2 74HC595
void setAddress(int address)
{
    shiftOutByte(address >> 8);
    shiftOutByte(address);
}

// Writes byte to memory address
void writeByteToEEPROM(int address, int data)
{
    setAddress(address);
    __delay_us(500);
    
    IO_0 = 1 & data;
    data = data >> 1;
    IO_1 = 1 & data;
    data = data >> 1;
    IO_2 = 1 & data;
    data = data >> 1;
    IO_3 = 1 & data;
    data = data >> 1;
    IO_4 = 1 & data;
    data = data >> 1;
    IO_5 = 1 & data;
    data = data >> 1;
    IO_6 = 1 & data;
    data = data >> 1;
    IO_7 = 1 & data;
    
    pulseWriteEn();
}

// Writes values from an array to EEPROM, in order, over a specified range
void writeArrayToEEPROM(int *arr, int arrSize, int address)
{
    int i;
    for(i=0;i<arrSize;i++)
    {
        writeByteToEEPROM(address+i, *(arr+i));
    }    
}

// Brings WRITE_EN low for a brief period, the signal is active low
void pulseWriteEn()
{
    __delay_us(800);
    WRITE_EN = 0;
    __delay_us(800);
    WRITE_EN = 1;
    
}

// Brings SHIFT_CLOCK high for a brief period, the signal is active high (rising edge)
void pulseClock()
{
    SHIFT_CLOCK = 1;
    __delay_us(500);
    SHIFT_CLOCK = 0;
}

// Brings SHIFT_LATCH high for a brief period, the signal is active high (rising edge)
void pulseLatch()
{
    SHIFT_LATCH = 1;
    __delay_us(500);
    SHIFT_LATCH = 0;
}