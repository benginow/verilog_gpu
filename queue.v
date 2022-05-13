//inputs: reading = am i reading from the queue right now
//        adding = am i adding to the queue
//outputs: removed register set          
begin queue(
    input clk,
    //TODO: rename things!
    input reading, output[255:0] returned_registers,
    input adding, input[255:0] registers_to_add, 
    output[15:0] size,  output err
);

reg [255:0]data[0:16'hffff];
reg [255:0] returned_registers;
//assuming we can only keep 65536 items in the queue
reg[15:0] size = 0;
reg[15:0] front = 0;
reg[15:0] back = 0;

always @(posedge clk) begin
    if (reading) begin
        if (size > 0) begin
            registers_to_add <= data[front];
            front <= (front == 0) ? 16'hfffe : front - 1;
        end
        else begin 
            err <= 1;
        end 
    end
    if (adding) begin 
        if (size < 16'hfffe) begin
            back <= back + 1;
            data[back] <= registers_to_add;
        end
        else begin
            err <= 1
        end
    end
end

endmodule