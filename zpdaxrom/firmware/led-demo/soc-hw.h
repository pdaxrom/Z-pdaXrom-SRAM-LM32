#ifndef SPIKEHW_H
#define SPIKEHW_H

#define PROMSTART 0x00000000
#define RAMSTART  0x00000800
#define RAMSIZE   0x400
#define RAMEND    (RAMSTART + RAMSIZE)

#define RAM_START 0x40000000
#define RAM_SIZE  0x00100000

#define FCPU      48000000

#define UART_RXBUFSIZE 32

/****************************************************************************
 * Types
 */
typedef unsigned int  uint32_t;    // 32 Bit
typedef signed   int   int32_t;    // 32 Bit

typedef unsigned char  uint8_t;    // 8 Bit
typedef signed   char   int8_t;    // 8 Bit

/****************************************************************************
 * Interrupt handling
 */
typedef void(*isr_ptr_t)(void);

void     irq_enable();
void     irq_disable();
void     irq_set_mask(uint32_t mask);
uint32_t irq_get_mak();

void     isr_init();
void     isr_register(int irq, isr_ptr_t isr);
void     isr_unregister(int irq);

/****************************************************************************
 * General Stuff
 */
void     halt();
void     jump(uint32_t addr);

/****************************************************************************
 * Timer
 */

#define MICO32_TIMER_STATUS_REG_OFFSET                  (0x00)
#define MICO32_TIMER_CONTROL_REG_OFFSET                 (0x04)
#define MICO32_TIMER_PERIOD_REG_OFFSET                  (0x08)
#define MICO32_TIMER_SNAPSHOT_REG_OFFSET                (0x0c)

/* status-register bits: */
#define MICO32_TIMER_STATUS_TO_BIT_MASK                 (0x1)

/* control-register bits */
#define MICO32_TIMER_CONTROL_INT_BIT_MASK               (0x1)
#define MICO32_TIMER_CONTROL_CONT_BIT_MASK              (0x2)
#define MICO32_TIMER_CONTROL_START_BIT_MASK             (0x4)
#define MICO32_TIMER_CONTROL_STOP_BIT_MASK              (0x8)

/* mico-timer register structure */
typedef struct st_MicoTimer{
    volatile unsigned int Status;
    volatile unsigned int Control;
    volatile unsigned int Period;
    volatile unsigned int Snapshot;
} timer_t;

void msleep(uint32_t msec);
void tic_init();

/***************************************************************************
 * GPIO0
 */
typedef struct st_MicoGPIO_t{
        volatile unsigned int data;
        volatile unsigned int tristate;
        volatile unsigned int irqMask;
        volatile unsigned int edgeCapture;
} gpio_t;



#define LSEG1_H	(1 << 31)
#define LSEG1_G	(1 << 30)
#define LSEG1_F	(1 << 29)
#define LSEG1_E	(1 << 28)
#define LSEG1_D	(1 << 27)
#define LSEG1_C	(1 << 26)
#define LSEG1_B	(1 << 25)
#define LSEG1_A	(1 << 24)

#define LSEG0_H	(1 << 23)
#define LSEG0_G	(1 << 22)
#define LSEG0_F	(1 << 21)
#define LSEG0_E	(1 << 20)
#define LSEG0_D	(1 << 19)
#define LSEG0_C	(1 << 18)
#define LSEG0_B	(1 << 17)
#define LSEG0_A	(1 << 16)

#define LSEG1_ANOD (1 << 12)
#define LSEG0_ANOD (1 << 11)

#define LED0_B	(1 << 10)
#define LED0_G	(1 << 9)
#define LED0_R	(1 << 8)

void gpio_init();

/***************************************************************************
 * UART0
 */
#define MICOUART_LSR_RX_RDY_MASK    (0x01)
#define MICOUART_LSR_PE_MASK        (0x04)
#define MICOUART_LSR_FE_MASK        (0x08)
#define MICOUART_LSR_TX_RDY_MASK    (0x20)

typedef struct st_MicoUart{
    volatile unsigned char rxtx;
    volatile unsigned char ier;
    volatile unsigned char iir;
    volatile unsigned char lcr;
    volatile unsigned char mcr;
    volatile unsigned char lsr;
    volatile unsigned char msr;
    volatile unsigned short div;
} uart_t;

void uart_init();
void uart_putchar(char c);
void uart_putstr(char *str);
char uart_getchar();

/***************************************************************************
 * Pointer to actual components
 */
extern gpio_t	*gpio0_leds;
extern uart_t   *uart0;
extern timer_t	*timer0;

#endif // SPIKEHW_H
