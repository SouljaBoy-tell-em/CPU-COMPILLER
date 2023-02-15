# CPU AND MY COMPILLER

this is a pretty big project. The assembler takes as input a list of commands that you enter from the keyboard and sends them to a binary file. 
The binary file is decoded in the virtual processor I created and as a result the commands are executed. 
The program uses: temporary variables, register variables, a variable from virtual memory. The final execution is in the graphical dump.

## commands:

### push:

push 10         - contain number "10" in the STACK;

push rax        - record the last element of the STACK in register RAX;

push [10]       - record the last element of the STACK in the 10th cell RAM;

push [rax + 10] - record the register RAX in the 10th cell RAM;

### pop:

pop             - destroy the last element of the STACK;

pop rax         - record the last element of the STACK in the register RAX;

pop [10]        - record the last element of the STACK in the 10th cell RAM;

### jump: use for cycle or condition

jb  <

jbe <=

ja  >

jae >=

je  =

jne ==

### other commands: 

add             - sum operator of the STACK;

sub             - difference operator of the STACK;

mul             - multiplication operator of the STACK;

div             - division operator of the STACK;

pow             - degree operator of the STACK. It applied to the last element of the stack and pushes again;

sqrt            - square root operator of the STACK. It applied to the last element of the stack and pushes again;

in              - input number from keyboard and push at the STACK;

out             - print in console the last element of the STACK;

hlt             - kill process;

endl            - print in console the symbol '\n';

space.          - print int console the symbol ' ';


### create cycle (for). If you want to create cycle, you should insert next contruction: for (i = 1; i < 7; i+=2)

push 1

pop rax

start:

push 2

push rax

add

pop rax

...

push 7

jb :start


start - label;

### create condition (if). If you want to create condition, you should insert next contruction: if (rax < 5)

push 22
pop rax
push 5
jb :start
...
start:

start - label;
label is pair of words between which actions take place.
