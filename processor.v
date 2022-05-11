
//inputs: where in memory is our first instruction (ie do we want to do )
//          which queue should we add to next (pass in the queue)
module processor(input  clk,
                input queue);

//this will not be pipelined. it takes one clock cycle to access memory (?)

//decode instruction
always @(posedge clk) begin

end

//read from queues or memory as needed
always @(posedge clk) begin

end

//execute
always @(posedge clk) begin

end

endmodule