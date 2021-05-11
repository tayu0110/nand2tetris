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
@ARG
D=M
@1
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
@R13
M=D
@THIS
D=A
@1
D=D+A
@R14
M=D
@R13
D=M
@R14
A=M
M=D
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
@THAT
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
@R13
M=D
@THAT
D=M
@1
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
@2
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
(MAIN_LOOP_START)
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
@COMPUTE_ELEMENT
D;JNE
@END_PROGRAM
0;JMP
(COMPUTE_ELEMENT)
@THAT
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@1
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
@THAT
D=M
@2
D=D+A
@R14
M=D
@R13
D=M
@R14
A=M
M=D
@THIS
D=A
@1
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
M=M+D
@SP
AM=M-1
D=M
@R13
M=D
@THIS
D=A
@1
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
@MAIN_LOOP_START
0;JMP
(END_PROGRAM)
(END)
@END
0;JMP
