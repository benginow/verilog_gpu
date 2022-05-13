//inputs: reading = am i reading from the queue right now
//        adding = am i adding to the queue
//outputs: removed register set          
module queue(input          clk,
             //TODO: rename things!
             input          reading,
             output reg [255:0] returned_registers,
             input          adding,
             input reg [255:0]  registers_to_add, 
             output [15:0]  size,
             output         err);
   reg [255:0]              data[0:16'hffff];
   //assuming we can only keep 65536 items in the queue
   reg [15:0]               front = 0;
   reg [15:0]               back = 0;
   reg [15:0]               size_ = 0;
   assign size = size_;

   always @(posedge clk) begin
      if (reading) begin
         if (size_ > 0) begin
            returned_registers <= data[front];
            front <= front + 1;
            size_ <= size_ - 1;
         end
         else begin
            err <= 1;
         end
      end
      if (adding) begin
         if (size_ < 16'hfffe) begin
            back <= back + 1;
            data[back] <= registers_to_add;
            size_ <= size_ + 1;
         end
         else begin
            err <= 1;
         end
      end
   end

endmodule
