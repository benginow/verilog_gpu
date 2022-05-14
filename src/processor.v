//stage zero is decode
//stage one is read regs
//stage two is read memory
//stage three is execute

module processor(input            clk,
                 output [15:0] curr_pc, input [31:0] instr,
                 output [3:0]  readreg0, input signed [31:0] in_reg0,
                 output [3:0]  readreg1, input signed [31:0] in_reg1,
                 output        reg_wen, output [3:0] reg_waddr, output [31:0] reg_wval,
                 output [1:0]  pred, input pred_val,
                 output        pred_wen, output [1:0] pred_waddr, output pred_wval,
                 output [15:0] readmem0, input [31:0] in_mem0,
                 output        mem_wen, output [15:0] mem_waddr, output [31:0] mem_wval,
                 output        queue_wen, output[3:0] queue_number,
                 output        request_new_pc, input set_pc, input[15:0] new_pc);

   reg                         request_new_pc_ = 1;
   assign request_new_pc = request_new_pc_;

   // PC stuff
   reg [15:0]                  pc;
   initial request_new_pc_ = 1;

   assign curr_pc = (set_pc ? new_pc : continue_on ? pc + 1 : pc);
   always @(posedge clk) begin
      if (set_pc && request_new_pc) begin
         pc <= new_pc;
         request_new_pc_ <= 0;
      end
   end

   reg [2:0] stage = 0;
   reg [31:0] saved_ins;

   // Instruction Decoding
   wire [31:0] ins = stage == 0 ? instr : saved_ins;
   assign pred = ins[31:30];
   wire        optype = ins[29];
   wire [4:0]  opcode = ins[28:24];
   assign readreg0 = ins[23:20];
   assign readreg1 = ins[19:16];
   wire [3:0]  tgtreg = (opcode == 0 || opcode == 5 || opcode == 6 || opcode == 8) ? ins[19:16] :
               (opcode == 12) ? ins[23:20] : ins[15:12];
   wire [15:0] constant = ins[15:0];

   wire        dont_write = (stage == 1 && (pred_val == 0 && pred != 0)) || request_new_pc;
   wire        continue_on = (stage == 1 && (pred_val == 0 && pred != 0)) ||
               (stage == 1 && (opcode == 1 || opcode == 2 || opcode == 3 || opcode == 4 || opcode == 5 || opcode == 6 || opcode == 7 || opcode == 8 || opcode == 9 || opcode == 10 || opcode == 11 || opcode == 12 || opcode == 13 || opcode == 14 || opcode == 15 || opcode == 16)) ||
               (stage == 2 && (opcode == 0));

   // Memory
   assign readmem0 = in_reg0[15:0];
   assign mem_wen = !dont_write && (stage == 1 && opcode == 1);
   assign mem_waddr = in_reg1[15:0];
   assign mem_wval = in_reg0;

   // Queue
   assign queue_wen = !dont_write && ((stage == 1 && opcode == 15) || (stage == 1 && opcode == 14));
   assign queue_number = opcode == 15 ? constant[3:0] : in_reg0[3:0];

   // Predicate writing
   assign pred_wen = !dont_write && (stage == 1 && opcode == 13);
   assign pred_waddr = tgtreg[1:0];
   assign pred_wval = in_reg0_tmp < in_reg1_tmp;
   wire signed [15:0] in_reg0_tmp = in_reg0[15:0];
   wire signed [15:0] in_reg1_tmp = in_reg1[15:0];
   wire signed [15:0] mult = in_reg0_tmp * in_reg1_tmp;

   // Register writing
   assign reg_wen = !dont_write && ((stage == 1 && opcode == 12) || (stage == 1 && (opcode == 2 || opcode == 3 || opcode == 4 || opcode == 5 || opcode == 6 || opcode == 7 || opcode == 8 || opcode == 9 || opcode == 10 || opcode == 11)) || (stage == 2 && (opcode == 0)));
   assign reg_waddr = tgtreg;
   assign reg_wval = opcode == 2 ? ({{16{1'b0}}, mult}) :
                     opcode == 3 ? (in_reg0 + in_reg1) :
                     opcode == 4 ? (in_reg0 - in_reg1) :
                     opcode == 5 ? (in_reg0 >> constant) :
                     opcode == 6 ? (in_reg0 << constant) :
                     opcode == 7 ? (in_reg0 & in_reg1) :
                     opcode == 8 ? (~in_reg0) :
                     opcode == 9 ? (in_reg0 ^ in_reg1) :
                     opcode == 10 ? (in_reg0 | in_reg1) :
                     opcode == 11 ? (~(in_reg0 & in_reg1)) :
                     opcode == 12 ? ({{16{1'b0}}, constant}) :
                     opcode == 0 ? in_mem0 : 0;

   // Stages
   always @(posedge clk) begin
      if (!request_new_pc) begin
         if (stage == 0) begin
            saved_ins <= ins;
         end
         if (stage == 1) begin
            if (opcode == 16) begin
               request_new_pc_ <= 1;
            end
         end
         if (continue_on) begin
            pc <= pc + 1;
            stage <= 0;
         end
         else begin
            stage <= stage + 1;
         end
      end
   end

endmodule
