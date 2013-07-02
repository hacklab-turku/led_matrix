

#include "Twi.h"
#include "connection.h"
#include <util/delay.h>

struct device {
    uint8_t dev_id;
    uint8_t row_offset;
    uint8_t col_offset;
};

uint8_t *screen_buffer = 0;
uint8_t screen_rows, screen_cols;

struct device devices[MAX_DEVICE_COUNT];
uint8_t dev_count = 0;

void initConnection(){
    TWIInit();
}

void setScreenMatrix(uint8_t *matrix, uint8_t rows, uint8_t cols){
    screen_buffer = matrix;
    screen_rows = rows;
    screen_cols = cols;
    dev_count = 0;
}

void swapMatrix(uint8_t *matrix){
    screen_buffer = matrix;
}

void setScreenDevice(uint8_t dev_id, uint8_t row_offset, uint8_t col_offset){

    if (dev_count > MAX_DEVICE_COUNT) return;   //Just to be sure that we dont overflow
    if (row_offset + 8 > screen_rows) return;
    if (col_offset + 8 > screen_cols) return;

    devices[dev_count].dev_id = dev_id;
    devices[dev_count].row_offset = row_offset;
    devices[dev_count].col_offset = col_offset;
    dev_count++;
}

void updateScreen(){
    uint8_t dev;

    for (dev = 0; dev < dev_count; dev++){
        TWIStartWrite(devices[dev].dev_id);
        TWIWrite(LM_WRITE_MATRIX);

        uint8_t r,c;

        for (r = devices[dev].row_offset; r < devices[dev].row_offset + 8; r++)
            for (c = devices[dev].col_offset; c < devices[dev].col_offset + 8; c++)
                TWIWrite(*(screen_buffer + c + r * screen_cols));

        TWIStop();
    }
}

void deleteAllDevices(){
    dev_count = 0;
}

void setScreenLed(uint8_t x, uint8_t y, uint8_t val){
    uint8_t dev;

    for (dev = 0; dev < dev_count; dev++){
        if (devices[dev].row_offset >= x && devices[dev].row_offset < x + 8 ) {
            if (devices[dev].col_offset >= y && devices[dev].col_offset < y + 8 ) {
                writeOneLed(devices[dev].dev_id, x - devices[dev].row_offset, y - devices[dev].col_offset, val);
            }
        }
    }
}

void sendMatrix(uint8_t device, uint8_t *mat){
    TWIStartWrite(device);
    TWIWrite(LM_WRITE_MATRIX);

    uint8_t u,i;

    for (i = 0; i < 8; i++)
        for (u = 0; u < 8; u++)
            TWIWrite(*(mat + u + i * 8));

    TWIStop();
}



void writeAllLeds(uint8_t device, uint8_t i){

    TWIStartWrite(device);
    TWIWrite(LM_WRITE_ALL);
    TWIWrite(i);

    TWIStop();
}

void writeOneLed(uint8_t device, uint8_t x, uint8_t y, uint8_t i){
    TWIStartWrite(device);
    TWIWrite(LM_WRITE_LED);
    uint8_t tmp = (x << 3) + y;
    TWIWrite(tmp);
    TWIWrite(i);

    TWIStop();
    _delay_us(100);
}

void readCarry(uint8_t device, uint8_t *data){
    _delay_us(1000);
    TWIStartRead(device);

    data[0] = TWIReadACK();
    data[1] = TWIReadACK();
    data[2] = TWIReadACK();
    data[3] = TWIReadACK();
    data[4] = TWIReadACK();
    data[5] = TWIReadACK();
    data[6] = TWIReadACK();
    data[7] = TWIReadNACK();

    TWIStop();
}


void sendCommand(uint8_t device, uint8_t command){
    TWIStartWrite(device);
    TWIWrite(command);
    TWIStop();
}

void sendCommandWithData(uint8_t device, uint8_t command, uint8_t* data, uint8_t len){
    TWIStartWrite(device);
    TWIWrite(command);

    uint8_t u;

    for (u = 0; u < len; u++)
        TWIWrite(data[u]);

    TWIStop();
}

void rotateLeft(uint8_t device){
    sendCommand(device, LM_ROTATE_LEFT);
}
void rotateRight(uint8_t device){
    sendCommand(device, LM_ROTATE_RIGHT);
}
void rotateUp(uint8_t device){
    sendCommand(device, LM_ROTATE_UP);
}
void rotateDown(uint8_t device){
    sendCommand(device, LM_ROTATE_DOWN);
}

void shiftLeft(uint8_t device, uint8_t *data){
    sendCommandWithData(device, LM_SHIFT_LEFT, data, 8);
}
void shiftRight(uint8_t device, uint8_t *data){
    sendCommandWithData(device, LM_SHIFT_RIGHT, data, 8);
}
void shiftUp(uint8_t device, uint8_t *data){
    sendCommandWithData(device, LM_SHIFT_UP, data, 8);
}
void shiftDown(uint8_t device, uint8_t *data){
    sendCommandWithData(device, LM_SHIFT_DOWN, data, 8);
}
