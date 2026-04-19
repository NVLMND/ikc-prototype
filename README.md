# IKC: I Know C (prototype)

write C, run it, repeat without wrapping everything in `main()` or going through the usual compile and run cycle every time.
this is a small REPL like environment for C that buffers your input and executes it on demand.

```
int x = 5;
printf("%d\n", x);
run
5
[ran with tcc]
```

no boilerplate, no setup needed only code and execution.
input is collected internally and classified into global declarations and executable code. you can write things in any order, including headers and globals at any point, and they’ll still be handled correctly when the program is built and executed.
execution is fast by default using `tcc`, with automatic fallback to `gcc` if needed.

```
int y = 7;
#include<stdio.h>
int x = 3;
printf("%d\n", x + y);
run
10
[ran with tcc]
```

basic interaction is simple:
- `run`- compile and execute buffered code  
- `clear`- reset the buffer  
- `quit`- exit  

this version includes:
- no `main()` requirement  
- buffered execution model  
- automatic classification (globals / body)  
- flexible input order (limited) 
- custom input handling (readline + dynamic buffer)  
- basic line editing (left/right movement)  
- fast compile cycle via `tcc` (fallback: `gcc`)

build and run: make && ./ikc


## note
this repository contains a minimal version built during a hackathon, the goal here is to showcase the core mechanism, not the full feature set.
several higher-level features have been intentionally excluded.
