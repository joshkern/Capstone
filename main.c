#include <msp430.h>

int main(void) {

	WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

/////////////////////////////////////////////////////////////////////////////
/////// 			CLOCK STUFF										/////////
/////////////////////////////////////////////////////////////////////////////

    UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
    __bis_SR_register(SCG0);                  // Disable the FLL control loop

    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_4;                      // Select DCO range 1.3 - 3.2MHz operation
    UCSCTL2 = 59;                   		  // Set DCO Multiplier for 2MHz
                                              // (N + 1) * FLLRef = Fdco
                                              // (59 + 1) * 32768 = 2MHz
    UCSCTL4 = UCSCTL4 + SELS__DCOCLK;		  // Set source to SMCLK as DCOCLK
    __bic_SR_register(SCG0);                  // Enable the FLL control loop

/////////////////////////////////////////////////////////////////////////////
/////// 			Setup Timer A1 for PWM							/////////
/////////////////////////////////////////////////////////////////////////////

    P2DIR |= BIT0;                       // P2.0 output
    P2SEL |= BIT0;                       // P2.0 options select
    TA1CCR0 = 100-1;                     // PWM Period
    TA1CCTL1 = OUTMOD_7;                 // CCR1 reset/set
    TA1CCR1 = 49;                        // CCR1 PWM duty cycle
    TA1CTL = TASSEL_2 + MC_1 + TACLR;    // SMCLK, up mode, clear TAR

 /////////////////////////////////////////////////////////////////////////////
 /////// 			Setup Timer A0 for LED Selection				 /////////
 /////////////////////////////////////////////////////////////////////////////

    TA0CCR0 =

    __bis_SR_register(LPM0_bits);             // Enter LPM0
    __no_operation();                         // For debugger
    return (0);
}
