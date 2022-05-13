    //stage zero is decode
    //stage one is read regs
    //stage two is read memory
    //stage three is execute

module processor(
                input clk,

                output reg[15:0] pc, input[31:0] instr,

                output[3:0] readreg0, input[31:0] in_reg0,
                output[3:0] readreg1, input[31:0] in_reg1,
                output reg_wen, output[3:0] reg_waddr, output[31:0] reg_wval,

                output reg [1:0] pred, input pred_val,
                output pred_wen, output[1:0] pred_waddr, output pred_wval,
                output[15:0] readmem0, input[31:0] in_mem0,
                output mem_wen, output[15:0] mem_waddr, output[31:0] mem_wval,

                output queue_wen, output[3:0] queue_number, output reg request_new_pc, input[15:0] new_pc, input[1:0] idle
                );


    reg[2:0] stage = 0;

    //stage 1 variables
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
    assign readreg0 = reg0;
    assign readreg1 = reg1;
    wire read_pred_val = pred_val;

    //request new pc register
    initial request_new_pc = 1;


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

            reg_wen <= (opcode == 0  || opcode == 2 
            ||opcode == 3 || opcode == 4 || opcode == 5
            || opcode == 6 || opcode == 7 || opcode == 8 
            || opcode == 9 || opcode == 10 || opcode == 11
            || opcode == 12) ? 1 : 0;

            reg_waddr <= targetreg;

            reg_wval <= (opcode == 0) ? readmem0 :
                        (opcode == 2) ? reg0val * reg1val :
                        (opcode == 3) ? reg0val + reg1val :
                        (opcode == 4) ? reg0val - reg1val :
                        (opcode == 5) ? reg0val >> reg1val :
                        (opcode == 6) ? reg0val << reg1val :
                        (opcode == 7) ? reg0val & reg1val :
                        (opcode == 8) ? !reg0val :
                        (opcode == 9) ? reg0val ^ reg1val :
                        (opcode == 10) ? reg0val | reg1val :
                        (opcode == 11) ? ~(reg0val & reg1val) :
                        (opcode == 12) ? constant : reg_waddr;


            mem_wen <= (opcode == 1);
            mem_waddr <= reg1val;
            mem_wval <= reg0val;

            pred_wen <= (opcode == 13);
            pred_waddr <= pred;
            pred_wval <= (reg0val < reg1val);
            

            //store in queue based off of number
            //need to communicate this to gpu, since gpu keeps track of all of this
            queue_wen <= (opcode == 14) || (opcode == 15);
            queue_number <= (opcode == 14) ? reg0val : constant;

            request_new_pc <= opcode == 16;

            stage <= 0;
            pc <= pc + 1;
        end

    end


endmodule
