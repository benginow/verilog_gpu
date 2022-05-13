    //stage zero is decode
    //stage one is read regs
    //stage two is read memory
    //stage three is execute

module processor(
                input clk,

                output[15:0] pc, input[31:0] instr,

                output[3:0] readreg0, input[15:0] in_reg0,
                output[3:0] readreg1, input[15:0] in_reg1,                
                output reg_wen, output[3:0] reg_waddr, output[15:0] reg_wval,

                output[1:0] pred, input pred_val,
                output pred_wen, output[1:0] pred_waddr, output pred_wval,
                
                output[15:0] readmem0, input[15:0] in_mem0,
                output mem_wen, output[15:0] mem_waddr, output[15:0] mem_wval,

                output queue_wen, output[3:0] queue_number, output request_new_pc, input[15:0] new_pc, input[1:0] idle
                );


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
    reg predregval;


    //i/o variables
    wire[3:0] readreg0 = reg1;
    wire[3:0] readreg1 = reg2;
    wire read_pred_val = pred_val;

    //request new pc register
    reg request_new_pc = 1;


    always @(posedge clk) begin
        //decode and idle stage
        if (stage == 0) begin 
            pred <= instr[31:30];
           optype <= instr[29];
            opcode <= instr[28:24];
            reg0 <= instr[23:20];
            reg1 <= instr[19:16];
            targetreg <= instr[15:12];
            constant <= instr[15:0];

            //stage increments only if we are not idling
            stage <= (request_new_pc || (idle != 0)) ? 0 : stage + 1;

            reg_wen <= 0;
            mem_wen <= 0;
            queue_wen <= 0;
            pred_wen <= 0;
            request_new_pc <= 0;
            
        end

        //read registers
        if (stage == 1) begin
            reg0val <= in_reg0;
            reg1val <= in_reg1;
            predregval <= pred ? read_pred_val : 1;
            stage <= stage + 1;
        end

        //read from memory as needed
        if (stage == 2) begin
            readmem0 <= reg0val;
        end

        if (stage == 3 & predregval) begin

            reg_wen <= (r_opcode == 0  || r_opcode == 2 
            ||r_opcode == 3 || r_opcode == 4 || r_opcode == 5
            || r_opcode == 6 || r_opcode == 7 || r_opcode == 8 
            || r_opcode == 9 || r_opcode == 10 || r_opcode == 11
            || r_opcode == 12) ? 1 : 0;

            reg_waddr <= targetreg;

            reg_wval <= (r_opcode == 0) ? readmem0 :
                        (r_opcode == 2) ? reg0val * reg1val :
                        (r_opcode == 3) ? reg0val + reg1val :
                        (r_opcode == 4) ? reg0val - reg1val :
                        (r_opcode == 5) ? reg0val >> reg1val :
                        (r_opcode == 6) ? reg0val << reg1val :
                        (r_opcode == 7) ? reg0val & reg1val :
                        (r_opcode == 8) ? !reg0val :
                        (r_opcode == 9) ? reg0val ^ reg1val :
                        (r_opcode == 10) ? reg0val | reg1val :
                        (r_opcode == 11) ? ~(reg0val & reg1val) :
                        (r_opcode == 12) ? constant : reg_waddr;


            mem_wen <= (r_opcode == 1);
            mem_waddr <= reg1val;
            mem_wval <= reg0val;

            pred_wen <= (r_opcode == 13);
            pred_waddr <= pred;
            pred_wval <= (reg0val < reg1val);
            

            //store in queue based off of number
            //need to communicate this to gpu, since gpu keeps track of all of this
            queue_wen <= (r_opcode == 14) || (r_opcode == 15);
            queue_number <= (r_opcode == 14) ? reg0val : constant;

            request_new_pc <= r_opcode == 16;

            stage <= 0;
            pc <= pc + 1;
        end

    end


endmodule
