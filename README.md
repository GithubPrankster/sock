# Stochastically Observed Computing Kaleidoscope
Sock is a bytecode vm that features just enough to be able to do "meaningful" computing of things. Constructed entirely in C, 
using 4 standard libraries and in around ~140 LOC.
## Compilation & Duration
`gcc -static -march=native -mtune=native -s sock.c -o sock`

As compiled with GCC 10.0.1, `bdver2` architecture, 16GB RAM, running `time ./sock test.sock` yielded:
```
real    0m0.042s
user    0m0.015s
sys     0m0.015s  
```
## The Features
* 16 instructions
* 2048 bytes of memory
* 4 8-bit registers
* Subroutine support (only 1 level)
* Around 4 modes of operation (number, register, memory, memory number, and some mixing)
* VM halting through exploitation of return instruction (0a, turned to 3a will halt operation)
* File reading
* Jumping, program self-modification potential
## In Action
![scoobis](https://cdn.discordapp.com/attachments/517928649727541248/713124772082614302/notepad_2020-05-21_17-09-58.png)

### But why?
I challenged myself to write this in one day. I love this sorta stuff xD
