module gpu(input               clk,
           input [15:0]      t_pc,
           input [15:0]      l_pc,
           input [15:0]      p_pc,
           output [255:0]    r_ret_regs,
           output reg [23:0] pixel,
           output reg [9:0]  counterX,
           output reg [9:0]  counterY,
           output            terminated);

   reg                       emptied = 0;
   reg [19:0]                term_counter = 20'hfffff;
   always @(posedge clk) begin
      if (should_term) begin
         if (term_counter > 0) begin
            term_counter <= term_counter - 1;
         end
      end
   end

   assign terminated = should_term && term_counter == 0;
   wire should_term = (t_size == 0) && (l_size == 0) && (p_size == 0) && (r_size == 0) && (z_size == 0);

   //QUEUES ______________________________________________________________________
   wire [255:0]              global_add_regs;

   wire                      t_reading;
   wire                      t_adding;
   wire [255:0]              t_ret_regs;
   wire [15:0]               t_size;
   wire                      t_err;
   queue transformation(clk,
                        t_reading, t_ret_regs,
                        t_adding, global_add_regs,
                        t_size, t_err);

   wire                      l_reading;
   wire                      l_adding;
   wire [255:0]              l_ret_regs;
   wire [255:0]              l_add_regs;
   wire                      l_err;
   wire [15:0]               l_size;
   queue lighting(clk,
                  l_reading, l_ret_regs,
                  l_adding, global_add_regs,
                  l_size, l_err);

   wire                      p_reading;
   wire                      p_adding;
   wire [255:0]              p_ret_regs;
   wire [255:0]              p_add_regs;
   wire                      p_err;
   wire [15:0]               p_size;
   queue projection(clk,
                    p_reading, p_ret_regs,
                    p_adding, global_add_regs,
                    p_size, p_err);

   wire                      r_reading;
   wire                      r_adding;
   wire [255:0]              r_add_regs;
   wire                      r_err;
   wire [15:0]               r_size;
   queue rasterization(clk,
                       r_reading, r_ret_regs,
                       r_adding, global_add_regs,
                       r_size, r_err);

   wire                      z_reading;
   wire                      z_adding;
   wire [255:0]              z_ret_regs;
   wire [255:0]              z_add_regs;
   wire                      z_err;
   wire [15:0]               z_size;
   queue zbuffer_queue(clk,
                       z_reading, z_ret_regs,
                       z_adding, z_add_regs,
                       z_size, z_err);


   //DEDICATED MEMORIES ___________________________________________________________
   wire [19:0]               zbuffer_mem_read0;
   wire [15:0]               zbuffer_mem_out0;
   wire [19:0]               zbuffer_mem_read1;
   wire [15:0]               zbuffer_mem_out1;
   wire                      zbuffer_mem_writing;
   wire [19:0]               zbuffer_mem_waddr;
   wire [15:0]               zbuffer_mem_wdata;
   zbuffer_mem zbuffer_mem(clk,
                           zbuffer_mem_read0, zbuffer_mem_out0,
                           zbuffer_mem_read1, zbuffer_mem_out1,
                           zbuffer_mem_writing, zbuffer_mem_waddr, zbuffer_mem_wdata);

   wire [19:0]               framebuffer_mem_read;
   wire [23:0]               framebuffer_mem_out;
   wire                      framebuffer_mem_writing;
   wire [19:0]               framebuffer_mem_waddr;
   wire [23:0]               framebuffer_mem_wdata;
   framebuffer_mem framebuffer_mem(clk,
                                   framebuffer_mem_read, framebuffer_mem_out,
                                   framebuffer_mem_writing, framebuffer_mem_waddr, framebuffer_mem_wdata);

   wire [15:0]               mem_read1;
   wire [31:0]               mem_out1;
   mem mem(clk, readmem0, in_mem0,
           mem_read1, mem_out1,
           mem_wen, mem_waddr, mem_wval);

   //instruction memory
   //all you can do is read from instr mem
   wire [15:0]               instr_mem_read1;
   wire [31:0]               instr_mem_out1;
   wire                      instr_mem_writing;
   wire [15:0]               instr_mem_waddr;
   wire [31:0]               instr_mem_wdata;
   mem instr_mem(clk, pc, read_instr_mem_addr,
                 instr_mem_read1, instr_mem_out1,
                 instr_mem_writing, instr_mem_waddr, instr_mem_wdata);


   //REGISTERS __________________________________________________________

   wire [255:0]              current_regs;
   wire                      writing_regs;
   wire [255:0]              queue_regs;
   regs                       curr_regs(clk, readreg0, reg0_output, 
                                        readreg1, reg1_output,
                                        reg_wen, reg_waddr, reg_wval,

                                        pred, pred_val, 
                                        pred_wen, pred_waddr, pred_wval,

                                        writing_regs, queue_regs,

                                        queue_wen, current_regs);

   // PROCESSOR

   wire [15:0]               pc;
   wire [31:0]               read_instr_mem_addr;

   wire [3:0]                readreg0;
   wire [3:0]                readreg1;
   wire [31:0]               reg0_output;
   wire [31:0]               reg1_output;
   wire                      reg_wen;
   wire [3:0]                reg_waddr;
   wire [31:0]               reg_wval;

   wire [1:0]                pred;
   wire                      pred_val;
   wire                      pred_wen;
   wire [1:0]                pred_waddr;
   wire                      pred_wval;

   wire [15:0]               readmem0;
   wire [31:0]               in_mem0;
   wire                      mem_wen;
   wire [15:0]               mem_waddr;
   wire [31:0]               mem_wval;

   wire                      queue_wen;
   wire [3:0]                queue_number;

   assign t_adding = (queue_wen == 1 && queue_number == 0);
   assign l_adding = (queue_wen == 1 && queue_number == 1);
   assign p_adding = (queue_wen == 1 && queue_number == 2);
   assign r_adding = (queue_wen == 1 && queue_number == 3);
   assign z_adding = (queue_wen == 1 && queue_number == 4);
   assign global_add_regs = current_regs;

   wire                      request_new_pc;
   wire                      set_pc;
   wire [15:0]               new_pc;

   processor proc1(clk,
                   pc, read_instr_mem_addr,
                   readreg0, reg0_output,
                   readreg1, reg1_output,
                   reg_wen, reg_waddr, reg_wval,
                   pred, pred_val,
                   pred_wen, pred_waddr, pred_wval,
                   readmem0, in_mem0,
                   mem_wen, mem_waddr, mem_wval,
                   queue_wen, queue_number,
                   request_new_pc, set_pc, new_pc);

   // SCHEDULING LOGIC ______________________________________________________

   reg                       scheduling_stage = 0;
   reg                       should_p_reading_saved;
   reg                       should_l_reading_saved;
   reg                       should_t_reading_saved;

   always @(posedge clk) begin
      if (scheduling_stage == 0) begin
         if (p_reading || l_reading || t_reading) begin
            scheduling_stage <= 1;
            should_p_reading_saved <= should_p_reading;
            should_l_reading_saved <= should_l_reading;
            should_t_reading_saved <= should_t_reading;
         end
      end
      if (scheduling_stage == 1) begin
         scheduling_stage <= 0;
      end
   end

   wire                      should_r_reading = (request_new_pc && (r_size > 0) && (z_size == 0) ? 1 : 0);
   wire should_p_reading = (request_new_pc && (p_size > 0) && (r_size == 0) && (z_size == 0) ? 1 : 0);
   wire should_l_reading = (request_new_pc && (l_size > 0) && (r_size == 0) && (p_size == 0) && (z_size == 0) ? 1 : 0);
   wire should_t_reading = (request_new_pc && (t_size > 0) && (r_size == 0) && (p_size == 0) && (l_size == 0) && (z_size == 0) ? 1 : 0);

   assign p_reading = should_p_reading && scheduling_stage == 0;
   assign l_reading = should_l_reading && scheduling_stage == 0;
   assign t_reading = should_t_reading && scheduling_stage == 0;

   assign queue_regs = should_p_reading_saved ? p_ret_regs :
                       should_l_reading_saved ? l_ret_regs :
                       should_t_reading_saved ? t_ret_regs : 0;
   assign writing_regs = scheduling_stage == 1 && (should_p_reading_saved || should_l_reading_saved || should_t_reading_saved);

   assign set_pc = scheduling_stage == 1;
   assign new_pc = should_p_reading_saved ? p_pc :
                   should_l_reading_saved ? l_pc :
                   should_t_reading_saved ? t_pc : 0;


   //Z-BUFFER QUEUE LOGIC____________________________________________________________

   reg [2:0]                 zbuffer_stage = 0;
   assign z_reading = (zbuffer_stage == 1);

   assign zbuffer_mem_read0 = {{4{1'b0}}, zbuffer_x} * 480 + {{4{1'b0}}, zbuffer_y};

   wire                      should_write = (zbuffer_stage == 3 && zbuffer_x >= 0 && zbuffer_y >= 0 && zbuffer_x < 640 && zbuffer_y < 480 && zbuffer_z >= 0 && zbuffer_z < zbuffer_mem_out0);
   assign zbuffer_mem_writing = should_write;
   assign zbuffer_mem_waddr = {{4{1'b0}}, zbuffer_x} * 480 + {{4{1'b0}}, zbuffer_y};
   assign zbuffer_mem_wdata = zbuffer_z;
   assign framebuffer_mem_writing = should_write;
   assign framebuffer_mem_waddr = zbuffer_mem_waddr;
   assign framebuffer_mem_wdata = {zbuffer_red[7:0], zbuffer_green[7:0], zbuffer_blue[7:0]};

   reg [255:0]               zbuffer_data;
   wire signed [15:0]        zbuffer_x = (zbuffer_stage == 2 ? z_ret_regs[255:240] : zbuffer_data[255:240]);
   wire signed [15:0]        zbuffer_y = (zbuffer_stage == 2 ? z_ret_regs[239:224] : zbuffer_data[239:224]);
   wire signed [15:0]        zbuffer_z = zbuffer_data[223:208];
   wire [15:0]               zbuffer_red = zbuffer_data[63:48];
   wire [15:0]               zbuffer_green = zbuffer_data[47:32];
   wire [15:0]               zbuffer_blue = zbuffer_data[31:16];

   always @(posedge clk) begin
      if (zbuffer_stage == 0 && z_size > 0) begin
        zbuffer_stage <= 1;
      end
      if (zbuffer_stage == 1) begin
        zbuffer_stage <= 2;
      end
      if (zbuffer_stage == 2) begin
        zbuffer_data <= z_ret_regs;
        zbuffer_stage <= 3;
      end
      if (zbuffer_stage == 3) begin
        zbuffer_stage <= 0;
      end
   end

   // ----------------------------------- DISPLAY LOGIC --------------------------------------------

   initial counterX = 0;
   initial counterY = 0;

   reg display_stage = 0;
   wire counterXMaxed = (counterX == 639);
   wire counterYMaxed = (counterY == 479);
   assign framebuffer_mem_read = {{10{1'b0}}, counterX} * 480 + {{10{1'b0}}, counterY};

   always @(posedge clk)
     begin
        if (display_stage == 0) begin
           display_stage <= 1;
        end
        else begin
           if (counterXMaxed)
             begin
                counterX <= 0;
                if (counterYMaxed)
                  begin
                     counterY <= 0;
                  end
                else
                  counterY <= counterY + 1;
             end
           else
             counterX <= counterX + 1;
           pixel <= framebuffer_mem_out;
           display_stage <= 0;
        end
     end

endmodule
