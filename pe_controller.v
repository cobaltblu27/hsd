`timescale 1ns / 1ps

module pe_con#(
       parameter VECTOR_SIZE = 64, // matrix and vector size
	   parameter L_RAM_SIZE = 12,  // size of RAM on PE_CONTROLLER : we store entire 64*64 matrix on this L_RAM
	   parameter R_RAM_SIZE = 6    // size of RAM on each PE : we store 64 vector on this R_RAM
    )
    (
        input start,
        output done,
        input aclk,
        input aresetn,
        //output [L_RAM_SIZE:0] rdaddr,
	    //input [31:0] rddata,
	    //output reg [31:0] wrdata,
	    
	    // to BRAM
	    output [31:0] BRAM_ADDR,
	    output [31:0] BRAM_WRDATA,
	    output [3:0] BRAM_WE,
	    output BRAM_CLK,
	    input [31:0] BRAM_RDDATA
);
   // PE
    wire [31:0] ain;
    wire [31:0] din;
    wire [R_RAM_SIZE-1:0] r_addr; // address of R_RAMs
    wire we_local;
    wire we_global;
    //wire we;
    wire valid;
    wire dvalid;
    wire [31:0] dout;
   
    wire [L_RAM_SIZE:0] rdaddr;  // since we have to read 64*64+64 vals from BRAM, we need 13 bits for addressing
    wire [31:0] rddata;
    
    reg [31:0] wrdata;
    
    wire [L_RAM_SIZE-1:0] l_addr; // address of L_RAM
    
    clk_wiz_0 u_clk (.clk_out1(BRAM_CLK), .clk_in1(aclk));
   
   // L_RAM : read first 64*64 vals from BRAM into L_RAM
    reg [31:0] gdout;
    (* ram_style = "block" *) reg [31:0] globalmem [0:VECTOR_SIZE*VECTOR_SIZE-1]; // since we store entire 64*64 matrix on
                                                                                  // L_RAM, size should be like this!
    always @(posedge aclk)
        if (we_global)
            globalmem[l_addr] <= rddata;
        else
            gdout <= globalmem[l_addr];

  
	//FSM
    // transition triggering flags
    wire load_done;
    wire calc_done;
    wire done_done;
        
    // state register
    reg [3:0] state, state_d;
    localparam S_IDLE = 4'd0;
    localparam S_LOAD = 4'd1;
    localparam S_CALC = 4'd2;
    localparam S_DONE = 4'd3;

	//part 1: state transition
    always @(posedge aclk)
        if (!aresetn)
            state <= S_IDLE;
        else
            case (state)
                S_IDLE:
                    state <= (start)? S_LOAD : S_IDLE;
                S_LOAD: // LOAD PERAM
                    state <= (load_done)? S_CALC : S_LOAD;
                S_CALC: // CALCULATE RESULT
                    state <= (calc_done)? S_DONE : S_CALC;
                S_DONE:
                    state <= (done_done)? S_IDLE : S_DONE;
                default:
                    state <= S_IDLE;
            endcase
    
    always @(posedge aclk)
        if (!aresetn)
            state_d <= S_IDLE;
        else
            state_d <= state;

	//part 2: determine state
    // S_LOAD : 1. Load 64 * 64 vals from BRAM to L_RAM 
    //          2. LOAD 64 vals from BRAM to R_RAMS
    reg load_flag;
    wire load_flag_reset = !aresetn || load_done;
    wire load_flag_en = (state_d == S_IDLE) && (state == S_LOAD);
    // CNTLOAD1 : We need to count until 64*64+64 vals are all loaded onto local RAMs. 
    // We multiply 2, 1 cycle for reading val from BRAM and the other for writing the val to RAM.
    localparam CNTLOAD1 = (VECTOR_SIZE * VECTOR_SIZE + VECTOR_SIZE) * 2 - 1; 
    always @(posedge aclk)
        if (load_flag_reset)
            load_flag <= 'd0;
        else
            if (load_flag_en)
                load_flag <= 'd1;
            else
                load_flag <= load_flag;
    
    // S_CALC
    reg calc_flag;
    wire calc_flag_reset = !aresetn || calc_done;
    wire calc_flag_en = (state_d == S_LOAD) && (state == S_CALC);
    localparam CNTCALC1 = (VECTOR_SIZE) - 1;
    always @(posedge aclk)
        if (calc_flag_reset)
            calc_flag <= 'd0;
        else
            if (calc_flag_en)
                calc_flag <= 'd1;
            else
                calc_flag <= calc_flag;
    
    // S_DONE
    reg done_flag;
    wire done_flag_reset = !aresetn || done_done;
    wire done_flag_en = (state_d == S_CALC) && (state == S_DONE);
    localparam CNTDONE = 5;
    always @(posedge aclk)
        if (done_flag_reset)
            done_flag <= 'd0;
        else
            if (done_flag_en)
                done_flag <= 'd1;
            else
                done_flag <= done_flag;
    
    
    // down counter
    reg [31:0] counter;
    wire [31:0] ld_val = (load_flag_en)? CNTLOAD1 :
                         (calc_flag_en)? CNTCALC1 : 
                         (done_flag_en)? CNTDONE  : 'd0;
    wire counter_ld = load_flag_en || calc_flag_en || done_flag_en;
    wire counter_en = load_flag || dvalid || done_flag;
    wire counter_reset = !aresetn || load_done || calc_done || done_done;
    always @(posedge aclk)
        if (counter_reset)
            counter <= 'd0;
        else
            if (counter_ld)
                counter <= ld_val;
            else if (counter_en)
                counter <= counter - 1;
    
    //part3: update output and internal register
    //S_LOAD: we
    
    // we_local : we have to start writing vals to R_RAMs when there are only 64 vals left to copy
	assign we_local = (load_flag && (counter < VECTOR_SIZE * 2) && !counter[0]) ? 'd1 : 'd0;
	
    // we_global : we have to continue writing vals to L_RAM until only 64 vals left to copy
	assign we_global = (load_flag && (counter >= VECTOR_SIZE * 2) && !counter[0]) ? 'd1 : 'd0;
	
	//S_CALC: wrdata 
   always @(posedge aclk)
        if (!aresetn)
                wrdata <= 'd0;
        else
            if (calc_done)
                    wrdata <= dout;
            else
                    wrdata <= wrdata;

	//S_CALC: valid
    reg valid_pre, valid_reg;
    always @(posedge aclk)
        if (!aresetn)
            valid_pre <= 'd0;
        else
            if (counter_ld || counter_en)
                valid_pre <= 'd1;
            else
                valid_pre <= 'd0;
    
    always @(posedge aclk)
        if (!aresetn)
            valid_reg <= 'd0;
        else if (calc_flag)
            valid_reg <= valid_pre;
     
    assign valid = (calc_flag) && valid_reg;
    
	//S_CALC: ain
	assign ain = (calc_flag)? gdout : 'd0;

	//S_LOAD : get r_addr and l_addr
    // r_addr => S_LOAD : 63 - (counter / 2)
    assign r_addr = (load_flag)? ((counter < VECTOR_SIZE*2)? (VECTOR_SIZE-1) - (counter/2) : 'd0):
                    (calc_flag)? counter[L_RAM_SIZE-1:0]: 'd0;
    
    // l_addr => S_LOAD : 4159 - (counter / 2)
    assign l_addr = (load_flag)? (VECTOR_SIZE*VECTOR_SIZE+VECTOR_SIZE-1) - (counter/2) :
                     'd0; 

	//S_LOAD
	assign din = (load_flag)? rddata : 'd0;
    // rdaddr = 4159 - (counter / 2)
    assign rdaddr = (state == S_LOAD)? (VECTOR_SIZE*VECTOR_SIZE+VECTOR_SIZE-1) - (counter/2) : 'd0;

	//done signals
    assign load_done = (load_flag) && (counter == 'd0);
    assign calc_done = (calc_flag) && (counter == 'd0) && dvalid;
    assign done_done = (done_flag) && (counter == 'd0);
    assign done = (state == S_DONE) && done_done;
    
    
    // BRAM interface
    assign rddata = BRAM_RDDATA;
    assign BRAM_WRDATA = wrdata;
    
    assign BRAM_ADDR = (done_flag_en)? 0 : { {29-L_RAM_SIZE{1'b0}}, rdaddr, 2'b00};
    assign BRAM_WE = (done_flag_en)? 4'hF : 0;
    
    
    my_pe #(
        .L_RAM_SIZE(R_RAM_SIZE)
    ) u_pe (
        .aclk(aclk),
        .aresetn(aresetn && (state != S_DONE)),
        .ain(ain),
        .din(din),
        .addr(r_addr),
        .we(we_local),
        .valid(valid),
        .dvalid(dvalid),
        .dout(dout)
    );

endmodule
