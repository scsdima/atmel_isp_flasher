#ifndef ___DELAY_H
    #define ___DELAY_H

    unsigned int _delay_timer;

    #define __delayMs(X)	_delay_timer=(X);	while(_delay_timer) CLRWDT();

    #define DELAY_MACRO 	if(_delay_timer>0)  _delay_timer--;

#endif