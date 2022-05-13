module mem(input clk,
            input [15:0] read0, output[15:0] out0,
            input [15:0] read1, output[15:0] out1,
            input       writing, input waddr[15:0], input wdata[15:0]);

    reg [15:0]data[0:16'hffff];

    always @(posedge clk) begin
        out0 <= data[rin0];
        out1 <= data[rin1];
        if (writing) begin
            data[waddr] = wdata;
        end
    end

endmodule
