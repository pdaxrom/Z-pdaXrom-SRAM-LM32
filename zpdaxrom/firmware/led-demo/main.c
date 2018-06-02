/**
 * 
 */

#include "soc-hw.h"

void writeint(uint32_t val)
{
	uint32_t i, digit;

	for (i=0; i<8; i++) {
		digit = (val & 0xf0000000) >> 28;
		if (digit >= 0xA) 
			uart_putchar('A'+digit-10);
		else
			uart_putchar('0'+digit);
		val <<= 4;
	}
}

void test2() {
    uart_putchar('b');   
}

void test() {
    uart_putchar('a');
    test2();
    uart_putchar('c');
} 

char glob[] = "Global";

volatile uint32_t *p;
volatile uint8_t *p2;

extern uint32_t tic_msec;

int main()
{
    char test2[] = "Lokalerstr";
    char *str = test2;
    uint32_t i;
    
//    for (i = 0; i < 4; i++)
 //       test2[i] = 'l';
  //  glob[0]  = 'g';
    
 	// Initialize stuff
	uart_init();

	// Say Hello!
	uart_putstr( "** Spike Test Firmware **\n\r" );

	// Initialize TIC
	isr_init();
	tic_init();
	irq_set_mask( 0x00000001 );
	irq_enable();

	// Say Hello!
	uart_putstr( "Timer Interrupt instelled.\n" );

	// Do some trivial tests
	uart_putstr( "Subroutine-Return Test: " );
	test();
	uart_putstr("\n\r");    

	uart_putstr( "Local-Pointer Test:" );
	for (;*str; str++) {
	   uart_putchar(*str);
	}
	uart_putstr("\n\r");    
	
	uart_putstr( "Global-Pointer Test:" );
	str = glob;
	for (;*str; str++) {
	   uart_putchar(*str);
	}
	uart_putstr("\n\r");

	uart_putstr( "Stack Pointer : " );
	writeint(get_sp());
	uart_putstr("\n\r");

	uart_putstr( "Global Pointer: " );
	writeint(get_gp());
	uart_putstr("\n\r");

#if 0
	uart_putstr( "Timer Test (1s): " );
	for(i=0; i<4; i++) {
		uart_putstr("tic...");    
		msleep(1000);
	}
	uart_putstr("\n\r");
#endif

	uart_putstr( "Timer Interrupt counter: " );
	writeint( tic_msec );
	uart_putstr("\n\r");

	int val = tic_msec;
	uart_putstr( "Shift: " );
	writeint( val );
	uart_putstr(" <-> ");    
	for(i=0; i<32; i++) {
		if (val & 0x80000000)
			uart_putchar( '1' );
		else
			uart_putchar( '0' );
			
		val <<= 1;
	}
	uart_putstr("\r\n");

	uart_putstr("GPIO LED test...\n\r");

#if 0
	val = 0x1;
	for (i = 0; i < 32; i++) {
	    gpio0_leds->data = ~val;
	    writeint(i); uart_putstr(" ");
	    msleep(2000);
	    val <<= 1;
	}
#endif 

#if 1
	gpio0_leds->data = ~LED0_R;
	msleep(2000);
	gpio0_leds->data = ~LED0_G;
	msleep(2000);
	gpio0_leds->data = ~LED0_B;
	msleep(2000);
#endif

	gpio0_leds->data = ~(LSEG0_A | LSEG0_B | LSEG0_C | LSEG0_D | LSEG0_E | LSEG0_F | LSEG0_G | LSEG0_H);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_A | LSEG0_B | LSEG0_C | LSEG0_D | LSEG0_E | LSEG0_F | LSEG0_G | LSEG0_H | LSEG0_ANOD);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_A | LSEG0_B | LSEG0_C | LSEG0_D | LSEG0_E | LSEG0_F | LSEG0_G | LSEG0_H);
	msleep(2000);

#if 1
	gpio0_leds->data = ~(LSEG0_A);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_B);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_C);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_D);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_E);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_F);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_G);
	msleep(2000);
	gpio0_leds->data = ~(LSEG0_H);
	msleep(2000);
//	gpio0_leds->data = 0xaaaaaaaa;
#endif

#if 1
	gpio0_leds->data = ~(LSEG1_A);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_B);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_C);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_D);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_E);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_F);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_G);
	msleep(2000);
	gpio0_leds->data = ~(LSEG1_H);
	msleep(2000);
#endif
//	gpio0_leds->data = ~(1 << 31);
//	msleep(2000);
	gpio0_leds->data = 0xaaaaaaaa;

jump(0);

	uart_putstr("Entering Echo Test...\n\r");
	int rc = 0;
	while (1) {
	    char c;
	   uart_putchar((c = uart_getchar()));
	    if (c == 0x1b) {
		writeint(timer0->Snapshot);
		uart_putstr(" -- ");
		writeint(tic_msec);
		uart_putstr("Reset!\n\r");
		rc++;
		if (rc == 10) {
		    jump(0);
		}
	    }
	}
}
