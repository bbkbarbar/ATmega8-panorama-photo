// ATmega8_Servo_control_WORKS_with_custom_50Hz_output_signal.c

/*******************************
 *       CURRENT version       *
 *                             *
 * Device: ATmega8             *
 *                             *
 * Pin configuration @TEST:    *
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
 * Pin configuration:          *
 *   @used:                    *
 *   - Button 1:   PD0         *
 *   - Button 2:   PD1         *
 *   - Potmeter 1: PC5 (ch5)   *
 *   - Potmeter 2: PC4 (ch4)   *
 *   - Servo:      PB1 (OC1A)  *
 *   - LED1-green: PB6         *
 *   - LED1-red:   PB7         *
 *                             *
 *     Boor Andras  @ 2016     *
 *                             *
 ******************************/

#define TEST
#define TESTBOARD

#define F_CPU 1000000

#include "lib/ATmega8_read_ADC.c"
#include "lib/ATmega8_servo_control.c"
#ifdef TESTBOARD
#include "lib/bar_display.c"
#endif


#ifndef DELAY_INCLUDED
    #include <util/delay.h>
    #define DELAY_INCLUDED
#endif


/////////////////////////////////     HW DEFINITIONS    /////////////////////////////////
//  LED
#define PORT_OF_LEDS            PORTB
#define LED_PIN_GREEN           6
#define LED_PIN_RED             7

//  BUTTON
#ifndef TESTBOARD
    #define PORT_INPUT_OF_BUTTONS   PIND
    #define PORT_OF_BUTTONS         PORTD
    #define BUTTON_1_PIN            0
    #define BUTTON_2_PIN            1
#else
    #define PORT_INPUT_OF_BUTTONS   PINB
    #define PORT_OF_BUTTONS         PORTB
    #define BUTTON_1_PIN            4
    #define BUTTON_2_PIN            5
#endif
//#define BUTTON_3_PIN          2                                   // inactive in v1

//#define BTN_TEST                (PINB >> 7)
#define BTN_1                   (PORT_INPUT_OF_BUTTONS >> BUTTON_1_PIN)
#define BTN_2                   (PORT_INPUT_OF_BUTTONS >> BUTTON_2_PIN)
//#define PIN_BTN_3             (PORT_INPUT_OF_BUTTONS >> BUTTON_3_PIN)   // inactive in v1

// Read button states
#define isPressed(x)             ((x & 0b1) == 0)
#define isReleased(x)            ((x & 0b1) == 1)


// ADC channels of potmeters
#define POT_TEST                0
#define POT1                    5
#define POT2                    4

// States of feedback LED
#define OFF                     0
#define GREEN                   1
#define RED                     2
#define YELLOW                  3

#ifdef TESTBOARD
    #define LEDBAR              PORTD
#endif

//////////////////////////////////        OTHERS         ////////////////////////////////
#define DEFAULT_DELAY           100

#define POT_MAX_VALUE           1023

#define RIGHT_END_POSITION      SERVO_RIGHT
#define LEFT_END_POSITION       SERVO_LEFT

#define NONE                    0
#define RIGHT                   1
#define LEFT                    2
#define BTN_RIGHT               BTN_1
#define BTN_LEFT                BTN_2

// System wait for BUTTON_RELEASE_DELAY ms before do the next command
// to avoid unwanted re-detection of buttonPress on same button.
#define BUTTON_RELEASE_DELAY    100

//////////////////////////////////        STATES         ////////////////////////////////
 // Ready to use. User can set initial direction and speed.
 // System is waiting for start input (btn). (Servo continuously follow the setted position.)
#define READY                   0
 // Rotation started, but not finished yet.
#define ROTATION_IN_PROGRESS    1
 // Rotation finished.System is waiting for user input (btn) to get "READY" state.
#define ROTATION_DONE           2


void wait(unsigned short val) {

    while(val--) _delay_ms(1);

}

void _wait(){
    _delay_ms(DEFAULT_DELAY);
}

void wait_us(double val){
    while(val--) _delay_us(1);
}


// Set color of feedback led (RG)
void setLed(unsigned short color){
    if(color == GREEN){
        PORT_OF_LEDS &= ~(1 << LED_PIN_RED); 
        PORT_OF_LEDS |=  (1 << LED_PIN_GREEN);
    }else
    if(color == RED){
        PORT_OF_LEDS |=  (1 << LED_PIN_RED); 
        PORT_OF_LEDS &= ~(1 << LED_PIN_GREEN);
    }else
    if(color == YELLOW){
        PORT_OF_LEDS |=  (1 << LED_PIN_RED); 
        PORT_OF_LEDS |=  (1 << LED_PIN_GREEN);
    }else{
        PORT_OF_LEDS &= ~(1 << LED_PIN_RED); 
        PORT_OF_LEDS &= ~(1 << LED_PIN_GREEN);
    }
}


#define isRotationDone() ( ((btnPressedPreviously == RIGHT && (getServoPosition() > RIGHT_END_POSITION)) || (btnPressedPreviously == LEFT && (getServoPosition() < LEFT_END_POSITION)))?0:1 )


void init(){   
    
    //                 LED
    // Set LED pins as output
    DDRB |= (1 << LED_PIN_GREEN) 
         |  (1 << LED_PIN_RED);

    //              POTMETERS
    // DDRC set as input for ADC in initADC() function

    //                SERVO
    // Output of servo set as output initServoControl() function
          
    //DDRB = (1<<PB0)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7);

    //               BUTTONS
    // Setup the pull-up resistor for buttons 
    PORT_OF_BUTTONS |= (1 << BUTTON_1_PIN) 
                    |  (1 << BUTTON_2_PIN);
    //PORTB |= 0b10000000; //PB7 pin for TEST

    #ifdef TESTBOARD
        DDRD = 0xFF;
        LEDBAR = 170;
    #endif

}

int main(){

    init();
    initADC();
    initServoControl();
    
    uint16_t positionInput, speedInput;

    unsigned char btnPressedPreviously = 0;

#ifndef TEST

    unsigned char currentState = READY;
    setLed(GREEN);

        //adc1 = (readADC(POT_TEST));
        //if( released(BTN_TEST) ){
        //setServoPosition(val);

    while(1){
        
        if(currentState == READY){ 
            
            // User can set initial direction and speed.
            positionInput = readADC(POT1);
            speedInput =    readADC(POT2);

            // Servo continuously follow the setted position.
            setServoPosition(  ((POT_MAX_VALUE-positionInput) * 1.9f) + SERVO_OUTPUT_MIN );


            // System is waiting for start input (btn). 
            if( btnPressedPreviously == NONE ){

                if(isPressed(BTN_RIGHT)){
                    btnPressedPreviously = RIGHT;
                }else
                if(isPressed(BTN_LEFT)){
                    btnPressedPreviously = LEFT;
                }
                
            }else{ //if any button is pressed previously then need to wait until button is released.

                if( ((btnPressedPreviously == RIGHT) && (isReleased(BTN_RIGHT))) ||
                    ((btnPressedPreviously == LEFT ) && (isReleased(BTN_LEFT ))) ){ 
                    // button what has been pressed previously is released now..
                    // btnPressedPreviously variable stores the identifier of button what has been released just now.
                    setLed(RED);
                    currentState = ROTATION_IN_PROGRESS;
                    wait(BUTTON_RELEASE_DELAY);
                }

            }

        } // eof READY state

        else

        if(currentState == ROTATION_IN_PROGRESS){ // Rotation started, but not finished yet.

            if(isRotationDone()){

                btnPressedPreviously = NONE;
                currentState = ROTATION_DONE;
                setLed(YELLOW);

            }else{
                if(btnPressedPreviously == RIGHT){
                    //TODO
                }
                else
                if(btnPressedPreviously == LEFT){
                    //TODO
                }
            }

        } // eof ROTATION_IN_PROGRESS

        else

        if(currentState == ROTATION_DONE){ // Rotation finished. 

            // System is waiting for user input (button press and release) 
            // before servo returns to initial direction and get "READY" state. 

            // Check that any button is pressed..
            if(btnPressedPreviously == NONE){
                if(isPressed(BTN_RIGHT)){
                    btnPressedPreviously = RIGHT;
                }else
                if(isPressed(BTN_LEFT)){
                    btnPressedPreviously = LEFT;
                }
            }else{
                // Check that previously pressed button is released..
                if( (btnPressedPreviously == RIGHT) && (isReleased(BTN_RIGHT)) ||
                    (btnPressedPreviously == LEFT ) && (isReleased(BTN_LEFT )) ){
                    
                    btnPressedPreviously = NONE;
                    
                    wait(BUTTON_RELEASE_DELAY);

                    currentState = READY;
                    setLed(GREEN);

                }
            }

        } // eof ROTATION_DONE

    }

#endif
#ifdef TEST

    setLed(YELLOW);
    while(1){

        // TestCode here
        positionInput = readADC(POT1);
        speedInput =    readADC(POT2);
        unsigned char output = 0;
        output |= getValueToShow(positionInput / 2, POT_MAX_VALUE/2);
        output |= getValueToShow(speedInput / 2, POT_MAX_VALUE/2) << 4;
        LEDBAR = output;
    }

#endif

    return 0;

}
