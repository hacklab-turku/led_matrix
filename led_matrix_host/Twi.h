#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include <avr/io.h>

void TWIInit(void){

    //set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x1B;
    //enable TWI
    TWCR = (1<<TWEN);

}

inline void TWIWaitReady(){
    while ((TWCR & (1<<TWINT)) == 0){};
}

inline uint8_t TWIGetStatus(void){
    return (TWSR & 0xF8);
}

void TWIWrite(uint8_t u8data){
    TWIWaitReady();
    TWDR = u8data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    TWIWaitReady();
}

void TWIStartWrite(uint8_t address){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    TWIWaitReady();
    TWIWrite(address);
}

void TWIStartRead(uint8_t address){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    TWIWaitReady();
    TWIWrite(address + 1);
}

void TWIStop(void){
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

uint8_t TWIReadACK(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    TWIWaitReady();
    return TWDR;
}

//read byte with NACK
uint8_t TWIReadNACK(void){
    TWCR = (1<<TWINT)|(1<<TWEN);
    TWIWaitReady();
    return TWDR;
}


#define BANK0_IODIRA 0x00
#define BANK0_IODIRB 0x01
#define BANK0_IPOLA 0x02
#define BANK0_IPOLB 0x03
#define BANK0_GPINTENA 0x04
#define BANK0_GPINTENB 0x05
#define BANK0_DEFVALA 0x06
#define BANK0_DEFVALB 0x07
#define BANK0_INTCONA 0x08
#define BANK0_INTCONB 0x09
#define BANK0_IOCON 0x0A
#define BANK0_GPPUA 0x0C
#define BANK0_GPPUB 0x0D
#define BANK0_INTFA 0x0E
#define BANK0_INTFB 0x0F
#define BANK0_INTCAPA 0x10
#define BANK0_INTCAPB 0x011
#define BANK0_GPIOA 0x12
#define BANK0_GPIOB 0x13
#define BANK0_OLATA 0x14
#define BANK0_OLATB 0x15

#define BANK1_IODIRA 0x00
#define BANK1_IODIRB 0x10
#define BANK1_IPOLA 0x01
#define BANK1_IPOLB 0x11
#define BANK1_GPINTENA 0x02
#define BANK1_GPINTENB 0x12
#define BANK1_DEFVALA 0x03
#define BANK1_DEFVALB 0x13
#define BANK1_INTCONA 0x04
#define BANK1_INTCONB 0x14
#define BANK1_IOCON 0x05
#define BANK1_GPPUA 0x06
#define BANK1_GPPUB 0x16
#define BANK1_INTFA 0x07
#define BANK1_INTFB 0x17
#define BANK1_INTCAPA 0x08
#define BANK1_INTCAPB 0x18
#define BANK1_GPIOA 0x09
#define BANK1_GPIOB 0x19
#define BANK1_OLATA 0x0A
#define BANK1_OLATB 0x1A



#endif // TWI_H_INCLUDED
