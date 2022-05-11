// registers
//you can read four registers at a time, so saving registers in the work queue takes four cycles

begin reg(input clk
            input[3:0] rin0, output[15:0] rout0,
            input[3:0] rin1, output[15:0] rout1,
            input[3:0] rin2, output[15:0] rout2,
            input[3:0] rin3, output[15:0] rout3,
            input wen0, input[3:0] win0, input[15:0] wdata0
            input wen1, input[3:0] win1, input[15:0] wdata1
            input wen2, input[3:0] win2, input[15:0] wdata2
            input wen3, input[3:0] win3, input[15:0] wdata3
            
            input[2:0] rpred, output predout,
            input wpreden, input[2:0] wpred, input write_pred_value);

    reg[15:0] data[0:15];
    reg pred[3:0];

    always @(posedge clk) begin
        rout0 <= data[rin0];
        rout1 <= data[rin1];
        rout2 <= data[rin2];
        rout3 <= data[rin3];
        predout <= pred[rpred]
        if (wen0) begin
            data[win0] = wdata0;
        end
        if (wen1) begin
            data[win1] = wdata1;
        end
        if (wen2) begin
            data[win2] = wdata2;
        end
        if (wen3) begin
            data[win3] = wdata3;
        end
        if (wpreden) begin
            pred[wpred] = wpredvalue;
        end
        

    end

endmodule