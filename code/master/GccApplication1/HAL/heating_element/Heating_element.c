/*
 * Heating_element.c
 *
 * Created: 12/15/2021 9:54:53 PM
 *  Author: MBR
 */ 
#include "Heating_element.h"
#include "DIO.h"

void Heating_element_init(void){
	PIN_MODE(Heating_element_pin,OUTPUT);
	
}
void Heating_element_ON(void){
	PIN_WRITE(Heating_element_pin,HIGH);
}
void Heating_element_OFF(void){
	PIN_WRITE(Heating_element_pin,LOW);
}