module framebuffer_mem(input clk,
                       input [19:0]      read,
                       output reg [23:0] out,
                       input             writing,
                       input [19:0]      waddr,
                       input [23:0]      wdata);

   reg [23:0]                            data[0:20'hfffff];

   integer                               i;
   initial begin
      for (i = 0; i < 20'hfffff; i++)
        data[i] = 0;
   end

   always @(posedge clk) begin
      out <= data[read];
      if (writing) begin
         data[waddr] <= wdata;
      end
   end

endmodule
