// ATmega8_Servo_control_WORKS_with_custom_50Hz_output_signal.c

/*******************************
 *       CURRENT version       *
 *                             *
 * Device: ATmega8             *
 *                             *
 * Pin configuration:          *
 *   @used:                    *
 *   - Button 1:   PB7         *
 *   - Potmeter 1: PC0 (ch0)   *
 *   - Servo:      PB1 (OC1A)  *
 *   @planned                  *
 *   - Button 2:   P__         *
 *   - Potmeter 2: PC_ (ch_)   *
 *   - LED1-green: P__         *
 *   - LED1-red:   P__         *
 *                             *
 *                             *
 *                             *
 *     Boor Andras  @ 2016     *
 ******************************/

#define F_CPU 1000000

#include <util/delay.h>

////////////////////////////////     PORT DEFINITIONS    ////////////////////////////////
#define PORT_OF_BUTTONS          PINB
// @Testing only
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

	float val = (float) SERVO_OUTPUT_MIN;
	#define STEP 1.0f
	unsigned char direction;

    while(1){
        
        adc = (readADC(0));

        if( ((PINB >> 7) & 0b1) ){
			
			setServoPosition(val);
			OUTPUT_PORT = getValueToShowPosition(val-SERVO_OUTPUT_MIN, SERVO_OUTPUT_MAX-SERVO_OUTPUT_MIN);
			_delay_ms(30);
            
            val+= STEP;
            if(val >= SERVO_OUTPUT_MAX){
                PORTB |= 0b100;
                OUTPUT_PORT = 0xFF;
                val = SERVO_OUTPUT_MIN;
                
                for(int i=0; i<20; i++)
                    _delay_ms(1000);
                PORTB &= ~0b100;
                setServoPosition(val);
                
                PORTB |= 0b1;
                for(int i=0; i<20; i++)
                    _delay_ms(1000);
                PORTB &= ~0b1;
            }

			/*
            OUTPUT_PORT = getValueToShowPosition(adc, 1023);
			/**/
            
        }else{     
            PORTD = 255;
            //OUTPUT_PORT = getValueToShowPosition(adc, 1023);
            setServoPosition(  ((1023-adc) * 1.9f) + 450 );
		}

    }

    return 0;

}