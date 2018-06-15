module my_pe #(
        parameter L_RAM_SIZE = 6
    )
    (
        // clk/reset
        input aclk,
        input aresetn,        
        // port A
        input [31:0] ain,        
        // peram -> port B 
        input [31:0] din,
        input [31:0] cin,
        input [L_RAM_SIZE-1:0]  addr,
        input we,        
        // integrated valid signal
        input valid,        
        // computation result
        output dvalid,
        output [31:0] dout
    );
    
    wire avalid = valid;
    wire bvalid = valid;
    wire cvalid = valid;
    // peram: PE's local RAM -> Port B
    reg [31:0] bin;
    (* ram_style = "block" *) reg [31:0] peram [0:2**L_RAM_SIZE - 1];
    always @(posedge aclk)
        if (we)
            peram[addr] <= din;
        else
            bin <= peram[addr];
        
    reg [31:0] dout_fb;
    always @(posedge aclk)
       if (!aresetn)
            dout_fb <= 'd0;
       else
            if (dvalid)
                dout_fb <= dout;
            else
                dout_fb <= dout_fb;
        
    wire [47:0] multval;
    reg [31:0] mac_out;
    
    assign dout = multval[39:8];

//    always@(posedge aclk) begin
//        if(dout != 0)
//            dvalid = 1;
//        if(!aresetn)
//            dvalid = 0;
//    end

    assign dvalid = (dout != 0) || ((ain == 0) &&(bin == 0)&&(cin == 0));
    
    xbip_multadd_0 mac(//TODO assign make dout correct fixed point output
        .CLK(aclk),
        .CE(1'b1),
        .SCLR(!aresetn),
        .A(ain),
        .B(bin),
        .C({cin[23:0], 8'b0}),
        .SUBTRACT('d0),
        .P(multval)
    );
       
    endmodule

 
