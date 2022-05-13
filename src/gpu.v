module gpu(     input               clk,
                input [15:0]        t_pc,
                input [15:0]        l_pc,
                input [15:0]        p_pc,
                output              rasterization_flag,
                output [255:0]      r_ret_regs,
                output reg [2:0]    pixel,
                output              terminated,
                output reg [9:0]    counterX,
                output reg [9:0]    counterY
        );


reg terminated = 0;

//QUEUES ______________________________________________________________________
reg[255:0] global_add_regs;

reg t_reading;
reg t_adding;
wire[255:0] t_ret_regs;
wire[15:0] t_size;
wire t_err;
queue transformation(clk, 
                    t_reading, t_ret_regs,
                    t_adding, global_add_regs,
                    t_size, t_err);

reg l_reading;
reg l_adding;
wire[255:0] l_ret_regs;
reg [255:0] l_add_regs;
wire l_err;
wire[15:0] l_size;
queue lighting(clk, 
                l_reading, l_ret_regs,
                l_adding, global_add_regs,
                l_size, l_err);

reg p_reading;
reg p_adding;
wire[255:0] p_ret_regs;
reg [255:0] p_add_regs;
wire p_err;
wire[15:0] p_size;
queue projection(clk, 
                p_reading, p_ret_regs,
                p_adding, global_add_regs,
                p_size, p_err);

reg r_reading = 1;
reg r_adding;
wire[255:0] r_ret_regs;
reg [255:0] r_add_regs;
wire r_err;
wire[15:0] r_size;
queue rasterization(clk, 
                r_reading, r_ret_regs,
                r_adding, global_add_regs,
                r_size, r_err);

reg z_reading;
reg z_adding;
wire[255:0] z_ret_regs;
reg [255:0] z_add_regs;
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
reg writing_regs;
reg[255:0] queue_regs;
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

//SETTING IDLING ___________________________________

//we begin in an idling state! need to wait for a program counter
reg[1:0] idling = 1;
reg pc_found;
reg queue_written;

always @(posedge clk) begin
        //we idle when either we are adding to queue or requesting new pc (which includes removing from queue).

        //need to keep idling
        idling <= queue_wen ? 1 :
                request_new_pc? 1 : 
                pc_found ? 0 :
                queue_written ? 0 : idling;
end

//QUEUE ADDITION LOGIC________________________________________________________________

reg[2:0] queue_stage = 0;
reg[255:0] queue_registers;
reg[3:0] saved_queue_num;

//read registers then add to queue
always @(posedge clk) begin

        queue_stage <= queue_wen ? 1 : 0;
        saved_queue_num <= queue_number;

        //STAGE ONE
        //if queue_wen is 1, then we have already signalled to read from registers
        //thus, all we have to do is add to 
        t_adding <= (queue_stage == 1 && saved_queue_num == 0) ? 1 : 0;
        l_adding <= (queue_stage == 1 && saved_queue_num == 1) ? 1 : 0;
        p_adding <= (queue_stage == 1 && saved_queue_num == 2) ? 1 : 0;
        r_adding <= (queue_stage == 1 && saved_queue_num == 3) ? 1 : 0;
        z_adding <= (queue_stage == 1 && saved_queue_num == 4) ? 1 : 0;
        global_add_regs <= current_regs;   

        // STAGE TWO, DONE IDLING
        queue_written <= (queue_stage == 0);
end

//QUEUE SCHEDULING LOGIC_________________________________________________________
reg[1:0] scheduling_stage = 0;

//keep in mind, we only every read for p, s, or t. r is done in our hacky way,
//and z buffer queue handles itself
always @(posedge clk) begin
        //we are always running rasterization if we can be
        rasterization_flag <= r_size != 0;

        scheduling_stage <= request_new_pc ? 1 : 
                                scheduling_stage == 1 ? 2 : 0;

        //STAGE 0
        //read registers from queue and set new pc
        if (request_new_pc) begin
                new_pc <= (p_size > 0) ? p_pc :
                        (l_size > 0) ? l_pc :
                        (t_size > 0) ? t_pc : 0;
        end 

        p_reading <= (request_new_pc && (p_size > 0)) ? 1 : 0;
        l_reading <= (request_new_pc && (p_size == 0) && (l_size > 0)) ? 1 : 0;
        t_reading <= (request_new_pc && (p_size == 0) && (l_size == 0) && (t_size > 0)) ? 1 : 0;

        //STAGE 1
        //write registers
        queue_regs <= p_reading ? p_ret_regs :
                        l_reading ? l_ret_regs :
                        t_reading ? t_ret_regs : 0;
        writing_regs <= scheduling_stage == 1;

        
        
        //takes one cycle to find the next instruction once the pc has been updated
        pc_found <= scheduling_stage == 2;
        

end

//Z-BUFFER QUEUE LOGIC____________________________________________________________

always @(posedge clk) begin
    
end

endmodule
