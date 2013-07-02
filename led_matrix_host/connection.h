#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED


#define LM_INVERSE          1
#define LM_ROTATE_LEFT      2
#define LM_ROTATE_RIGHT     3
#define LM_ROTATE_UP        4
#define LM_ROTATE_DOWN      5
#define LM_SHIFT_LEFT       6
#define LM_SHIFT_RIGHT      7
#define LM_SHIFT_UP         8
#define LM_SHIFT_DOWN       9
#define LM_WRITE_MATRIX     10
#define LM_WRITE_ID         11
#define LM_WRITE_LED        12
#define LM_WRITE_ALL        13

#define LM_READ_CARRY       128
#define LM_READ_MATRIX      129

/*---------------------------------------
 * Basic function to cotrol led arrays
 *---------------------------------------
 */


/* MAX_DEVICE_COUNT defines how many matrices
 * you can have connectes in to your system.
 */

#define MAX_DEVICE_COUNT 6

/* Call this function to initialize i2c
 * connection.
 */
void initConnection();

/* This function must be called before you can use
 * updateScreen function. Calling program must create
 * screen buffer which pointer and size are given
 * to driver.
 *
 * %param *matrix: pointer to 2d array
 * %param rows: number of rows in matrix
 * %paran cols: number of colums in matrix
 */
void setScreenMatrix(uint8_t *matrix, uint8_t rows, uint8_t cols);

/* This function lets you change matrix
 * without need to set deviced again.
 * The dimension of the matrix must be same
 * whit the one set by setScreenMatrix.
 *
 * %param *matrix: pointer to new matrix
 */
void swapMatrix(uint8_t *matrix);

/* This function sets the devices and their position in
 * the matrix.
 *
 * %param dev_id: Id of the device
 * %param row_offset: row offset value. Must be >=0 and < rows of the matrix - 8
 * %param col_offset: colum offset value. Must be >=0 and < colums of the matrix - 8
 */
void setScreenDevice(uint8_t dev_id, uint8_t row_offset, uint8_t col_offset);

/* Removes all devices that are set. After this you
 * can set devices in new order
 */
void deleteAllDevices();

/* This function sends matrix data to the devices in ways
 * that it is set with setScreenMatrix and setScreenDevice.
 */
void updateScreen();

/* This function sets one led in the screen with
 * given value. At least the device which holds the position
 * must be given with setScreenDevice function
 *
 * %param x: x position for led to be set
 * %param y: y position for led to be set
 * %param val: value to be set on given led.  Value between 0 and 16
 */
void setScreenLed(uint8_t x, uint8_t y, uint8_t val);


/*---------------------------------------------------
 * Advanced functions to control individual devices
 *---------------------------------------------------
 */

/* This function sends 8x8 matrix to given device.
 *
 * %param device: Device id
 * %param *mat: pointer to 8x8 matrix
 */
void sendMatrix(uint8_t device, uint8_t *mat);

/* This function sets all leds to given value
 * on given device.
 *
 * %param device: Device id
 * %param i: Value to be set,  0 >= i < 16
 */
void writeAllLeds(uint8_t device, uint8_t i);

/* This function sets one led to given value
 * given device.
 *
 * %param device: Device id
 * %param x: x position of led, 0 >= x < 8
 * %param y: y position of led, 0 >= y < 8
 * %param i: value to be set on led
 */
void writeOneLed(uint8_t device, uint8_t x, uint8_t y, uint8_t i);

/* This function reads carry data from device.
 * See shift functions.
 *
 * %param device: Device id
 * %param *data: pointer to 8 byte array where data is put
 */
void readCarry(uint8_t device, uint8_t *data);

/* These functions can be used to send raw commands
 * to devices. Also data can be sent
 *
 * %param device: Device id
 * %param command: command to send, see defines
 * %param *data: pointer to send data
 * %param len: length of the send data
 */
void sendCommand(uint8_t device, uint8_t command);
void sendCommandWithData(uint8_t device, uint8_t command, uint8_t* data, uint8_t len);

/* These functions rotates rows or colums of the device.
 * The overfloved row or colum is put to other side.
 *
 * %param device: Device id
 */
void rotateLeft(uint8_t device);
void rotateRight(uint8_t device);
void rotateUp(uint8_t device);
void rotateDown(uint8_t device);

/* These functions shifts rows or colums of the device.
 * 8 byte array from data is put to the empty row or colum.
 * Overflovn data is stored in device's carry data. It
 * can be read with readCarry function.
 *
 * %param device: Device id
 * %param *data: 8 byte array, that hold data to be sent
 */
void shiftLeft(uint8_t device, uint8_t *data);
void shiftRight(uint8_t device, uint8_t *data);
void shiftUp(uint8_t device, uint8_t *data);
void shiftDown(uint8_t device, uint8_t *data);

#endif // CONNECTION_H_INCLUDED
