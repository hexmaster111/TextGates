
### _1bit_full_adder
``` mermaid
flowchart LR
IN_A["A"]
IN_B["B"]
IN_Carry["CARRY"]
OUT_Sum["SUM"]
OUT_Carry["CARRY"]
XOR0["XOR"]
XOR1["XOR"]
AND0["AND"]
AND1["AND"]
OR0["OR"]
IN_A --> XOR0
IN_B --> XOR0
XOR0 --> XOR1
IN_Carry --> XOR1
XOR0 --> AND0
IN_Carry --> AND0
IN_A --> AND1
IN_B --> AND1
AND0 --> OR0
AND1 --> OR0
OR0 --> OUT_Carry
XOR1 --> OUT_Sum
```

### _4bit_full_adder
``` mermaid
flowchart LR

%% INPUTS
A_1
A_2
A_3
A_4

B_1
B_2
B_3
B_4

C_In

%% OUTPUTS
S_1
S_2
S_3
S_4

C_Out


%% LOGIC
addr_1
addr_2
addr_3
addr_4

%% CONNECTIONS
A_1 --->|A|addr_1 
A_2 --->|A|addr_2 
A_3 --->|A|addr_3 
A_4 --->|A|addr_4 
B_1 --->|B|addr_1 
B_2 --->|B|addr_2 
B_3 --->|B|addr_3 
B_4 --->|B|addr_4



%% Wire C_In connects to pin C_in of chip addr_1
C_In --->|C_in|addr_1

addr_1 --->|C_out --- C_in| addr_2 
addr_2 --->|C_out --- C_in| addr_3
addr_3 --->|C_out --- C_in| addr_4
addr_4 --->|C_out|C_Out


addr_1 --->|Sum|S_1
addr_2 --->|Sum|S_2
addr_3 --->|Sum|S_3
addr_4 --->|Sum|S_4

%% src_chip_or_wire--->|src_pin---------dst_pin|dst_chip

```