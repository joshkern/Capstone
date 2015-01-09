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

    P1DIR |= BIT2+BIT3+BIT4;            // Set P1.2, 1.3, 1.4 as output
    P1SEL |= BIT2+BIT3+BIT4;            // Set P1.2, 1.3, 1.4 to output 1,2,3 of timer A0
    TA0CCR0 = 20-1;                     // PWM Period
    TA0CCTL1 = OUTMOD_7;                // CCR1 reset/set
    TA0CCR1 = 9;                        // CCR1 PWM duty cycle
    TA0CCTL2 = OUTMOD_7;                // CCR2 reset/set
    TA0CCR2 = 9;                        // CCR2 PWM duty cycle
    TA0CCTL3 = OUTMOD_7;                // CCR3 reset/set
    TA0CCR3 = 9;                        // CCR3 PWM duty cycle
    TA0CTL = TASSEL_2 + TACLR + MC_1;   // SMCLK, clear TAR, Up Mode

 /////////////////////////////////////////////////////////////////////////////
 /////// 			Setup Timer B0 for LED Selection				 /////////
 /////////////////////////////////////////////////////////////////////////////

    TB0CCR0 = 500 - 1;					// PWM Period
    TB0CCTL0 |= CCIE;				    // CCR2 reset/set
    TB0CCTL2 |= CCIE;				    // CCR2 reset/set
    TB0CCR1 = 100 - 1;					// Trigger for ADC
    TB0CCR2 = 200 - 1;					// PWM Duty Cycle
    TB0CCR3 = 200 - 1;					// PWM Duty Cycle
    TB0CCR4 = 200 - 1;					// PWM Duty Cycle
    TB0CCR5 = 200 - 1;					// PWM Duty Cycle
    TB0CTL =  TASSEL_2 + TBIE + TBCLR +  MC_1;				// Enable interrupt on Timer B, Up Mode

/////////////////////////////////////////////////////////////////////////////
/////// 						Setup ADC12				 			/////////
/////////////////////////////////////////////////////////////////////////////

    volatile unsigned int skin[20];
    volatile unsigned int bone[20];
    P6SEL = 0x03;                             // Enable A/D channel inputs
    ADC12CTL0 = ADC12SHT0_5 + ADC12MSC + ADC12ON; //Sample time is 96 clocks, rising edge trigger, turn on ADC
    ADC12CTL1 = ADC12SHP + ADC12SHS_3 + ADC12SSEL_1 + ADC12CONSEQ_3;       // Use sampling timer, repeated sequence
    ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
    ADC12MCTL1 = ADC12INCH_1 + ADC12EOS;      // ref+=AVcc, channel = A1, end of sequence
    ADC12IE = 0x02;                           // Enable ADC12IFG.1
    ADC12CTL0 |= ADC12ENC;                    // Enable conversions

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable global interrupts
    __no_operation();                         // For debugger
    //return (0);
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
	TA0CCR0 = 20 ;						// Turn on TA0
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
	    case  2:                           		 // CCR1 not used
	    	ADC12CTL0 |= ADC12ON:				 // Turn the ADC back on
	    case  4:
	    	TA0CCR0 = 0;                     // Turn off TA0
	    	TA0CCTL1 = OUTMOD_0;		     // Turn off TA0CCR1
	    	TB0CCTL2 &= ~CCIE;				 // Turn off TB0CCR2
	    	TA0CCTL2 = OUTMOD_7;			 // Turn on TA0CCR2
	    	TB0CCTL3 |= CCIE;				 // Turn on TB0CCR3

	    	// Turn off all the other flags
	    	TB0CCTL3 &= ~CCIFG;
	    	TB0CCTL4 &= ~CCIFG;
	    	TB0CCTL5 &= ~CCIFG;
	    	break;

	    case  6:
	    	TA0CCR0 = 0;                     // Turn off TA0
	    	TA0CCTL2 = OUTMOD_0;			 // Turn off TA0CCR2
	    	TB0CCTL3 &= ~CCIE;				 // Turn off TB0CCR3
	    	TA0CCTL3 = OUTMOD_7;			 // Turn on TA0CCR3
	    	TB0CCTL4 |= CCIE;				 // Turn on TB0CCR4

	    	// Turn off all the other flags
	    	TB0CCTL2 &= ~CCIFG;
	    	TB0CCTL4 &= ~CCIFG;
	    	TB0CCTL5 &= ~CCIFG;
	    	break;

	    case  8:
	    	TA0CCR0 = 0;					 // Turn off TA0
	    	TA0CCTL3 = OUTMOD_0;			 // Turn off TA0CCR3
	    	TB0CCTL4 &= ~CCIE;				 // Turn off TB0CCR4
	    	TB0CCTL5 |= CCIE;				 // Turn on TB0CCR5

	    	// Turn off all the other flags
	    	TB0CCTL3 &= ~CCIFG;
	    	TB0CCTL2 &= ~CCIFG;
	    	TB0CCTL5 &= ~CCIFG;
	    	break;

	    case 10:
	    	TA0CCR0 = 0;					 // Turn off TB0CCR5
	    	TB0CCTL2 |= CCIE;				 // Turn on TB0CCR2
	    	TA0CCTL1 = OUTMOD_7;			 // Turn on TA0CCR1

	    	// Turn off all the other flags
	    	TB0CCTL3 &= ~CCIFG;
	    	TB0CCTL4 &= ~CCIFG;
	    	TB0CCTL2 &= ~CCIFG;
	    	break;

	    case 12: break;                          // reserved
	    case 14: break;
	    default: break;
	}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
#error Compiler not supported!
#endif
{
  static unsigned int index = 0;

  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6: break;                           // Vector  6:  ADC12IFG0
  case  8: 		                            // Vector  8:  ADC12IFG1
	    skin[index] = ADC12MEM0;           // Move A0 results, IFG is cleared
	    bone[index] = ADC12MEM1;           // Move A1 results, IFG is cleared
	    index++;                           // Increment results index, modulo; Set Breakpoint1 here

	    if(index%5 == 0)
	    	ADC12CTL0 &= ~ADC12ON;
	    if (index == 19)
	    {
	      (index = 0);
	    }
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}

