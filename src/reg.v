// registers
//you can read six registers at a time, so saving registers in the work queue takes four cycles (the last four are reserved for work queue reading, sorry for lazy design)

module reg(input clk
            input[3:0] rin0, output[15:0] rout0,
            input[3:0] rin1, output[15:0] rout1,
            input wen0, input[3:0] win0, input[15:0] wdata0
            
            input[2:0] rpred, output predout,
            input wpreden, input[2:0] wpred, input write_pred_value
            
            input writing_regs, input[255:0] change_me,
            
            input give_me, output[255:0] the_regs
            );

    reg[15:0] data[0:15];
    reg pred[3:0];
    integer i;

    reg[255:0] the_regs;

    always @(posedge clk) begin
        //this should only happen if no other reads and writes are happening
        //ie, we are removing from work queue and doing something else
        if (writing_regs) begin
            for (i = 0; i < 16; i = i + 1) begin
                //store reg 0 at the end... dont worry about it lol....
                data[i] = change_me[16*i+15:16*i];
            end
            i = 0;
        end

        if (give_me) begin
            for (i = 0; i < 16; i = i + 1) begin
                the_regs[16*i+15:16*i] = data[i];
            end
            i = 0;
        end

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