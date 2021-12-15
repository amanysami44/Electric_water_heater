/*
 * GccApplication1.c
 * Created: 8/30/2021 10:54:58 AM
 * Author : Amany sami
 */ 




/*_HARDWARE_CONSTRAINS->_CONNECT_THE_7_SEGMENT_TO_TEST_THIS_CODE_*/


/*_THIS_CODE_IS_COUNTING_UP_A_NUMBER_*/

/*_WHEN_YOU_DISCONNECT_THE_POWER_FROM_KIT_AND_RECONNECT_IT_THE_KIT_SHOULD_COUNTS_STARTING_FROM_THE_LAST_NUMBER_BEFORE_POWERING_OFF_*/



#include "LED.h"
#include "P_B.h"
#include "S_SEG.h"
#include "EX_INT.h"
#include "TEMP_S.h"
#include "REG.h"
#include "TIMER.h"
#include "WDT.h"
#include "EEPROM_24C16.h"
#include "DIO.h"
#include "Heating_element.h"
#include "cooling_element.h"
#include <avr/interrupt.h>
#define  F_CPU 16000000UL
#include "util/delay.h"


#define display_mode 1
#define setting_mode 0
/**********/////////////////*/*/////////////////////////*
extern u32 num_of_ov;
extern u32 initial_value;
/***********************************/
u8 tens =0;  // ones and tens of the temp value that displayed 
u8 ones =0;  // on the seven segment 

u8 mode_selector=1 ;  // select which mode is on  display or setting 

u8 counter =0;   //on / off counter (odd/even repersent on and off)

u8 task_timer_slector =1; // select which timer is on in the task 



int temp =0;  // temp from temp sensor 
u8 average_temp=0; // average vaLUe of 10 temp values 
u8 Desired_temp;


int main(void)
{    
	/**********************INITAIZATION ****************/
	LED_INIT(M_LED);
	P_B_INIT(P_B_0);  // up button 
	P_B_INIT(P_B_1); // down button 
	P_B_INIT(P_B_2); //on /off push button 
	TEMP_S_INIT();
	
	Heating_element_init();
	cooling_element_init();
	S_SEG_INIT();
	EEPROM_24C16_INIT();
	
	TIMER_0_INIT();
	TIMER_0_SET_TIME(10);
	TIMER_0_START();
	
	TIMER_1_INIT();
	TIMER_1_SET_TIME(100);
	TIMER_1_START();
	
	u8 d ;
	//to make sure loop executed only one time 
	u8 mode_A=0;  // a for display 
	u8 mode_B=0;  // for setting 
	
	u32 converted_temp_count =0 ;// pointer to array of 10 vaLUes read by sensor 
	u32 temp_arr[10]; //array of the last 10 values of temp that read by sensor 
	u32 temp_sum=0;  //sum of the 10 read values 
	
u8  EEROM_data_add;
	
	if(EEPROM_24C16_READ(0) != 0xff)
	{
	
	EEROM_data_add=0x00; //address of the byte the desired temp stored in 
	Desired_temp=EEPROM_24C16_READ(EEROM_data_add); // the data stored in the eeeprom 
	}
	
	
	// set pins of heating /cooling pins 
        Heating_element_OFF();
		cooling_element_OFF();
		LED_OFF(M_LED);
		
	while(1)
    {
		counter= P_B_READ(P_B_2)==1? ++counter :counter ;  
		if (counter %2 ) // odd value (on mode) 
		{
			if(mode_selector==display_mode) //display mode on 
			{
				if (mode_A ==0) // loop executed only 1 time
				{
					mode_A=1;
					mode_B=0;
					task_timer_slector=1;
					
					TIMER_0_START();
				} 
				temp=TEMP_S_READ(); // storing temp readings from sensor 
				_delay_ms(100);   // waiting untill reading temp is done 
			   
			   temp_arr[converted_temp_count]=temp; //storing temp that read in array 
			   converted_temp_count ++;
			   
			   if(converted_temp_count>=10)
			   {
				   converted_temp_count=9;  //back one place to get the last reading 
			   
			   
			   u8 i;
			   for(i=0;i<10 ;i++)
			   {
				   temp_sum=temp_sum+temp_arr[i];
			   }
			   
			   average_temp=(u8)(temp_sum/10);  //average of 10 readings 
			   //loop to swap the array elements 
			   for (d=0 ; d<9 ; d++)
			   {
				   temp_arr[d]=temp_arr[d+1];
			   }
			
			   }
			if((Desired_temp-5)>=average_temp)
			{
				TIMER_1_START();
				cooling_element_OFF();
				Heating_element_ON(); // heating element on 
				while(PIN_READ(PC4)==1) //led blinking 
				{
					LED_TOGGLE(M_LED);
					_delay_ms(1000);
				}
			}
			else if((Desired_temp+5)<=average_temp)
			{
			TIMER_1_STOP();
			cooling_element_ON(); //cooling element on
			Heating_element_OFF();
			LED_ON(M_LED);
			}
			else
			{
				TIMER_1_STOP();
				cooling_element_OFF();
				Heating_element_OFF();
				LED_OFF(M_LED);
			}
			/***** switch to setting mode ***/
			if(P_B_READ(P_B_2) | P_B_READ(P_B_1))
			{
				mode_selector=setting_mode;
			}
				}
		else
		{
			if(mode_B==0) // exeucted one time (setting )
			{
				mode_A=0;
				mode_B=1;
				
				task_timer_slector=0;
				TIMER_1_START();
			/*turn off led  , heater and cooler*/	
				LED_OFF(M_LED);
				cooling_element_OFF();
				Heating_element_OFF();
				
			}
			ones=Desired_temp % 10;
			tens=Desired_temp / 10;
			
			if(P_B_READ(P_B_0)) //up button to increase temp 
			{
				
				Desired_temp=(Desired_temp+5)<=75 ? Desired_temp+5 :Desired_temp;
				ones=Desired_temp % 10;
				tens=Desired_temp /10 ;
				EEPROM_24C16_WRITE(EEROM_data_add,Desired_temp);
			}
			else if(P_B_READ(P_B_1)) //DOWN button to DEcrease temp
			{
			
				Desired_temp=(Desired_temp-5)<=35 ? Desired_temp-5 :Desired_temp;
				ones=Desired_temp % 10;
				tens=Desired_temp /10 ;
				EEPROM_24C16_WRITE(EEROM_data_add,Desired_temp);
			}
			
		}
			}
			else  // even value (off mode)
			/* defualt values */
			mode_selector=1;
			//turn off timers 
			TIMER_0_STOP();
			TIMER_1_STOP();
			
			//turn of s7 seg by reinitialize it
			S_SEG_INIT();
			
			//turnoff leds
			LED_OFF(M_LED);
			
			
			/*trun off heater , cooler */
			PIN_WRITE(PC3,LOW);
			PIN_WRITE(PC4,LOW);
			
			
		
		
	}
}

ISR(TIMER1_OVF_vect)
{
	static u32 counter1 ;
	static u32 counter2 ;
	static u32 counter3 ;
	counter1++;
	if (counter1 == num_of_ov)
	{    counter2++;
		if((task_timer_slector ==0) && (counter2%100==0))
		{    S_SEG_DISPLAY(Desired_temp);
			 S_SEG_TOGGLE();
			counter3++;
		}
		else if (counter3==5)
		{
			S_SEG_DISPLAY(average_temp);
			task_timer_slector=1;
		}
		
		counter1 = 0;
		counter2 =0;
		counter3=0;
		TCNT1 = initial_value;
	}
}


