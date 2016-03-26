/**********************************************************************
 *    Read ADC value and show it on PORTx as a LED bar                *
 *    Device: ATmega8                                                 *
 *                                                                    *
 *    Tested with ADC channels:                                       *
 *      + Channel 0 (10 bit) - OK                                     *
 *      + Channel 3 (10 bit) - OK                                     *
 *      + Channel 4 (10 bit) - OK                                     *
 *      + Channel 5 (10 bit) - OK                                     *
 *                                                                    *
 *    DDRC seems doesn't matter (it works when DDRC = 0b11101111)     *
 *                                                                    *
 *                      Boor Andras - 2014                            *
 *                                                                    *
 *    https://www.youtube.com/watch?v=1EOWUTbraFo                     *
 *********************************************************************/

//#define F_CPU 1000000

#include <avr/io.h>

void initADC() {

    //Set ADC ports as input port
    //        76543210
    DDRC &= 0b11000000;

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