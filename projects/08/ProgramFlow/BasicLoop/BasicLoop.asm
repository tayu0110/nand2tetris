// @256
// D=A
// @SP
// M=D
// @RETURN:Sys.init:0
// D=A
// @SP
// A=M
// M=D
// @SP
// M=M+1
// @LCL
// D=M
// @SP
// A=M
// M=D
// @SP
// M=M+1
// @ARG
// D=M
// @SP
// A=M
// M=D
// @SP
// M=M+1
// @THIS
// D=M
// @SP
// A=M
// M=D
// @SP
// M=M+1
// @THAT
// D=M
// @SP
// A=M
// M=D
// @SP
// M=M+1
// @SP
// D=M
// @LCL
// M=D
// @5
// D=D-A
// @ARG
// M=D
// @Sys.init
// 0;JMP
(RETURN:Sys.init:0)
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@R13
M=D
@LCL
D=M
@0
D=D+A
@R14
M=D
@R13
D=M
@R14
A=M
M=D
(LOOP_START)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M+D
@SP
AM=M-1
D=M
@R13
M=D
@LCL
D=M
@0
D=D+A
@R14
M=D
@R13
D=M
@R14
A=M
M=D
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@SP
AM=M-1
D=M
@R13
M=D
@ARG
D=M
@0
D=D+A
@R14
M=D
@R13
D=M
@R14
A=M
M=D
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@LOOP_START
D;JNE
@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
(END)
@END
0;JMP
