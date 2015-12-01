#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"

#ifdef DEBUG
void__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

int main(void)
{
	uint32_t ui32ADC0Value[4];

	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

	//Use sequencer 2
	ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 2);
	while(1)
	{
		ADCIntClear(ADC0_BASE, 2);
		ADCProcessorTrigger(ADC0_BASE, 2);
		while(!ADCIntStatus(ADC0_BASE, 2, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC *	9) + 160) /	5;

		//Turn the LED on if the temperature is above 80F
		if(ui32TempValueF > 80)
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
		}
	}
}


