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
/////// 			Setup Timer A0 for PWM							/////////
/////////////////////////////////////////////////////////////////////////////

    P1DIR |= BIT2+BIT3+BIT4;             // Set P1.2, 1.3, 1.4 as output
    P1SEL |= BIT2+BIT3+BIT4;             // Set P1.2, 1.3, 1.4 to output 1,2,3 of timer A0
    TA0CCR0 = 20-1;                     // PWM Period
    TA0CCTL1 = OUTMOD_7;                 // CCR1 reset/set
    TA0CCR1 = 9;                        // CCR1 PWM duty cycle
    TA0CCR2 = 9;                        // CCR2 PWM duty cycle
    TA0CCR3 = 9;                        // CCR3 PWM duty cycle
    TA0CTL = TASSEL_2 + TACLR + MC_1;    // SMCLK, clear TAR, Up Mode

 /////////////////////////////////////////////////////////////////////////////
 /////// 			Setup Timer B0 for LED Selection				 /////////
 /////////////////////////////////////////////////////////////////////////////

    TB0CCR0 = 500 - 1;					// PWM Period
    TB0CCTL2 = OUTMOD_7;				// CCR2 reset/set
    TB0CCR1 = 100 - 1;					// Trigger for ADC
    TB0CCR2 = 200 - 1;					// PWM Duty Cycle
    TB0CTL = TBIE + MC_1;				// Enable interrupt on Timer B, Up Mode

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable global interrupts
    __no_operation();                         // For debugger
    return (0);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) TIMER0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	TA0CTL &= MC_1;						 // Turn on TA0
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(TB0IV, 14))
	{
		case  0: break;                          // No interrupt
	    case  2: break;                          // CCR1 not used
	    case  4:
	    	TA0CTL &= MC_0;						 // Turn off TA0
	    	TA0CCTL1 = OUTMOD_0;				 // Turn off TA0CCR1
	    	TB0CCTL2 = OUTMOD_0;				 // Turn off TB0CCR2
	    	TA0CCTL2 = OUTMOD_7;				 // Turn on TA0CCR2
	    	TB0CCTL3 = OUTMOD_7;				 // Turn on TB0CCR3
	    	break;
	    case  6:
	    	TA0CTL &= MC_0;						 // Turn off TA0
	    	TA0CCTL2 = OUTMOD_0;				 // Turn off TA0CCR2
	    	TB0CCTL3 = OUTMOD_0;				 // Turn off TB0CCR3
	    	TA0CCTL3 = OUTMOD_7;				 // Turn on TA0CCR3
	    	TB0CCTL4 = OUTMOD_7;				 // Turn on TB0CCR4
	    	break;
	    case  8:
	    	TA0CTL &= MC_0;						 // Turn off TA0
	    	TA0CCTL3 = OUTMOD_0;				 // Turn off TA0CCR3
	    	TB0CCTL4 = OUTMOD_0;				 // Turn off TB0CCR4
	    	TB0CCTL5 = OUTMOD_7;				 // Turn on TB0CCR5
	    	break;
	    case 10:
	    	TB0CCTL5 = OUTMOD_0;				 // Turn off TB0CCR5
	    	TB0CCTL2 = OUTMOD_7;				 // Turn on TB0CCR2
	    	TA0CCTL1 = OUTMOD_7;				 // Turn on TA0CCR1
	    	break;
	    case 12: break;                          // reserved
	    case 14: break;
	    default: break;
	}
}

