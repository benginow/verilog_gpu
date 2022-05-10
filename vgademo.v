`include "hvsync_generator.v"

module vgademo(
               input            clk,
               output reg [2:0] pixel,
               output           hsync_out,
               output           vsync_out
               );
   wire                         inDisplayArea;
   wire [9:0]                   CounterX;
   wire [9:0]                   CounterY;

   hvsync_generator hvsync(
                           .clk(clk),
                           .vga_h_sync(hsync_out),
                           .vga_v_sync(vsync_out),
                           .CounterX(CounterX),
                           .CounterY(CounterY),
                           .inDisplayArea(inDisplayArea)
                           );

   always @(posedge clk)
     begin
        if (inDisplayArea)
          pixel <= CounterX[9:7];
        else // if it's not to display, go dark
          pixel <= 3'b000;
     end

endmodule
