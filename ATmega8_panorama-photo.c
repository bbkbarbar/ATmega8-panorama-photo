// ATmega8_Servo_control_WORKS_with_custom_50Hz_output_signal.c

/*************************
 *    CURRENT version    *
 ************************/

#define F_CPU 1000000

#include <util/delay.h>

////////////////////////////////     PORT DEFINITIONS    ////////////////////////////////
#define OUTPUT_PORT              PORTD

//////////////////////////////////        OTHERS         ////////////////////////////////
#define DEFAULT_DELAY              100

#include "ATmega8_read_ADC.c"
#include "ATmega8_servo_control.c"
#include "bar_display.c"


void wait(unsigned short val) {

    while(val--) _delay_ms(1);

}

void _wait(){
    _delay_ms(DEFAULT_DELAY);
}

void wait_us(double val){
    while(val--) _delay_us(1);
}


void init(){   
    //        76543210
    DDRB  = 0b01111111;    
    DDRD  = 0b11111111;
          
    PORTC = 0;
    PORTD = 0x01;
          
    //DDRB = (1<<PB0)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7);

    // Setup the pull-up resistor on PB7 pin
    PORTB |= 0b10000000; 
}


int main(){

    init();
    initADC();
    initServoControl();
    

    uint16_t adc;


    while(1){
        
        adc = (readADC(0));

        if( ((PINB >> 7) & 0b1) ){
            OUTPUT_PORT = getValueToShowPosition(adc, 1023);
            continue;
        }        
        
        PORTD = 255;
        //OUTPUT_PORT = getValueToShowPosition(adc, 1023);
        setServoPosition(  ((1023-adc) * 1.9f) + 450 );

    }

    return 0;

}