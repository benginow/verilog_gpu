
//inputs: where in memory is our first instruction (ie do we want to do )
//          which queue should we add to next (pass in the queue)

//this is not a pipelined core. just a simple joe schmoe core. it'll be ok. dw.
module processor(
                input clk,

                output[15:0] pc, input[31:0] instr,

                output[3:0] readreg0, input[15:0] in_reg0,
                output[3:0] readreg1, input[15:0] in_reg1,
                
                output reg_wen, output[3:0] reg_waddr, output[15:0] reg_wval,
                
                output[15:0] readmem0, input[15:0] in_mem0,
                output mem_wen, output[15:0] mem_waddr, output[15:0] mem_wval
                );

    //stage one is decode
    //stage two is read regs
    //stage three is read/write
    reg[2:0] stage = 0;

    //stage 1 variables
    reg[1:0] pred;
    reg optype;
    reg[4:0] opcode;
    reg[3:0] reg0;
    reg[3:0] reg1;
    reg[3:0] targetreg;
    reg[15:0] constant;


    //stage 2 variables
    reg[15:0] reg0val;
    reg[15:0] reg1val;


    //i/o variables
    wire[3:0] readreg0 = reg1;
    wire[3:0] readreg1 = reg2;

    always @(posedge clk) begin


        if (stage == 0) begin 
            pred <= instr[31:30];
            optype <= instr[29]
            opcode <= instr[28:24];
            reg0 <= instr[23:20];
            reg1 <= instr[19:16];
            targetreg <= instr[15:12];
            constant <= instr[15:0];
            stage <= stage + 1;

            //make sure you're no longer writing
            reg_wen <= 0;
            mem_wen <= 0;
        end

        //read registers
        if (stage == 1) begin
            reg0val <= in_reg0;
            reg1val <= in_reg1;
            stage <= stage + 1;
        end

        //read from memory as needed
        if (stage == 2) begin
            readmem0 <= reg0val;
        end

        if (stage == 3) begin
                //load from memory
            if (r_opcode == 0) begin
                //need to read from reg then load memory next cycle
                //TODO: DOUBLE CHECK THIS. MAKE SURE ASSEMBLER WORKS!
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= readmem0;
            end
            //store in memory
            if (r_opcode == 1) begin
                mem_wen <= 1;
                mem_waddr <= reg1val;
                mem_wval <= reg0val;
            end
            //multiply
            if (r_opcode == 2) begin
                //need to read from reg then store in reg next cycle
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= reg0val * reg1val;
            end
            //add
            if (r_opcode == 3) begin
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= reg0val + reg1val;
            end
            //sub
            if (r_opcode == 4) begin
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= reg0val - reg1val;
            end
            //srl
            if (r_opcode == 5) begin
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= reg0val >> reg1val;
            end
            //sll
            if (r_opcode == 6) begin
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= reg0val << reg1val;
            end
            //and
            if (r_opcode == 7) begin
                reg_wen <= 1;
                reg_waddr <= targetreg;
                reg_wval <= reg0val & reg1val;
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

            stage <= 0;
        end

    end


endmodule