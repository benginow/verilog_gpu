module mem(input clk,
            input [15:0] read0, output reg[31:0] out0,
            input [15:0] read1, output reg[31:0] out1,
            input       writing, input [15:0] waddr, input [31:0] wdata);

    reg [31:0]data[0:16'hffff];

    always @(posedge clk) begin
        out0 <= data[read0];
        out1 <= data[read1];
        if (writing) begin
            data[waddr] <= wdata;
        end
    end

endmodule
