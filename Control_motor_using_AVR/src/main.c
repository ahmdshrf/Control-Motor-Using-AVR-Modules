#include "common_macros.h"
#include "micro_config.h"
#include "std_types.h"
#include "ADC.h"
#include "LCD.h"

#include <avr/interrupt.h>

unsigned char g_Interrupt_Flag = 0;

/* External INT1 Interrupt Service Routine */
ISR(INT1_vect)
{
	g_Interrupt_Flag = 1;
}

/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	SREG  &= ~(1<<7);      // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin
	PORTD |= (1<<PD3);     // Enable the internal pull up resistor at PD3 pin
	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1
	// Trigger INT1 with the falling edge
	MCUCR |= (1<<ISC11);
	MCUCR &= ~(1<<ISC10);
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}

void PWM_Timer0_Init(unsigned char set_duty_cycle){

	TCNT0 = 0; //initial timer value

	OCR0  = set_duty_cycle;

	DDRB  = DDRB | (1<<PB3); //set OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}



int main(void)
{
	uint16 res_value;
	ADC_init();
	LCD_init();
	LCD_clearScreen();
	INT1_Init();
	LCD_displayString("PWM Value = ");
	DDRD = DDRD | (1<<PD0);
	DDRD = DDRD | (1<<PD1);
	PORTD = PORTD & (~(1<<PD0));
    PORTD = PORTD | (1<<PD1);
	while(1)
	{
		LCD_goToRowColumn(0,12);
		res_value = ADC_readChannel(0);
		res_value /= 4;
		LCD_intgerToString(res_value);
		PWM_Timer0_Init(res_value);
	    if(g_Interrupt_Flag == 1)
	    {
	    	PORTD ^= (1<<PD1);
	    	PORTD ^= (1<<PD0);
	    }
	    g_Interrupt_Flag = 0;
	}
}
