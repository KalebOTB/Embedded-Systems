# Multiplexer Cycling for Liquid Level Sensing

This is a very simple design allowing us to monitor multiple liquid level sensors. Using a mutliplexer reduces the number of pins necessary from the microcontroller saving us pins for alternative functionalities.

#### Basic Understandings
- [GPIO](https://en.wikipedia.org/wiki/General-purpose_input/output)

### Hardware

#### Part Selection

- Texas Instruments [SN74HC151N](https://www.ti.com/product/SN74HC151/part-details/SN74HC151N) 8-Line To 1-Line Data Selector/Multiplexer,

- Texas Instruments [MSP430FR2476](https://www.ti.com/product/MSP430FR2476/part-details/MSP430FR2476TRHAR) Mixed-Signal Microcontroller

- [Liquid Level Sensor](https://www.mouser.com/ProductDetail/Littelfuse/59630-1-T-05-A?qs=sGAEpiMZZMs29kr3d%252BndI%252Bz%252B5qY7%2FswXh13rrbQA2SA%3D) Float Switch (x8)[^1]
- 10k Ohm Pull-down Resistor (x8)

#### Necessary Connections
![Hardware connections image...](https://github.com/KalebOTB/Embedded-Systems/blob/main/Multiplexer_Cycling_for_Liquid_Level_Sensing/Circuit.png)

### Software
Full example code can be found [here](https://github.com/KalebOTB/Embedded-Systems/blob/main/Multiplexer_Cycling_for_Liquid_Level_Sensing/main.c)
#### Port Configurations
```
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
```

#### Multiplexer Cycling
This function iterates over each possible multiplexer selection, recording the value of each sensor, 0 (open switch) or 1 (closed switch). 8-bits are returned denoting the results of the cycle.
```
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
```
[^1]: Suitable alternatives can be found cheaper on sites like Amazon
