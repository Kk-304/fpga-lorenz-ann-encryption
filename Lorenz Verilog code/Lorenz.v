module Lorentz(

    input  signed [23:0] x0,
    input  signed [23:0] x1,
    input  signed [23:0] x2,

    output signed [23:0] y0,
    output signed [23:0] y1,
    output signed [23:0] y2
);

parameter SHIFT = 20;

// ---------------- fixed point multiply ----------------
function signed [23:0] qmul;
    input signed [23:0] a;
    input signed [23:0] b;
    reg signed [47:0] mult;
begin
    mult = a * b;
    qmul = mult >>> SHIFT;
end
endfunction

// =======================================================
// ---------------- LAYER 1 WEIGHTS ----------------
// =======================================================

parameter signed [23:0] W1_00 = -24'sd483386;
parameter signed [23:0] W1_01 =  24'sd1360136;
parameter signed [23:0] W1_02 = -24'sd563602;
parameter signed [23:0] W1_03 =  24'sd158334;
parameter signed [23:0] W1_04 =  24'sd160903;

parameter signed [23:0] W1_10 = -24'sd619757;
parameter signed [23:0] W1_11 =  24'sd478783;
parameter signed [23:0] W1_12 =  24'sd1198253;
parameter signed [23:0] W1_13 =  24'sd978643;
parameter signed [23:0] W1_14 =  24'sd479348;

parameter signed [23:0] W1_20 = -24'sd484877;
parameter signed [23:0] W1_21 = -24'sd445235;
parameter signed [23:0] W1_22 =  24'sd21573;
parameter signed [23:0] W1_23 =  24'sd8717;
parameter signed [23:0] W1_24 =  24'sd874036;

// ---------------- LAYER 1 BIAS ----------------

parameter signed [23:0] B1_0 =  24'sd0;
parameter signed [23:0] B1_1 =  24'sd80477;
parameter signed [23:0] B1_2 = -24'sd25026;
parameter signed [23:0] B1_3 = -24'sd544986;
parameter signed [23:0] B1_4 = -24'sd39321;

// =======================================================
// LAYER 1 COMPUTE
// =======================================================

wire signed [23:0] a1_0, a1_1, a1_2, a1_3, a1_4;

assign a1_0 = qmul(x0,W1_00) + qmul(x1,W1_10) + qmul(x2,W1_20) + B1_0;
assign a1_1 = qmul(x0,W1_01) + qmul(x1,W1_11) + qmul(x2,W1_21) + B1_1;
assign a1_2 = qmul(x0,W1_02) + qmul(x1,W1_12) + qmul(x2,W1_22) + B1_2;
assign a1_3 = qmul(x0,W1_03) + qmul(x1,W1_13) + qmul(x2,W1_23) + B1_3;
assign a1_4 = qmul(x0,W1_04) + qmul(x1,W1_14) + qmul(x2,W1_24) + B1_4;

// =======================================================
// RELU
// =======================================================

wire signed [23:0] ys0, ys1, ys2, ys3, ys4;

assign ys0 = (a1_0 > 0) ? a1_0 : 24'sd0;
assign ys1 = (a1_1 > 0) ? a1_1 : 24'sd0;
assign ys2 = (a1_2 > 0) ? a1_2 : 24'sd0;
assign ys3 = (a1_3 > 0) ? a1_3 : 24'sd0;
assign ys4 = (a1_4 > 0) ? a1_4 : 24'sd0;

// =======================================================
// ---------------- LAYER 2 ----------------
// =======================================================

parameter signed [23:0] W2_00 =  24'sd502625;
parameter signed [23:0] W2_01 =  24'sd450664;
parameter signed [23:0] W2_02 = -24'sd99673;

parameter signed [23:0] W2_10 =  24'sd593494;
parameter signed [23:0] W2_11 =  24'sd268591;
parameter signed [23:0] W2_12 = -24'sd322272;

parameter signed [23:0] W2_20 = -24'sd233657;
parameter signed [23:0] W2_21 =  24'sd850934;
parameter signed [23:0] W2_22 = -24'sd421661;

parameter signed [23:0] W2_30 =  24'sd1214;
parameter signed [23:0] W2_31 =  24'sd7605;
parameter signed [23:0] W2_32 =  24'sd287820;

parameter signed [23:0] W2_40 =  24'sd307498;
parameter signed [23:0] W2_41 =  24'sd114750;
parameter signed [23:0] W2_42 =  24'sd1081346;

// ---------------- LAYER 2 BIAS ----------------

parameter signed [23:0] B2_0 = -24'sd57509;
parameter signed [23:0] B2_1 =  24'sd1983;
parameter signed [23:0] B2_2 =  24'sd120164;

// =======================================================
// LAYER 2 COMPUTE
// =======================================================

wire signed [23:0] z2_0, z2_1, z2_2;

assign z2_0 =
    qmul(ys0,W2_00) +
    qmul(ys1,W2_10) +
    qmul(ys2,W2_20) +
    qmul(ys3,W2_30) +
    qmul(ys4,W2_40) +
    B2_0;

assign z2_1 =
    qmul(ys0,W2_01) +
    qmul(ys1,W2_11) +
    qmul(ys2,W2_21) +
    qmul(ys3,W2_31) +
    qmul(ys4,W2_41) +
    B2_1;

assign z2_2 =
    qmul(ys0,W2_02) +
    qmul(ys1,W2_12) +
    qmul(ys2,W2_22) +
    qmul(ys3,W2_32) +
    qmul(ys4,W2_42) +
    B2_2;

// =======================================================
// OUTPUT
// =======================================================

assign y0 = z2_0;
assign y1 = z2_1;
assign y2 = z2_2;

endmodule