
 /*************************************\
 *        Panorama-photo v1.2          *
 *                                     *
 *  Device: ATmega8                    *
 *          mega8L-8PU - OK            *
 *          mega8-16PU - ??            *
 *  Deploy configuration:              *
 *   SUT_CKSEL: ________               *
 *   ISP freq: 115.2 kHz               *
 *                                     *
 *                                     *
 *  Pin configuration:                 *
 *   @used:                            *
 *   - Button 1:   PD0                 *
 *   - Button 2:   PD1                 *
 *   - Potmeter 1: PC5 (ch5)           *
 *   - Potmeter 2: PC4 (ch4)           *
 *   - Servo:      PB1 (OC1A)          *
 *   - LED1-green: PB7                 *
 *   - LED1-red:   PB6                 *
 *                                     *
 *  Pin configuration @TEST:           *
 *   - Button 1:   PB5                 *
 *   - Button 2:   PB4                 *
 *   - LEDBAR:     PD0-7               *
 *                                     *
 *  Test: WORKS on target board v1.0   *
 *                                     *
 *         Boor Andras  @ 2016         *
 *                                     *
 \*************************************/

//#define TEST
//#define SPIKE
#define TESTBOARD
//#define ENABLE_ROTATE_BACK

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


/////////////////////////////////////     HW DEFINITIONS    /////////////////////////////////////
// LED
#define PORT_OF_LEDS                PORTB
#define LED_PIN_GREEN               7
#define LED_PIN_RED                 6

// BUTTON
#ifndef TESTBOARD
    #define PORT_INPUT_OF_BUTTONS   PIND
    #define PORT_OF_BUTTONS         PORTD
    #define BUTTON_1_PIN            0
    #define BUTTON_2_PIN            1
#else
    #define PORT_INPUT_OF_BUTTONS   PINB
    #define PORT_OF_BUTTONS         PORTB
    #define BUTTON_1_PIN            5
    #define BUTTON_2_PIN            4
#endif
//#define BUTTON_3_PIN              2                                         // inactive in v1

#define BTN_1                       (PORT_INPUT_OF_BUTTONS >> BUTTON_1_PIN)
#define BTN_2                       (PORT_INPUT_OF_BUTTONS >> BUTTON_2_PIN)
//#define PIN_BTN_3                 (PORT_INPUT_OF_BUTTONS >> BUTTON_3_PIN)   // inactive in v1
#define BTN_RIGHT                   BTN_1
#define BTN_LEFT                    BTN_2

// Read button states
#define isPressed(x)                ((x & 0b1) == 0)
#define isReleased(x)               ((x & 0b1) == 1)


// ADC channels of potmeters
#define POT_TEST                    0
#define POT1                        5
#define POT2                        4

// States of feedback LED
#define OFF                         0
#define GREEN                       1
#define RED                         2
#define YELLOW                      3

#ifdef TESTBOARD
    #define LEDBAR                  PORTD
#endif

//////////////////////////////////////        OTHERS         ////////////////////////////////////
#define DEFAULT_DELAY               100

#define POT_MAX_VALUE               1023

#define RIGHT_END_POSITION          SERVO_RIGHT
#define LEFT_END_POSITION           SERVO_LEFT

// Identify direction (of previously pressed button)
#define NONE                        0
#define RIGHT                       1
#define LEFT                        2

#define INC_TO_RIGHT                (-1)
#define INC_TO_LEFT                 (1)


// System wait for BUTTON_RELEASE_DELAY ms before do the next command
// to avoid unwanted re-detection of buttonPress on same button.
#define BUTTON_RELEASE_DELAY        500

// Minimum delay in continous rotation to prevent from to fast rotating
#define MIN_DELAY_FOR_ROTATION      50

// TRY to prevent servo "quake" symptom
#define DELAY_AFTER_SERVO_ROTATION_IN_READY_STATUS 30

// For using tighten servo degree to prevent problems 
// coused by servo's internal limit-detection
#define SERVO_EDGE_THRESHOLD        100

// Used when system is in "ROTATE_BACK" state
// and need to determine when servo almost reached the adjusted initial position
#define SERVO_ROTATE_BACK_THRESHOLD 50

//////////////////////////////////////        STATES         ////////////////////////////////////
// Ready to use. User can set initial direction and speed.
// System is waiting for start input (btn). (Servo continuously follow the setted position.)
#define READY                       1
// Rotation started, but not finished yet.
#define ROTATION_IN_PROGRESS        2
// Rotation finished.System is waiting for user input (btn) to get "ROTATE_BACK" state.
#define ROTATION_DONE               3
// Servo rotating back before get "READY" state.
#define ROTATE_BACK                 4


// Additional information can be displayed on ledbar (in unused io-ports)
// when configuration runs in testboard.
#ifdef TESTBOARD
    void showValueOnLedBar(unsigned short value, unsigned short min, unsigned short max){
        LEDBAR = getValueToShow(
            value - min, 
            max - min
        );
    }
#endif


// Wait function can be used instead of direct usage of "_delay_ms(val)" function
// because of the input range problem of "_dalay_ms()" function.
void wait(unsigned short val) {
    while(val--) _delay_ms(1);
}

void _wait(){
    _delay_ms(DEFAULT_DELAY);
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


// Determine conditions when servo reached any end position
#define isRotationDone() ( ((btnPressedPreviously == RIGHT && (getServoPosition() > RIGHT_END_POSITION + SERVO_EDGE_THRESHOLD)) || (btnPressedPreviously == LEFT && (getServoPosition() < LEFT_END_POSITION - SERVO_EDGE_THRESHOLD)))?0:1 )


// Calculate difference between given values (can not be negative)
unsigned short diff(unsigned short x, unsigned short y){
    return ( ((x)>(y))?((x)-(y)):((y)-(x)) );
}


// Modify servo direction with given value
void rotateServo(short value){
    setServoPosition( getServoPosition() + value );
}


// Calculate delay between iteration steps of rotation
unsigned short calculateDelayOfRotationSteps(uint16_t speedInput){
    return (unsigned short)( MIN_DELAY_FOR_ROTATION + speedInput );
}


// Calculate servoPosition from potmeter input
unsigned short calculateServoPositionFromDirectionInput(uint16_t positionInput){
    return ( ((unsigned short) (POT_MAX_VALUE - positionInput) * 1.9f) + SERVO_OUTPUT_MIN );
}


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

#ifndef SPIKE
    
    uint16_t positionInput = 0, speedInput = 0;
    #ifdef TESTBOARD
    #ifdef TEST
        unsigned short m, n;
    #endif
    #endif


    // Initialize values and feedback according to get READY state..
    unsigned char btnPressedPreviously = NONE;
    unsigned char currentState = READY;
    setLed(GREEN);

    while(1){
        
        if(currentState == READY){ 
            
            // User can set initial direction and speed.
            speedInput =    readADC(POT2);
            if(speedInput > 800){
                speedInput *= 2;
            }
            #ifdef TESTBOARD
                showValueOnLedBar( (unsigned short)speedInput, 0, POT_MAX_VALUE*2);
            #endif
            
            positionInput = readADC(POT1);

            // Show "value-quaking" of positionInput
            #ifdef TESTBOARD
                #ifdef TEST
                    m = ((unsigned short)(positionInput / 10) * 10); // round to
                    n = m + 8;
                    showValueOnLedBar( (unsigned short)positionInput, m, n );
                #endif
            #endif

            // Servo continuously follow the setted position.
            setServoPosition( calculateServoPositionFromDirectionInput(positionInput) );

            // Prevent servo "quake" symptom (seems partly helps)
            //wait(DELAY_AFTER_SERVO_ROTATION_IN_READY_STATUS); // @If PASSED remove after next test

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
                    // button what has been pressed previously AND released now..
                    // btnPressedPreviously variable stores the identifier of button what has been released just now.

                    // Set values and feedback according to get ROTATION_IN_PROGRESS state
                    setLed(RED);
                    currentState = ROTATION_IN_PROGRESS;
                    wait(BUTTON_RELEASE_DELAY);

                }

            }

        } // eof READY state

        else

        if(currentState == ROTATION_IN_PROGRESS){ // Rotation started, but not finished yet.

            // Option for user to interrupt rotation progress
            if( (isPressed(BTN_LEFT)) && (isPressed(BTN_RIGHT)) ){
                setLed(OFF);

                // wait until all button released
                while( (isPressed(BTN_LEFT)) || (isPressed(BTN_RIGHT)) ){
                    wait(BUTTON_RELEASE_DELAY);
                }

                // When all button released get ROTATION_DONE state
                currentState = ROTATION_DONE;
                btnPressedPreviously = NONE;
                setLed(YELLOW);
            }

            else

            if(isRotationDone()){

                // Set values and feedback according to get ROTATION_DONE state
                btnPressedPreviously = NONE;
                currentState = ROTATION_DONE;
                setLed(YELLOW);

            }else{

                if(btnPressedPreviously == RIGHT){
                    
                    #ifdef TESTBOARD
                        showValueOnLedBar( getServoPosition(), SERVO_OUTPUT_MIN, SERVO_OUTPUT_MAX );
                    #endif
                    
                    rotateServo(INC_TO_RIGHT);
                    wait( calculateDelayOfRotationSteps(speedInput) );

                }
                else

                if(btnPressedPreviously == LEFT){
                    
                    #ifdef TESTBOARD
                        showValueOnLedBar( getServoPosition(), SERVO_OUTPUT_MIN, SERVO_OUTPUT_MAX );
                    #endif
                    
                    rotateServo(INC_TO_LEFT);
                    wait( calculateDelayOfRotationSteps(speedInput) );

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
                if( ((btnPressedPreviously == RIGHT) && (isReleased(BTN_RIGHT))) ||
                    ((btnPressedPreviously == LEFT ) && (isReleased(BTN_LEFT ))) ){
                    
                    #ifdef ENABLE_ROTATE_BACK                    
                        // re-read position input before get ROTATE_BACK state..
                        positionInput = readADC(POT1);

                        // Set values and feedback according to get ROTATE_BACK state
                        currentState = ROTATE_BACK;
                        setLed(OFF);
                        btnPressedPreviously = NONE;
                    #endif

                    #ifndef ENABLE_ROTATE_BACK
                        // Set values and feedback according to get ROTATE_BACK state
                        currentState = READY;
                        btnPressedPreviously = NONE;
                        setLed(GREEN);
                    #endif

                    wait(BUTTON_RELEASE_DELAY);

                }

            }

        } // eof ROTATION_DONE

        else

        if(currentState == ROTATE_BACK){

            unsigned short wantedPosition = calculateServoPositionFromDirectionInput(positionInput);
            uint8_t direction;

            // Note: RIGHT < LEFT
                    //2400               1500
            if( getServoPosition() > wantedPosition ){ // need to rotate right (--)
                direction = INC_TO_RIGHT;
            }else{ // need to rotate left (++)
                direction = INC_TO_LEFT;
            }

            short attempsToRotateBack = 0;

            //               900
            unsigned short distance;
            //            900 .. 890 .. 880 ..                            >  50
            while( ((distance = diff(getServoPosition(), wantedPosition))  > SERVO_ROTATE_BACK_THRESHOLD) && (attempsToRotateBack < 300) && (attempsToRotateBack >= 0) ){ // Need to rotate back..

                if(attempsToRotateBack < 10){ // to fine starts
                    rotateServo( direction );
                }else

                if( distance > 400 ){
                    rotateServo( direction * 10 );
                }else
                if( distance > 250 ){
                    rotateServo( direction * 5 );
                }else
                if( distance > 150 ){
                    rotateServo( direction * 2 );
                }else{
                    rotateServo( direction );
                }

                attempsToRotateBack++;

                wait(20);

            }

            // Set values and feedback according to get READY state 
            currentState = READY;
            btnPressedPreviously = NONE;
            setLed(GREEN);

        } // eof ROTATE_BACK

    }

#endif
#ifdef SPIKE

    setLed(YELLOW);
    while(1){

        // TestCode here
        positionInput = readADC(POT1);
        speedInput =    readADC(POT2);

        #ifdef TESTBOARD
            unsigned char output = 0;
            output |= getValueToShow(positionInput / 2, POT_MAX_VALUE/2);
            output |= getValueToShow(speedInput / 2, POT_MAX_VALUE/2) << 4;
            LEDBAR = output;
        #endif

    }

#endif

    return 0;

}

