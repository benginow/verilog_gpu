begin gpu(      input            clk,
               output reg [2:0] pixel,
               output           hsync_out,
               output           vsync_out,
               output           terminated
        );

//will be set to one when there is no longer anything to schedule on the queues
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
                    t_size, t_err
                        );

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

//REGISTERS ____________________________________________________________________
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

// PROCESSOR ____________________________________________________________________
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

reg[1:0] idling;

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

//zbuffer and framebuffer -- still need to implement these


//QUEUE ADDITION LOGIC________________________________________________________________________
reg[255:0] register_holder;
//if the processor is asking for more work(pointed out in the idle object), then 
reg writing_regs;
wire[255:0] queue_regs;
reg[3:0] queue_num;
//write to the queue
reg written_to_queue_flag;
always @(posedge clk) begin
    if (queue_wen) begin
        writing_regs <= current_regs; 
        //MAKE SURE TO LOOK AT WHEN QUEUE BIT IS ENABLED
        written_to_queue_flag <= queue_wen ? 1 : 0;
        t_add_regs <= (queue_num == 0 && !written_to_queue_flag)  
        if (queue_num == 0) begin
            //TODO: reset this to one after a cycle
            t_adding <= 1;
            t_add_regs <= current_regs;
        end
        if (queue_num == 1) begin
            l_adding <= 1;
            l_add_regs <= current_regs;
        end
        if (queue_num == 2) begin
            p_adding <= 1;
            p_add_regs <= current_regs;
        end
        if (queue_num == 3) begin
            r_adding <= 1;
            r_add_regs <= current_regs;
        end
        if (queue_num == 4) begin
            z_adding <= 1;
            z_add_regs <= current_regs;
        end
    end 
end

//QUEUE SCHEDULING LOGIC_________________________________________________________
always @(posedge clk) begin
    //MAKE SURE THAT REQUEST NEW PC WORKS CORRECTLY
    //YOU REQUEST REGS FROM QUEUE WHILE IDLING
    //THIS WHOLE SEQUENCE NEEDS TO BE FIXED
    //NEED TO BE IDLING WHEN 
    idling <= request_new_pc ? 2 : 
                (idling != 0) ? idling - 1 : 0;
    //FIX THIS
    requesting_regs_from_queue <= idling ? 1 : 0;
    writing_regs <= requesting_regs_from_queue ? 1 : 0;
    if (request_new_pc) begin
        //the buffer queue will process itself, so we don't check size of buffer queue
        if (r_size >= p_size && r_size != 0) begin
            new_pc <= 
        end
        else if (p_size >= l_size && p_size != 0) begin
            new_pc <= 
        end
        else if (l_size >= t_size && l_size != 0) begin
            new_pc <= 
        end
        else if (t_size != 0) begin
            new_pc <= 
        end
        else begin
            terminated <= 1;
        end
    end
end

//Z-BUFFER QUEUE LOGIC____________________________________________________________

always @(posedge clk) begin
    
end

endmodule