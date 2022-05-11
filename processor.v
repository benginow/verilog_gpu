
//inputs: where in memory is our first instruction (ie do we want to do )
//          which queue should we add to next (pass in the queue)
module processor(input clk,
                input[31:0] d_instr, output[15:0] f_pc,
                output[3:0] readreg0, input[16:0] inputreg0,
                output[3:0] readreg1, input[16:0] inputreg1,
                );

//this will not be pipelined. it takes one clock cycle to access memory (?)

//read this from instr mem
reg[15:0] f_pc;
//fetch instruction
always @(posedge clk) begin
    f_pc <= f_pc + 1;
end

//decode instruction
//i think the instruction should be passed in from gpu.v
always @(posedge clk) begin
    //print these later to make sure decode is working
    r_pred <= d_instr[31:30];
    r_optype <= d_instr[29]
    r_opcode <= d_instr[28:24];
    r_reg1 <= d_instr[23:20];
    r_reg2 <= d_instr[19:16];
    r_constant <= d_instr[15:0];
end

//read from registers as needed
reg[1:0] r_pred;
reg r_optype;
reg[4:0] r_opcode;
reg[3:0] r_reg1;
reg[3:0] r_reg2;
reg[15:0] r_constant;
always @(posedge clk) begin
    //load from memory
    if (r_opcode == 0) begin
        //need to read from reg then load memory next cycle
    end
    //store in memory
    if (r_opcode == 1) begin
        //need to read from reg then store in memory next cycle
    end
    //multiply
    if (r_opcode == 2) begin
        //need to read from reg then store in reg next cycle

    end
    if (r_opcode == 3) begin

    end
    if (r_opcode == 4) begin

    end
    if (r_opcode == 5) begin
        
    end
    if (r_opcode == 6) begin

    end
    if (r_opcode == 7) begin
        
    end
    if (r_opcode == 8) begin

    end
    if (r_opcode == 9) begin

    end
    if (r_opcode == 10) begin

    end
    if (r_opcode == 11) begin

    end
    if (r_opcode == 12) begin
        
    end
    if (r_opcode == 13) begin

    end
    if (r_opcode == 14) begin

    end
    //store in queue based off of number
    //need to communicate this to gpu, since gpu keeps track of all of this
    if (r_opcode == 15) begin

    end
    //store in queue based off of register
    //need to communicate this to gpu so it may do it
    if (r_opcode == 16) begin
        
    end
    //end
    if (r_opcode == 17) begin

    end
end

//execution stage
//read from memory as needed, store in memory as needed
//store in register as needed
reg[15:0] e_reg_data;
reg[15:0] e_reg_store;

reg[1:0] e_pred;
reg e_optype;
reg[4:0] e_opcode;
reg[3:0] e_reg1;
reg[3:0] e_reg2;
reg[15:0] e_constant;
always @(posedge clk) begin

end

endmodule