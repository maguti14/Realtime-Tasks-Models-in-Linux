CSE522: Real time embedded systems
Assignment 1 - Spring 2018

Team members:
Vu Coughlin - 1206036828
Mauricio Gutierrez - 1206284660


----------------------------------------------------
				System Requirements
----------------------------------------------------
- Linux (Ubuntu 16.04)
- gcc
- make
- cross compiler for Galileo Gen2 


----------------------------------------------------
	To compile our program, there are two options:
----------------------------------------------------
1) To compile with native linux compiler, run:
$ make
or 
$ make native

2) To compile with cross compiler, run:
$ make cross

*** Note *** : Before compile or re-compile the program, please make sure to run clean so there's no side effect:
$ make clean


----------------------------------------------------
	To run our program, please run:
----------------------------------------------------
- Please run our program with root permission
$ sudo ./homework1 [args]


- The program expects 1 argument from the user to enable or not enable priority inheritence (PI). Thus run the program with 0 for disable PI, 1 for enable PI.

Usage: sudo ./homework1 [argv] 

$ sudo ./homework1 0		// for disable PI
$ sudo ./homework1 1		// for enable PI



----------------------------------------------------
	Command used to trace program:
----------------------------------------------------
- We tested our code with trace-cmd, kernelshark, and ftrace. We have attached a sample of the trace-cmd below:

$ sudo trace-cmd record -e sched_switch -e sched_wakeup numactl --physcpubind=0 ./homework1 0 < testcase9.txt

