// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
(KEYCHECK)
@24576
D=M
@BLACK
D;JNE
@WHITE
D;JEQ

(WHITE)
@0
D=A
@bw
M=D
@FLIP
0;JMP

(BLACK)
@0
D=!A
@bw
M=D
@FLIP
0;JMP

(FLIP)
@8191
D=A
@i
M=D

(FLIPLOOP)
@i
D=M
@SCREEN
D=A+D
@pixel
M=D
@bw
D=M
@pixel
A=M
M=D
@i
MD=M-1

@FLIPLOOP
D;JGE

@KEYCHECK
0;JMP