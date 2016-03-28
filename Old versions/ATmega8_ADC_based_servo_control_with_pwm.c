
#define F_CPU 1000000

#include <util/delay.h>

////////////////////////////////     PORT DEFINITIONS    ////////////////////////////////
#define OUTPUT_PORT              PORTD

//////////////////////////////////        OTHERS         ////////////////////////////////
#define DEFAULT_DELAY              100

#include "ATmega8_read_ADC.c"
#include "ATmega8_servo_control.c"


void wait(unsigned short val) {

    while(val--) _delay_ms(1);

}

void _wait(){
    _delay_ms(DEFAULT_DELAY);
}

void wait_us(double val){
    while(val--) _delay_us(1);
}


void init(){   // 76543210
    DDRB  = 0b11111111;
    DDRD  = 0b11111111;
          
    PORTC = 0;
    PORTD = 0x01;
          
    //DDRB = (1<<PB0)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7);
}


uint8_t getValueToShow(float val, int max){
    #define BAR_SIZE  8
                       //    142      255  
    float divVal = (max+1) / BAR_SIZE;  // 255 -> 32
    float barLength = val / divVal;     // 4,43

    uint8_t res;
    if( barLength < 0.5 ){
        res = 0b0;
    }else
    if( barLength < 1.5 ){
        res = 0b1;
    }else
    if( barLength < 2.5 ){
        res = 0b11;
    }else
    if( barLength < 3.5 ){
        res = 0b111;
    }else
    if( barLength < 4.5 ){
        res = 0b1111;       
    }else
    if( barLength < 5.5 ){
        res = 0b11111;
    }else
    if( barLength < 6.5 ){
        res = 0b111111;
    }else
    if( barLength < 7.5 ){
        res = 0b1111111;
    }else{
        res = 0b11111111;
    }

return res;
}


int main(){

    init();
    initADC();
	initServoControl();

    uint16_t adc;


    while(1){
        
        adc = (readADC(0));
        OUTPUT_PORT = getValueToShow(adc, 1023);
		setServoPosition(  ((1023-adc) * 1.9f) + 450 );

    }

    return 0;

}
