# DosHack

## Description

The main goal of this project is to learn basics of hacking and reverse engineering.

I recieve program from [Worthlane](https://github.com/worthlane) (only .com file) that emulate password checking and then providing some important data. 

Work is divided into two parts:
1. I have to find 2 software vulnerabilities and exploit them without changing source code.
2. Write a program on C-language that will change source code and provide me an access to important data (it is king of crack).

Used software:
- Turbo debugger in order to understand how the program works
- Qview to see asm code without debugging
- C language to write crack program

## Finding first vulnerability (CIPHER)

I'm going to pay special attention to registers si and di because they are often used for buffer loading and printing (used in special commands like stosb, loadsb, etc.)

Strange idea - maybe everything is considered to be a correct password. Unfortunately, no:
![incorrect pass](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img6.png)

Firstly, I want to check the program for the most obvious vulnerability: maybe correct password is written in data unchanged. So, it's better to debug and find out where the correct password is saved.

Main body of the program is calling three different functions before terminating the program: ![three calls](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img1.png) Let's step in the first one.

![first func](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img2.png)
I'm sending 'P' letter to the input (it's equals to 50h in ASCII-code). Easy to see that program adds 4Eh to my ASCII-code and save it in ds:[di], which equals ds:[03A3], after that it increases di. Also, as I can see program does not check how many chars I have already sent to the input, which suggests that I can try to overflow bufer from ds:[di] and change some data in code.
Now, I'll send ($5B -  4E = 0D$, it's sent by enter) in order to end this function on move to the next one.

Step into the next call:
![second func](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img3.png)
In the first three line there is comparison of [si] and [di]. And di is exactly the place in which my input letters where sent. So, my suggestion is that [si] may contain correct password. Si equal to 0399h. 

This function handles two cases:
1. Buffer ds:[si] == ds:[di] -> return bx equal 0FFFFh
2. Buffer ds:[si] != ds:[di] -> set bx to 0000h and return.
Maybe, this info will help in the future.

Let's checkout 0399h and 03A3h:
![buffers](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img4.png)
$Value [03A3h] = 9E = 4E + 50h = 4E + 'P'$. It's exactly what I have sent. 
So, chars from 0399h to 03A3h are: 
- BB BD B2 B3 88 B2 B3 B9 BD 5B. 
Decoding it, knowing that there's a 4E shift:
- 6D 6F 64 65 3A 64 65 6B 6F 0D
- m o  d  e  :  d  e  k  o 

So, password is all this line because in my suggestion it have to end with 0D (because this char is also saved in buffer by func1 and checked by func2).
Let's try it:
![verified](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img5.png)

I passed the verification and now have the access to the important data.


## Finding second vulnerability (buffer overflow)
As I already mentioned, there is no buffer size checking, so I can overflow it. Looking through the code I found out that there is no addression beyond the 03A3h, so I can overflow it without ruining program code. All I need is to get to the stack and change return address to the part of the code where access is granted bypassing the verification stages.

So, first of all, I need to find address where to return. I'll enter an invalid password and check which compare leads to denied permission. 

![permission verification](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img7.png)
In main function bx is compared to FFFF. As we found out in the second function (call 0142h) this register is changed depending on whether ds:si equal ds:di or not. If it's equal it's set to FFFF. So, in case bx equals 0FFFFh program jumps on 0121h and then another function is called. This function is called regardless of bx value, but with different DI register values. So, I may suggest that 0121h is the return address I want. Let's try it. 

Func1 return address is at 0FFFCh and buffer begins at address 03A3h. So, I have to put $0FFFC - 03A3 = FC59 = 64601$ empty bytes and "D3 B3 0D", where "D3 B3" is 0121h ($D3 + 4E = 21$, $B3 + 4E = 01$, 4E is cipher shift and they are put in reverse order because of little-endian) and 0D - end of the input.

Genereting this [byte sequence](/assets/InFiles/input) with [code](/Src/Overflow/StackOverflow.cpp):
![verification stack overflow](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img8.png)


## Cracking
Cracking is pretty simple - all I need is to give me right permission all the time. There can be different ways of doing this:
- I can delete all reading password functions and also delete denying part (change them to nop)
- Replace func call before comparing bx and FFFFh with mov bx, FFFFh (change call 142h on mov bx, FFFh) ![call](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img7.png)
- Delete setting mov to 0 in func2 ![setting to 0](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img3.png)

I'll take option 3 - in my opinion it's better to not to delete some function calls because in real life cases they could be called more than once (I can forget to delete one of them) or can do more that just checking the password. So it's preferebly to change function2. Let's do it. 

Code, that I would like to delete start at byte 55 and ends at byte 58 as I can see in Qview:
![Qview](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img9.png)
All I need is to change these bytes to 3 nops with this [code](/Src/Crack/Crack.cpp). Result:
![Cracked](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img10.png)

Now, I would like to add some fun stuff using SDL graphics lib. I'll add popular meme 'Vzlob kazino'. This is the result:
![Vzlom](https://github.com/d3clane/DosHack/blob/main/assets/imgs/img11.png)



