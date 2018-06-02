`define LATTICE_FAMILY "MachXO2"
`define LATTICE_FAMILY_MachXO2
`define LATTICE_DEVICE "LCMXO2-7000HC"
`ifndef SYSTEM_CONF
`define SYSTEM_CONF
`timescale 1ns / 100 ps
`ifndef SIMULATION
   `define CharIODevice
`endif
`ifndef SIMULATION
   `define DEBUG_ROM
`endif
`ifndef SIMULATION
   `define CFG_DEBUG_ENABLED
`endif
`define CFG_EBA_RESET 32'h0
`define CFG_DEBA_RESET 32'h4000
`define CFG_DISTRAM_POSEDGE_REGISTER_FILE
`define MULT_ENABLE
`define CFG_PL_MULTIPLY_ENABLED
`define SHIFT_ENABLE
`define CFG_PL_BARREL_SHIFT_ENABLED
`define CFG_MC_DIVIDE_ENABLED
`define CFG_SIGN_EXTEND_ENABLED
`ifndef SIMULATION
   `define CFG_WATCHPOINTS 32'h0
`endif
`ifndef SIMULATION
   `define CFG_JTAG_ENABLED
`endif
`ifndef SIMULATION
   `define CFG_JTAG_UART_ENABLED
`endif
`define CFG_ICACHE_ENABLED
`define CFG_ICACHE_BASE_ADDRESS 32'h0
`define CFG_ICACHE_LIMIT 32'h401fffff
`define CFG_ICACHE_SETS 256
`define CFG_ICACHE_ASSOCIATIVITY 1
`define CFG_ICACHE_BYTES_PER_LINE 16
`define CFG_ICACHE_AUTO
`define CFG_DCACHE_ENABLED
`define CFG_DCACHE_BASE_ADDRESS 32'h0
`define CFG_DCACHE_LIMIT 32'h401fffff
`define CFG_DCACHE_SETS 256
`define CFG_DCACHE_ASSOCIATIVITY 1
`define CFG_DCACHE_BYTES_PER_LINE 16
`define CFG_DCACHE_AUTO
`define INCLUDE_LM32
`define ADDRESS_LOCK
`define LM32_I_PC_WIDTH 31
`define ADDRESS_LOCK
`define ebrEBR_WB_DAT_WIDTH 32
`define ebrINIT_FILE_NAME "firmware/boot0-serial/image.ram"
`define ebrINIT_FILE_FORMAT "hex"
`define ADDRESS_LOCK
`define sramREAD_LATENCY 1
`define sramWRITE_LATENCY 1
`define sramSRAM_ADDR_WIDTH 20
`define sramSRAM_DATA_WIDTH 16
`define sramSRAM_BE_WIDTH 2
`define sramASRAM_WB_DAT_WIDTH 32
`define timerS_WB_DAT_WIDTH 32
`define timerS_WB_ADR_WIDTH 4
`define ADDRESS_LOCK
`define timerPERIOD_NUM 32'h14
`define timerPERIOD_WIDTH 32'h20
`define timerWRITEABLE_PERIOD
`define timerREADABLE_SNAPSHOT
`define timerSTART_STOP_CONTROL
`define ADDRESS_LOCK
`define uartUART_WB_DAT_WIDTH 8
`define uartUART_WB_ADR_WIDTH 4
`define uartCLK_IN_MHZ 0
`define uartBAUD_RATE 115200
`define IB_SIZE 32'h4
`define OB_SIZE 32'h4
`define BLOCK_WRITE
`define BLOCK_READ
`define INTERRUPT_DRIVEN
`define CharIODevice
`define uartLCR_DATA_BITS 8
`define uartLCR_STOP_BITS 1
`define brd_ledsGPIO_WB_DAT_WIDTH 32
`define brd_ledsGPIO_WB_ADR_WIDTH 4
`define ADDRESS_LOCK
`define brd_ledsOUTPUT_PORTS_ONLY
`define brd_ledsDATA_WIDTH 32'h15
`endif // SYSTEM_CONF
