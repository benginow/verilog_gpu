// registers
//you can read four registers at a time, so saving registers in the work queue takes four cycles

begin reg(input clk
            input[3:0] rin0, output[15:0] rout0,
            input[3:0] rin1, output[15:0] rout1,
            input[3:0] rin2, output[15:0] rout2,
            input[3:0] rin3, output[15:0] rout3,
            input wen, input[3:0] win, input[15:0] wdata);

    reg[15:0] data[0:15];

    always @(posedge clk) begin
        rout0 <= data[rin0];
        rout1 <= data[rin1];
        if (wen) begin
            data[win] = wdata;
        end
    end

endmodule