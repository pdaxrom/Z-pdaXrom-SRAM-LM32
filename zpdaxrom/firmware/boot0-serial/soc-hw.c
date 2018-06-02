#include "soc-hw.h"

gpio_t	*gpio0_leds = (gpio_t *) 0x80000100;
uart_t	*uart0 = (uart_t *)      0x80000080;
timer_t *timer0 = (timer_t *)    0x80000000;

isr_ptr_t isr_table[32];


void tic_isr();
/***************************************************************************
 * IRQ handling
 */
void isr_null()
{
}

void irq_handler(uint32_t pending)
{
	int i;

	for(i=0; i<32; i++) {
		if (pending & 0x01) (*isr_table[i])();
		pending >>= 1;
	}
}

void isr_init()
{
	int i;
	for(i=0; i<32; i++)
		isr_table[i] = &isr_null;
}

void isr_register(int irq, isr_ptr_t isr)
{
	isr_table[irq] = isr;
}

void isr_unregister(int irq)
{
	isr_table[irq] = &isr_null;
}

/***************************************************************************
 * TIMER Functions
 */
volatile uint32_t tic_msec;

void msleep(uint32_t msec)
{
	uint32_t snap = tic_msec;
	uint32_t cnt =  snap + msec;
	int ov = (cnt < snap)?1:0;
	do {
 	    snap = tic_msec;
	    if (ov && snap > cnt) {
		continue;
	    }
 	} while ( snap < cnt );
}

void tic_isr()
{
	tic_msec++;

	timer0->Status = 0;
}

void tic_init()
{
	tic_msec = 0;

	isr_register(0, &tic_isr);

	timer0->Control = MICO32_TIMER_CONTROL_STOP_BIT_MASK;
	timer0->Period = (FCPU / 1000);
	timer0->Control = (MICO32_TIMER_CONTROL_START_BIT_MASK | MICO32_TIMER_CONTROL_INT_BIT_MASK | MICO32_TIMER_CONTROL_CONT_BIT_MASK);
}



/***************************************************************************
 * GPIO Functions
 */

void gpio_init()
{
    gpio0_leds->data = (LED0_R | LED0_G | LED0_B);
}

/***************************************************************************
 * UART Functions
 */
void uart_init()
{
	//uart0->ier = 0x00;  // Interrupt Enable Register
	//uart0->lcr = 0x03;  // Line Control Register:    8N1
	//uart0->mcr = 0x00;  // Modem Control Register

	// Setup Divisor register (Fclk / Baud)
	//uart0->div = (FCPU/(57600*16));
}

char uart_getchar()
{
	while (!(uart0->lsr & MICOUART_LSR_RX_RDY_MASK)) ;
	return uart0->rxtx;
}

void uart_putchar(char c)
{
	while (!(uart0->lsr & MICOUART_LSR_TX_RDY_MASK));
        uart0->rxtx = c;
}

void uart_putstr(char *str)
{
	char *c = str;
	while(*c) {
		uart_putchar(*c);
		c++;
	}
}
