

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
<!-- C_out -->