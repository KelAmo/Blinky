//*****************************************************************************
//
//	Task_Blinky.c - Blink the LED and display number of SysTicks
//
//		Author: 		Gary J. Minden
//		Organization:	KU/EECS/EECS 388
//		Date:			2012-08-23
//		Version:		1.0
//
//		Purpose:		Example program that demonstrates:
//							(1) setting up the SysTick Timer and interrupt
//							(2) setting up a GPIO pin (LEC)
//							(3) setting up the OLED display
//							(4) drawing on the OLED
//							(5) blinking (toggling) a LED
//							(6) formatting and displaying a number on the OLED
//

//		Notes:			Updated at KU and adapted from the TI LM3S1968 blinky
//						and other examples.
//
//*****************************************************************************
//

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
//#include "inc/lm3s1968.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "drivers/rit128x96x4.h"

#include "stdio.h"

//*****************************************************************************
//
// The speed of the processor clock in Hertz, which is therefore the speed of the
// clock that is fed to the peripherals.
//
//*****************************************************************************
unsigned long g_ulSystemClock;

//*****************************************************************************
//
//	The number of clock cycles between SysTick interrupts. The SysTick interrupt
//	period is 0.1 mS. All events in the application occur at some fraction of
//	this clock rate.
//
//*****************************************************************************
#define SysTickFrequency		10000

//*****************************************************************************
//
// The number of SysTick clock ticks that have occurred.
//
//*****************************************************************************
static unsigned long g_ulTickCount = 0;

//*****************************************************************************
//
// Handles the SysTick interrupt.
//
//*****************************************************************************
void SysTickIntHandler(void) {

    //
    // Increment the tick count.
    //

    g_ulTickCount++;

}

//*****************************************************************************
//
// Task to Blink the on-board LED.
//
//*****************************************************************************

static unsigned long Task0NextExecute = 0;
static unsigned long Task0DeltaExecute = (SysTickFrequency * 500) / 10000;

void Task0_Init() {

    //
    // Enable the GPIO Port G.
    //
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOG );

    //
    // Configure GPIO_G to drive the Status LED.
    //
    GPIOPinTypeGPIOOutput( GPIO_PORTG_BASE, GPIO_PIN_2 );
    GPIOPadConfigSet( GPIO_PORTG_BASE,
    					GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );

    //
	//	The initial time to execute is 50 SysTicks from now.
    //
	Task0NextExecute = g_ulTickCount + Task0DeltaExecute;

}

void Task0_Execute() {

	unsigned long LED_Data = 0;

	if ( g_ulTickCount >= Task0NextExecute ) {

        //
        // Toggle the LED.
        //
		LED_Data = GPIOPinRead( GPIO_PORTG_BASE, GPIO_PIN_2 );
		LED_Data = LED_Data ^ 0x04;
        GPIOPinWrite( GPIO_PORTG_BASE, GPIO_PIN_2, LED_Data );

        //
        //	Advance Task0NextExecute time.
        //

        Task0NextExecute += Task0DeltaExecute;

	}
}

//*****************************************************************************
//
// Task to update the OLED display.
//
//*****************************************************************************

static unsigned long Task1NextExecute = 0;
static unsigned long Task1DeltaExecute = (SysTickFrequency * 5000) / 10000;

void Task1_Init() {

    //
    // Initialize the OLED display and write status.
    //
    RIT128x96x4Init(1000000);
    RIT128x96x4StringDraw("Blinky",            36,  0, 8);

    //
	//	The initial time to execute is Delta SysTicks from now.
    //
	Task1NextExecute = g_ulTickCount + Task1DeltaExecute;

}

void Task1_Execute() {

	char	TimeString[24];
	int		Task1_Status;

	if ( g_ulTickCount >= Task1NextExecute ) {
		Task1_Status = sprintf( TimeString, "Task_Blinky: %d", g_ulTickCount );
		Task1_Status = Task1_Status;

	    RIT128x96x4StringDraw( TimeString, 16,  0, 15);

        //
        //	Advance Task0NextExecute time.
        //

		Task1NextExecute += Task1DeltaExecute;

	}
}

//*****************************************************************************
//
// Task to draw an image on the OLED display.
//
//*****************************************************************************

static unsigned long Task2NextExecute = 0;
static unsigned long Task2DeltaExecute = (SysTickFrequency * 5000) / 10000;
static unsigned char Task2_Image_01[] = { 0xF0, 0xF0, 0xF0, 0xF0,
											0xF0, 0xF0, 0xF0, 0xF0 };

void Task2_Init() {

	//	We assume the OLED display has been enabled.

	RIT128x96x4ImageDraw( Task2_Image_01, 40, 48, 16, 1 );

}
//*****************************************************************************
//
//	Main program to initialize hardware and execute Tasks.
//
//*****************************************************************************

int main( void )  {
    volatile unsigned long ulLoop;

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ );

	//
	// Get the system clock speed.
	//
	g_ulSystemClock = SysCtlClockGet();

    //
    // Configure SysTick to periodically interrupt.
    //
    SysTickPeriodSet( g_ulSystemClock / SysTickFrequency );
    SysTickIntEnable();
    SysTickEnable();

    //
    //	Initialize Tasks
    //

    Task0_Init();
    Task1_Init();
    Task2_Init();

    //
    //	Execute Tasks
    //

    while ( 1 ) {
    	Task0_Execute( );
    	Task1_Execute( );
    }

}
