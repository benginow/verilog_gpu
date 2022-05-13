// registers

module regs(input clk,
            input [3:0] rin0, output reg[31:0] rout0,
            input [3:0] rin1, output reg[31:0] rout1,
            input       wen0, input[3:0] win0, input[31:0] wdata0,
            input [1:0] rpred, output reg predout,
            input       wpreden, input[1:0] wpred, input write_pred_value,
            input       writing_regs, input[255:0] change_me,
            input       give_me, output reg [255:0] the_regs
            );

   reg [31:0]           data[0:15];
   reg                  pred[3:0];
   integer              i;

   always @(posedge clk) begin
      //this should only happen if no other reads and writes are happening
      //ie, we are removing from work queue and doing something else
      if (writing_regs) begin
         for (i = 0; i < 8; i = i + 1) begin
            //store reg 0 at the end... dont worry about it lol....
            data[i] <= change_me[(32*i+31)+:32];
         end
         i = 0;
      end

      if (give_me) begin
         for (i = 0; i < 8; i = i + 1) begin
            the_regs[(32*i+31)+:32] <= data[i];
         end
         i = 0;
      end

      rout0 <= data[rin0];
      rout1 <= data[rin1];
      predout <= pred[rpred];
      if (wen0) begin
         data[win0] <= wdata0;
      end
      if (wpreden) begin
         pred[wpred] <= write_pred_value;
      end

   end

endmodule
