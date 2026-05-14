`timescale 1ns/1ps

module Lorentz_tb;

reg signed [23:0] x0;
reg signed [23:0] x1;
reg signed [23:0] x2;

wire signed [23:0] y0;
wire signed [23:0] y1;
wire signed [23:0] y2;


// DUT
Lorentz dut(
    .x0(x0),
    .x1(x1),
    .x2(x2),
    .y0(y0),
    .y1(y1),
    .y2(y2)
);


initial begin

    // default values
    x0 = 0;
    x1 = 0;
    x2 = 0;

    // read command line args
    if(!$value$plusargs("x=%d", x0))
        $display("WARNING: x not provided");

    if(!$value$plusargs("y=%d", x1))
        $display("WARNING: y not provided");

    if(!$value$plusargs("z=%d", x2))
        $display("WARNING: z not provided");

    #20;

    $display("x = %d", y0);
    $display("y = %d", y1);
    $display("z = %d", y2);

    #10;
    $finish;

end

endmodule