
#define TWI_SLAVE_ADDRESS   2


#define F_CPU 8000000UL


#define PIN_SHIFT_CLOCK     PINC1
#define PIN_LATCH_CLOCK     PINC2
#define PIN_DATA            PINC3

#define PORT_SHIFT_CLOCK    PORTC
#define PORT_LATCH_CLOCK    PORTC
#define PORT_DATA           PORTC

#define DDR_SHIFT_CLOCK     DDRC
#define DDR_LATCH_CLOCK     DDRC
#define DDR_DATA            DDRC

#define PORT_COL            PORTD
#define DDR_COL             DDRD

#define DDR_DEBUG_LED       DDRB
#define PORT_DEBUG_PORT     PORTB
#define PIN_DEBUG_LED       PINB1


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

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/twi.h>



void bit_bang(uint8_t data);

inline uint8_t rol(uint8_t b) {return (b << 1) | (b >> 7);}
inline uint8_t ror(uint8_t b) {return (b >> 1) | (b << 7);}

void shift_right();
void shift_left();
void shift_up();
void shift_down();
void set_right(uint8_t *data);
void set_left(uint8_t *data);
void set_up(uint8_t *data);
void set_down(uint8_t *data);

void do_task(uint8_t command, uint8_t *data, uint8_t len);

volatile uint8_t pwm_matrix_buffer [8][8] = {{1,  2,  3,  4,  5,  6,  7,  8},
                                             {8,  7,  6,  5,  4,  3,  2,  1},
                                             {1,  2,  3,  4,  5,  6,  7,  8},
                                             {8,  7,  6,  5,  4,  3,  2,  1},
                                             {1,  2,  3,  4,  5,  6,  7,  8},
                                             {8,  7,  6,  5,  4,  3,  2,  1},
                                             {1,  2,  3,  4,  5,  6,  7,  8},
                                             {8,  7,  6,  5,  4,  3,  2,  1} };

uint8_t carry[8] = {0,0,0,0,0,0,0,0};
volatile uint32_t time = 0;




int main(void)
{
    DDR_DATA |= (1 << PIN_DATA);
    DDR_SHIFT_CLOCK |= (1 << PIN_SHIFT_CLOCK);
    DDR_LATCH_CLOCK |= (1 << PIN_LATCH_CLOCK);

    PORT_SHIFT_CLOCK &= ~(1 << PIN_SHIFT_CLOCK);
    PORT_LATCH_CLOCK &= ~(1 << PIN_LATCH_CLOCK);

    DDR_COL = 0xFF;
    PORT_COL = 0;

    TCCR2A |= (1<<WGM21); //Timer2 to CTC mode
    TCCR2B |= (1<<CS21) | (1<<CS22); //Timer2 prescaler to 256
    OCR2A = 2; //Set compare A in timer2 so it will trigger interrupt every ~64 us

    TIMSK2 |= (1<<OCIE2A); //Enable compare A interrupt in timer2

    TCCR1A |= (1<<WGM12); //Timer2 to CTC mode
    TCCR1B |= (1<<CS12); //Timer2 prescaler to 256
    OCR1A = 31;  //Set compare B in timer2 so it will trigger interrupt every ~1 ms
    TIMSK1 |= (1<<OCIE1A); //Enable compare A interrupt in timer2



    sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed

    TWAR = TWI_SLAVE_ADDRESS + 1;
    TWCR = (1 << TWEA) | (1 << TWEN);

    DDR_DEBUG_LED  |= (1 << PIN_DEBUG_LED);

    PORT_DEBUG_PORT |= (1 << PIN_DEBUG_LED);
    _delay_ms(500);
    PORT_DEBUG_PORT &= ~(1 << PIN_DEBUG_LED);


    uint8_t twi_first_byte = 0;
    uint8_t twi_bytes_recived = 0;
    uint8_t twi_command = 0;

    uint8_t twi_bytes_transmited = 0;

    uint8_t twi_data[128];


    while(1){
      if (TWCR & (1 << TWINT)){
            switch (TW_STATUS){

                /* Recive mode */
                case TW_SR_SLA_ACK:
                case TW_SR_GCALL_ACK:
                    twi_bytes_recived = 0;
                    twi_first_byte = 1;
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;

                case TW_SR_DATA_ACK:
                case TW_SR_GCALL_DATA_ACK :
                    if (twi_first_byte){
                        twi_command = TWDR;
                        twi_first_byte = 0;
                    }
                    else if(twi_bytes_recived < 128) {
                        twi_data[twi_bytes_recived] = TWDR;
                        twi_bytes_recived++;
                    }
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;

                case TW_SR_STOP:
                    PORT_DEBUG_PORT |= (1 << PIN_DEBUG_LED);
                    do_task(twi_command, twi_data, twi_bytes_recived);
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;
                case TW_NO_INFO:
                    break;
                case TW_BUS_ERROR:
                    TWCR |= (1 << TWSTO) | (1 << TWINT);
                    break;

                /*Transfer mode*/
                case TW_ST_SLA_ACK:
                    twi_bytes_transmited = 1;
                    TWDR = carry[0];
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;

                case TW_ST_DATA_ACK:
                    TWDR = carry[twi_bytes_transmited++];
                    if (twi_bytes_transmited < 7)
                        TWCR |= (1 << TWEA) | (1 << TWINT);
                    else
                        TWCR |= (1 << TWINT);
                    break;
                case TW_ST_DATA_NACK:
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;

                case TW_ST_LAST_DATA :
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;
                default:
                    TWCR |= (1 << TWEA) | (1 << TWINT);
                    break;


            }
            PORT_DEBUG_PORT &= ~(1 << PIN_DEBUG_LED);
        }

    }

    return 0;
}

void do_task(uint8_t command, uint8_t *data, uint8_t len) {

    uint8_t u,i;
    uint8_t tmp;

    switch (command){
        case LM_INVERSE:
            for (i = 0; i < 8; i++){
                for (u = 0; u < 8; u++){
                    if (pwm_matrix_buffer[i][u] > 16) pwm_matrix_buffer[i][u] = 16;
                    pwm_matrix_buffer[i][u] = 16 - pwm_matrix_buffer[i][u];
                }
            }
            break;

        case LM_ROTATE_LEFT:
            shift_left();
            set_right(carry);
            break;
        case LM_ROTATE_RIGHT:
            shift_right();
            set_left(carry);
            break;
        case LM_ROTATE_UP:
            shift_up();
            set_down(carry);
            break;
        case LM_ROTATE_DOWN:
            shift_down();
            set_up(carry);
            break;
        case LM_SHIFT_LEFT:
            shift_left();
            set_right(data);
            break;
        case LM_SHIFT_RIGHT:
            shift_right();
            set_left(data);
            break;
        case LM_SHIFT_UP:
            shift_up();
            set_down(data);
            break;
        case LM_SHIFT_DOWN:
            shift_down();
            set_up(data);
            break;

        case LM_WRITE_ID:
            break;

        case LM_WRITE_ALL:
            for (i = 0; i < 8; i++)
                for (u = 0; u < 8; u++)
                    pwm_matrix_buffer[i][u] = data[0] ;
            break;

        case LM_WRITE_LED:
            tmp = data[0];
            pwm_matrix_buffer[tmp >> 3][tmp & 0x07] = data[1];
            break;

        case LM_WRITE_MATRIX:
            for (i = 0; i < 8; i++)
                for (u = 0; u < 8; u++)
                    pwm_matrix_buffer[u][i] = data[(u * 8) + i] ;
            break;

        default:
            break;
    }



}



void bit_bang(uint8_t data){

    uint8_t current_bit = 0b000000001;

    while (current_bit){
        if (data & current_bit) PORT_DATA |= (1 << PIN_DATA);
        else PORT_DATA &= ~(1 << PIN_DATA);

        PORT_SHIFT_CLOCK |= (1 << PIN_SHIFT_CLOCK);
        PORT_SHIFT_CLOCK &= ~(1 << PIN_SHIFT_CLOCK);

        current_bit <<= 1;
    }
    PORT_LATCH_CLOCK |= (1 << PIN_LATCH_CLOCK);
    PORT_LATCH_CLOCK &= ~(1 << PIN_LATCH_CLOCK);
}



ISR(TIMER2_COMPA_vect){
    static uint8_t current_row = 0b10000000;
    static uint8_t row = 0;
    static uint8_t pwm_cycle = 0;

    PORT_COL = 0;
    _delay_us(1); //Dont work without small delay. Problem in optimization?

    bit_bang(current_row);
    current_row = ror(current_row);

    uint8_t i;
    uint8_t t = 1;
    uint8_t data = 0;


    for (i = 0; i < 8; i++){
        if (pwm_matrix_buffer[row][i] > pwm_cycle) data += t;
        t = t << 1;

    }

    PORT_COL = data;

    row++;
    row &= 0x07;

    if (!row){
        pwm_cycle++;
        pwm_cycle &= 0x0F;
    }
}

ISR(TIMER1_COMPA_vect){
    time++;
}

void shift_right(){
    uint8_t u,i;

    for (i = 0; i < 8; i++) {
        carry[i] =  pwm_matrix_buffer[i][7];
        for (u = 7; u > 0; u--)
            pwm_matrix_buffer[i][u] =  pwm_matrix_buffer[i][u - 1];
    }
}

void shift_left(){
    uint8_t u,i;

    for (i = 0; i < 8; i++) {
        carry[i] =  pwm_matrix_buffer[i][0];
        for (u = 0; u < 7; u++)
            pwm_matrix_buffer[i][u] =  pwm_matrix_buffer[i][u + 1];
    }
}

void shift_up(){
    uint8_t u,i;

    for (i = 0; i < 8; i++) {
        carry[i] =  pwm_matrix_buffer[7][i];
        for (u = 7; u > 0; u--)
            pwm_matrix_buffer[u][i] =  pwm_matrix_buffer[u - 1][i];
    }
}

void shift_down(){
    uint8_t u,i;

    for (i = 0; i < 8; i++) {
        carry[i] =  pwm_matrix_buffer[0][i];
        for (u = 0; u < 7; u++)
            pwm_matrix_buffer[u][i] =  pwm_matrix_buffer[u + 1][i];
    }
}

void set_right(uint8_t *data){
    uint8_t i;
    for (i = 0; i < 8; i++)
        pwm_matrix_buffer[i][7] = data[i] ;
}

void set_left(uint8_t *data){
    uint8_t i;
    for (i = 0; i < 8; i++)
        pwm_matrix_buffer[i][0] = data[i] ;
}

void set_up(uint8_t *data){
    uint8_t i;
    for (i = 0; i < 8; i++)
        pwm_matrix_buffer[7][i] = data[i] ;
}

void set_down(uint8_t *data){
    uint8_t i;
    for (i = 0; i < 8; i++)
        pwm_matrix_buffer[0][i] = data[i] ;
}
