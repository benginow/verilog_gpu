module vgademo(
               input             clk,
               output reg [23:0] pixel,
               output reg [9:0]  counterX,
               output reg [9:0]  counterY,
               output reg        done
               );
   initial counterX = 0;
   initial counterY = 0;
   initial done = 0;
   

   wire                          counterXMaxed = (counterX == 639);
   wire                          counterYMaxed = (counterY == 479);

   always @(posedge clk)
     begin
        if (counterXMaxed)
          begin
             counterX <= 0;
             if (counterYMaxed)
               begin
                  counterY <= 0;
                  done <= 1;
               end
             else
               counterY <= counterY + 1;
          end
        else
          counterX <= counterX + 1;
        pixel <= 24'hff0000;
     end

endmodule
