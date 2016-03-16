// ATmega8_Servo_control_WORKS_with_custom_50Hz_output_signal.c

/*************************
 *    CURRENT version    *
 ************************/

#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>

#define SEC_IN_US      1000000

////////////////////////////////     PORT DEFINITIONS    ////////////////////////////////
#define OUTPUT_PORT              PORTD
#define SERVO_PORT               PORTD


////////////////////////////////    PIN  DEFINITIONS    ////////////////////////////////
#define SERVO_PIN                  PD0
#define FEEDBACK_PIN1              PD3
#define FEEDBACK_PIN2              PD4

//#define SIGN_COUNT                  50
#define SIGN_COUNT                  1

////////////////////////////////        OTHERS         ////////////////////////////////
#define DEFAULT_DELAY              100
#define SERVO_PWM_WIDTH_IN_US     2000


void wait(unsigned short val) {

    while(val--) _delay_ms(1);

}

void _wait(){
    _delay_ms(DEFAULT_DELAY);
}

void wait_us(double val){
    while(val--) _delay_us(1);
}


/*
 *    "ProgressBar" on portD
 */
void showByteValueOnBar(unsigned int val){
    
    if(val > 16){
        PORTD =         0b1;
    }
    if(val > 48){
        PORTD |=       0b10;    
    }
    if(val > 80){
        PORTD |=      0b100;
    }
    if(val > 112){
        PORTD |=     0b1000;
    }
    if(val > 144){
        PORTD |=    0b10000;
    }
    if(val > 176){
        PORTD |=   0b100000;
    }
    if(val > 208){
        PORTD |=  0b1000000;
    }
    if(val > 240){
        PORTD |= 0b10000000;
    }

}


void init(){   // 76543210
    DDRB  = 0b11111111;
    DDRC  =  0b0000000;
    DDRD  = 0b11111111;
          
    PORTC = 0;
    PORTD = 0x01;
          
    //DDRB = (1<<PB0)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7);
}


/*==========================================================================*
 *                 ADC (10bit) (tested in channel 0 and channel 5)          *
 *                             Tested on ATmega8                            *
 *          DDRC seems doesn't matter (it works when DDRC = 0b11101111)     *
 *                             Boor Andras - 2014                           *
 *==========================================================================*/
void initADC() {
    //         76543210
    ADMUX  = 0b01000000;
    ADCSRA = 0b10000111;
} 


uint16_t readADC(uint8_t ch) {

    ADMUX = (ADMUX & 0b11110000) | ch;
//  01000001    ADMUX jelenlegi állapota (1-es ADC csatorna beállítva)
//& 11110000    ezzel toroljuk az elozo portok kivalasztast
//= 01000000    eredmenynek kapjuk az eredeti ADMUX beallitast
//| 00000101    "rairjuk" az ADC csatornat
//= 01000101    
    ADCSRA|=(1<<ADSC);          // konverzió elindítás
    while(ADCSRA & (1<<ADSC));  // várás az átalakításra

    return (ADCL | (ADCH<<8));  // adc érték visszaadása
}


double calculateServoVal(int degree){
    //     -90° => 1.0 ms
    //       0° => 1.5 ms
    //  +90° => 2.0 ms

    double usVal = (((degree + 90)/90) * 500) + 1000;
    return usVal;

}


void setServoDegree(int degree){
    double servoVal = calculateServoVal(degree);
    int i;
    for(i=0; i<2000; i++){
        
        SERVO_PORT |= (1<<SERVO_PIN);
        wait_us(servoVal);
        
        SERVO_PORT &= ~(1<<SERVO_PIN);    
        double freeTime = SERVO_PWM_WIDTH_IN_US - servoVal;
        wait_us(freeTime); 

    }    
}


void feedback(unsigned char state){
    
    //RM if(state == 1 || state == 3){
    if(state & 0b01){
        OUTPUT_PORT |=  (1<<FEEDBACK_PIN1);
    }else{
        OUTPUT_PORT &= ~(1<<FEEDBACK_PIN1);
    }

    //RM if(state == 2 || state == 3){
    if(state & 0b10){
        OUTPUT_PORT |=  (1<<FEEDBACK_PIN2);
    }else{
        OUTPUT_PORT &= ~(1<<FEEDBACK_PIN2);
    }

}


void generateOutputForFrequencyMeasurement(double freqencyInHz, float pct){
    
    float fillFactor = pct / 100;
    if(fillFactor > 1){
        fillFactor = 1;
    }
    if(fillFactor < 0){
        fillFactor = 0;
    }
    double tCycle = (1 / freqencyInHz) * SEC_IN_US;
    double tOnInUs  = tCycle * fillFactor;
    double tOffInUs = tCycle * (1-fillFactor);
    //double tOffInUs = tOnInUs;
      //double tOffInUs = (tCycle - tOnInUs);

    int i;
    for(i = 0; i < SIGN_COUNT; i++){

        SERVO_PORT |=  (1<<SERVO_PIN);
        _delay_us(tOnInUs);

        SERVO_PORT &= ~(1<<SERVO_PIN);
        _delay_us(tOffInUs);

    }

}


int main(){

    init();
    initADC();

    int degree = 0;
    uint16_t adc;
    unsigned char counter = 0;

    float j;

    while(1){
        
        feedback(1);

        adc = 255 - (readADC(5) / 4);
        PORTB = adc & 0b11111111;
        
        j = (((adc / 8)*0.25) + 0.37);
        generateOutputForFrequencyMeasurement(6, j);      // j -> 2

        //for(j = 0.5; j<7; j+= 0.5){  // 7*
            //generateOutputForFrequencyMeasurement(6, j);      // j -> 2


            /*
            counter++;
            if(counter >= 254){
                counter = 0;
            }
            PORTB = counter & 0b11111111;
            /**/
        //}      

        /*
        
        for(j = 1; j<0.5; j-= 1){
            generateOutputForFrequencyMeasurement(6, j);
        }
        
        _delay_ms(2000);
        */

        /*
        adc = readADC(0);
        adc = (adc / 4);

        showByteValueOnBar(adc);        
        
        setServoDegree(adc);
        /**/


        /*
        degree = -89;
        setServoDegree(degree);

        degree = 0;
        setServoDegree(degree);

        degree = 89;
        setServoDegree(degree);
        /**/
    }

    return 0;

}
