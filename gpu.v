 //need to schedule each pipeline to run something from work queue

begin gpu(      input            clk,
               output reg [2:0] pixel,
               output           hsync_out,
               output           vsync_out
        );


//instantiate all four queues
queue transformation(clk, );
queue lighting(clk, );
queue projection(clk, );
queue rasterization(clk, )

//two dedicated memories -- one general purpose, one for instructions
mem mem(clk, );
//we only care about reading the instruction mem in the processor, so figure that out, maybe pass instruction to processor? so processor passes pc in, and the instr_mem passes in instruction
mem instr_mem(clk, );
//current registers -- also need to pass this into instruction memory
reg curr_regs(clk, )

//schedule onto the pipeline (make this parallelized later)
//pass in program counter
reg idle;
processor proc1(clk, );

//zbuffer and framebuffer -- still need to implement these

reg[255:0] register_holder;
//if the processor is asking for more work(pointed out in the idle object), then 
always @(posedge clk) begin

end


endmodule