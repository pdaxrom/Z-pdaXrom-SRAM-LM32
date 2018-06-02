`include "zpdaxrom.v"
module zpdaxrom_top (
	input clk_ext,
	input btn_resetn,
	input rxd,
	output txd,
	
	output [8:0]	seg_led_h,
	output [8:0]	seg_led_l,
	output [2:0]	led_rgb,
 
	output [19:0] SRAM_ADDR,
	inout [15:0] SRAM_DATA,
	output			SRAM_CE,
	output			SRAM_OE,
	output			SRAM_WE,
	output			SRAM_UB,
	output			SRAM_LB
);

wire clk_sys;

pll pll_imp(
	.CLKI(clk_ext),
	.CLKOP(clk_sys)
);

zpdaxrom zpdaxrom_u
( 
	.clk_i(clk_sys),
	.reset_n(btn_resetn),
	.uartSIN(rxd),
	.uartSOUT(txd),
	.brd_ledsPIO_OUT({seg_led_l[8], seg_led_h[8], led_rgb, seg_led_h[7:0], seg_led_l[7:0]}),
	
	.sramsram_csn(SRAM_CE),
	.sramsram_oen(SRAM_OE),
	.sramsram_wen(SRAM_WE),
	.sramsram_be({SRAM_UB, SRAM_LB}),
	.sramsram_addr(SRAM_ADDR),
	.sramsram_data(SRAM_DATA)
);
endmodule
