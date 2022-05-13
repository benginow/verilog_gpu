begin gpu(      input               clk,
                input [15:0]        t_pc,
                input [15:0]        l_pc,
                input [15:0]        p_pc,
                output              rasterization_flag,
                output [255:0]      r_ret_regs,
                output reg [2:0]    pixel,
                output              terminated
                output reg [9:0]    counterX,
                output reg [9:0]    counterY
        );


reg terminated = 0;

//QUEUES ______________________________________________________________________
wire t_reading;
wire t_adding;
wire[255:0] t_ret_regs;
wire [255:0] t_add_regs;
wire[15:0] t_size;
wire t_err;
queue transformation(clk, 
                    t_reading, t_ret_regs,
                    t_adding, t_add_regs,
                    t_size, t_err);

wire l_reading;
wire l_adding;
wire[255:0] l_ret_regs;
wire [255:0] l_add_regs;
wire l_err;
wire[15:0] l_size;
queue lighting(clk, 
                l_reading, l_ret_regs,
                l_adding, l_add_regs,
                l_size, l_err);

wire p_reading;
wire p_adding;
wire[255:0] p_ret_regs;
wire [255:0] p_add_regs;
wire p_err;
wire[15:0] p_size;
queue projection(clk, 
                p_reading, p_ret_regs,
                p_adding, p_add_regs,
                p_size, p_err);

wire r_reading;
wire r_adding;
wire[255:0] r_ret_regs;
wire [255:0] r_add_regs;
wire r_err;
wire[15:0] r_size;
queue rasterization(clk, 
                r_reading, r_ret_regs,
                r_adding, r_add_regs,
                r_size, r_err);

wire z_reading;
wire z_adding;
wire[255:0] z_ret_regs;
wire [255:0] z_add_regs;
wire z_err;
wire[15:0] z_size;
queue zbuffer(clk, 
                z_reading, z_ret_regs,
                z_adding, z_add_regs,
                z_size, z_err);


//DEDICATED MEMORIES ___________________________________________________________
mem mem(clk, readmem0, in_mem0,
             memnull0, memnull1,
             mem_wen, mem_waddr, mem_wval);
//instruction memory
//all you can do is read from instr mem
mem instr_mem(clk, pc, read_instr_mem_addr,
                null0, null1,
                null2, null3, null4);

//REGISTERS __________________________________________________________
reg[3:0] queue_read;
wire[255:0] current_regs;
reg[255:0] writing_regs;
reg curr_regs(clk, readreg0, reg0_output, 
                    readreg1, reg1_output,
                    reg_wen, reg_waddr, reg_wval,

                    pred, pred_val, 
                    pred_wen, pred_waddr, pred_wval,

                    writing_regs, queue_regs,

                    queue_wen, current_regs
                    );

// PROCESSOR __________________________________________________________
wire[15:0] pc;

wire[15:0] read_instr_mem_addr;

wire[3:0] readreg0;
wire[3:0] readreg1;
wire[15:0] reg0_output;
wire[15:0] reg1_output;
wire reg_wen;
wire[3:0] reg_waddr;
wire[15:0] reg_wval;

wire[1:0] pred;
wire pred_val;
wire pred_wen;
wire[1:0] pred_waddr;
wire pred_wval;

wire[15:0] readmem0;
wire[15:0] in_mem0;
wire mem_wen;
wire[15:0] mem_waddr;
wire[15:0] mem_wval;

wire queue_wen;
wire[3:0] queue_number;

wire request_new_pc;
wire new_pc;

reg[1:0] idling = 1;

processor proc1(clk, 

                pc, read_instr_mem_addr,

                readreg0, in_reg0,
                readreg1, in_reg1,
                reg_wen, reg_waddr, reg_wval,

                pred, pred_val,
                pred_wen, pred_waddr, pred_wval,

                readmem0, in_mem0,
                mem_wen, mem_waddr, mem_wval,

                queue_wen, queue_number,
                request_new_pc, new_pc, idling
                );

//QUEUE ADDITION LOGIC________________________________________________________________

always @(posedge clk) begin

end

always @(posedge clk) begin

end

//QUEUE SCHEDULING LOGIC_________________________________________________________
always @(posedge clk) begin
    
end

//Z-BUFFER QUEUE LOGIC____________________________________________________________

always @(posedge clk) begin
    
end

endmodule
