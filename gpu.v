 //need to schedule each pipeline to run something from work queue

begin gpu(      input            clk,
               output reg [2:0] pixel,
               output           hsync_out,
               output           vsync_out
        );


//instantiate all four queues


//two dedicated memories -- one general purpose, one for instructions
mem mem(clk, );
//we only care about reading the instruction mem in the processor, so figure that out, maybe pass instruction to processor
mem instr_mem(clk, );

//schedule onto the pipeline (make this parallelized later)
//pass in program counter
processor proc1(clk, );

endmodule