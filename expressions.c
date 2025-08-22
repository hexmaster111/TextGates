
/*
Q =
    AND (   -- EXP_use_chip
        A,     -- EXP_use_wire
        B      -- EXP_use_wire
    )
    ;
*/
// void _Q_EQ_AND_A_B()
// {
//     ChipStructure s = {
//         .assignments = {
//             .items = {
//                 &(Assign){
//                     .assign_to = SLICE_CSTR("Q"),
//                     .expression = (Expr){
//                         .kind = EXP_use_chip,
//                         .name = SLICE_CSTR("AND"),
//                         .sub_expressions.items = {
//                             &(Expr){.kind = EXP_use_wire, .name = SLICE_CSTR("A")},
//                             &(Expr){.kind = EXP_use_wire, .name = SLICE_CSTR("B")},
//                         }},
//                 },
//             }}};
// }

/*
    O = BUFF -- use chip
        (
            I -- use wire
        )
*/
// void _O_EQ_BUFF_I()
// {
//     ChipStructure s = {
//         .assignments = {
//             .items = {
//                 &(Assign){
//                     .assign_to = SLICE_CSTR("O"),
//                     .expression = (Expr){
//                         .kind = EXP_use_chip,
//                         .name = SLICE_CSTR("BUFF"),
//                         .sub_expressions.items = {
//                             &(Expr){.kind = EXP_use_wire, .name = SLICE_CSTR("I")},
//                         },
//                     },
//                 },
//             },
//         },
//     };
// }

/*
C_out =  OR(AND(A,B), AND(C_in, XOR(A,B)))

C_out =
    OR --use chip
    (
        AND --use chip
        (
            A, -- use wire
            B  -- use wire
        )
        ,
        AND -- use chip
        (
            C_in -- use wire
            XOR  -- use chip
            (
                A -- use wire
                B -- use wire
            )
        )
    )

*/

// void _C_OUT__EQ_OR_AND_A_B__AND_C_IN_XOR_A_B()
// {
//     ChipStructure s = {
//         .assignments = {
//             .items = {
//                 &(Assign){
//                     .assign_to = SLICE_CSTR("C_out"),
//                     .expression = (Expr){
//                         .kind = EXP_use_chip,
//                         .name = SLICE_CSTR("OR"),
//                         .sub_expressions.items = {
//                             &(Expr){
//                                 .kind = EXP_use_chip,
//                                 .name = SLICE_CSTR("AND"),
//                                 .sub_expressions.items = {
//                                     &(Expr){
//                                         .kind = EXP_use_wire,
//                                         .name = SLICE_CSTR("A"),
//                                     },
//                                     &(Expr){
//                                         .kind = EXP_use_wire,
//                                         .name = SLICE_CSTR("B"),
//                                     }},
//                             },
//                             &(Expr){
//                                 .kind = EXP_use_chip,
//                                 .name = SLICE_CSTR("AND"),
//                                 .sub_expressions.items = {
//                                      &(Expr){.kind = EXP_use_wire,.name = SLICE_CSTR("C_IN")},
//                                      &(Expr){
//                                        .kind = EXP_use_chip,
//                                        .name = SLICE_CSTR("XOR"),
//                                        .sub_expressions.items = {
//                                            &(Expr){
//                                                .kind = EXP_use_wire,
//                                                .name = SLICE_CSTR("A"),
//                                            },
//                                            &(Expr){
//                                                .kind = EXP_use_wire,
//                                                .name = SLICE_CSTR("B"),
//                                            }},
//                                    },       
//                                 }
//                             },
//                         },
//                     }},
//             }}};
// }
