/*
 * cooling_element.c
 *
 * Created: 12/15/2021 10:03:24 PM
 *  Author: MBR
 */ 
#include "cooling_element.h"
#include "DIO.h"


void cooling_element_init(void){
	PIN_MODE(cooling_element_pin,OUTPUT);
}
void cooling_element_ON(void){
	PIN_WRITE(cooling_element_pin,HIGH);
}
void cooling_element_OFF(void){
	PIN_WRITE(cooling_element_pin,LOW);
}