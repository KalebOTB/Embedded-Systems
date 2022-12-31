#include <msp430fr2476.h>

char cycle_selection_outputs();
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    //Port Configurations
    //Set GPIO pins as outputs
    P2DIR |= (BIT0 | BIT1 | BIT2);
    P4DIR |= (BIT5);
    //Set GPIO pins as inputs
    P4DIR &= ~(BIT1);
    //Set outputs to 0 initially
    P2OUT = 0;
    P4OUT = 0;
    //Set pins to GPIO functionality
    P2SEL0 &= ~(BIT0 | BIT1 | BIT2);
    P2SEL1 &= ~(BIT0 | BIT1 | BIT2);
    P4SEL0 &= ~(BIT1 | BIT5);
    P4SEL1 &= ~(BIT1 | BIT5);

    char output = cycle_selection_outputs();
	return 0;
}

//Selection line cycling to determine if liquid levels are low.
char cycle_selection_outputs(){
    char cycle_sel_out = 0;
    unsigned char i=0;
    P4OUT |= BIT5;  					//Enable multiplexers
    for(i;i<8;i++){ 					//Cycle through selection lines
        P2OUT &= ~(BIT0 | BIT1 | BIT2); 		//Clear selection lines
        P2OUT |= i;    					//Set selection lines with current selection
        cycle_sel_out |= (((P4IN & BIT1) >> 1) << i);   //Checking and storing output
    }
    P4OUT &= ~BIT5; 					//Disable multiplexer
    return cycle_sel_out;
}
