#ifndef _MAIN_H_
#define _MAIN_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define sbi(str,bit) (str |= (1<<bit)) // set bit in register
#define cbi(str,bit) (str &= ~(1<<bit)) // clear bit in register
#define tbi(str,bit) (str ^= (1<<bit)) // toggle bit in register
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit)) // kiem tra bit = 1
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit))) // kiem tra bi = 0
#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit)) // lap cho den khi bit = 1
#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit)) // lap cho den khi bit = 0

#endif