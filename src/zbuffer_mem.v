module zbuffer_mem(input clk,
                   input [19:0] read0,
                   output reg [15:0] out0,
                   input [19:0] read1,
                   output reg [15:0] out1,
                   input        writing,
                   input [19:0] waddr,
                   input [15:0] wdata);

   reg [15:0]                    data[0:20'hfffff];

   always @(posedge clk) begin
      out0 <= data[read0];
      out1 <= data[read1];
      if (writing) begin
         data[waddr] = wdata;
      end
   end

endmodule
