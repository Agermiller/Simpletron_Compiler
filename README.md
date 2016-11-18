# Simpletron_Compiler

The code you see is my take on a two part problem from Deitel's C++ Programming book. Programs written in a programming language called 'Simple' will be converted to Simple Machine Language (SML) and executed. This project is not a true compiler but a virtual one, since the machine language code is not actually commanding any harware directly, but is instead using C++.

##Part 1

Part 1 of this challenge began in chapter 8, which required that a program be written to evaluate a very basic machine language code. The machine language syntax is a 4 digit number, where the leading two digits describe the operation and the trailing two digits describe the location in memory of which to run the operation against.

**OPERATION CODES:**
+ *Input/Output operations:* 
  + READ = 10;  WRITE = 11;   
+ *Load and store operations:*
  + LOAD = 20; STORE = 21;
+ *Arithmetic operations:*
  + ADD = 30; SUBTRACT = 31;  DIVIDE = 32;  MULTIPLY = 33;
+ *Transfer-of-control operations:* 
  + BRANCH = 40;  BRANCHNEG = 41; BRANCHZERO = 42; HALT = 43;
  
Take the following program as an example, which reads two numbers from the keyboard and prints their sum.

| Location      | Number        | Instruction  |
| ------------- |:-------------:| ------------:|
|00             | +1007         |Read A        |
|01             | +1008         |Read B        |
|02             | +2007         |Load A        |
|03             | +3008         |Add B         |
|04             | +2109         |Store C       |
|05             | +1109         |Write C       |
|06             | +4300         |Halt          |
|07             | +0000         |Variable A    |
|08             | +0000         |Variable B    |
|09             | +0000         |Result C      |


##Part 2

Part 2 of this challenge continues in chapter 20, and is an extension to the above challenge. This program evaluates a basic high-level language called Simple (similar to early versions of the language BASIC), converts it to Simple machine language (SML) code, and then executes the SML code.

These are the following acceptable commands for the Simple language. 
Each statement begins with a line number. The actual value of the line is arbitrary, so long as the lines continue forth in ascending order.

| Command       | Example             |
| ------------- |---------------------|
|rem            |10 rem this is a remark |
|input          |20 input x           |
|let            |30 let a = 1 + (b * 2)|
|print          |40 print a           |
|goto           |50 goto 60           |
|if...goto      |60 if a == x goto 70 |
|end            |70 end               |

Consider the following Simple code as an example, which reads 3 digits from the keyboard and prints the average. 

| Simple Program      |
| ------------------- |
|10 rem  calculate the average of 3 integers|
|20 input a|
|30 input b|
|40 input c|
|50 let d = (a + b + c) /3|
|60 rem  the average will be printed|
|70 print d|
|80 end|

The program will then generate SML code based on the Simple input. Once the SML is generated and evaluated, the appropriate prompts will then be shown in the console.

| Simple Program      |SML Location & Instruction| Description  |
| ------------------- |:------------------------:| ------------ |
|10 rem  calculate the average of 3 integers| *none*  |`rem` ignored|
|20 input a                                 |00  +1020|read `a` into location 20|
|30 input b                                 |01  +1019|read `b` into location 19|
|40 input c                                 |02  +1018|read `c` into location 18|
|50 let d = (a + b + c) /3                  |03  +2020|load `a` (20) into the accumulator|
|                                           |04  +3019|add `b` (19) to accumulator|
|                                           |05  +2115|store result in temporary location 15|
|                                           |06  +2015|load from temporary location 15|
|                                           |07  +3018|add `c` (18) to accumulator|
|                                           |08  +2114|store result in temporary location 14|
|                                           |09  +2014|load from temporary location 14|
|                                           |10  +3216|divide loaded variable by `3` (16)|
|                                           |11  +2117|store result into `d` (17)|
|60 rem  the average will be printed        | *none*  |`rem` ignored|
|70 print d                                 |12  +1117|output `d` to screen|
|80 end                                     |13  +4300|terminate execution|
| *none*                                    |14  +0000|memory location for temp variable| 
| *none*                                    |15  +0000|memory location for temp variable|
| *none*                                    |16  +0003|memory location for the constant `3`|
| *none*                                    |17  +0000|memory location for variable `d`|
| *none*                                    |18  +0000|memory location for variable `c`|
| *none*                                    |19  +0000|memory location for variable `b`|
| *none*                                    |20  +0000|memory location for variable `a`|



PDF of the Simple language chapter problem [here](http://www.deitel.com/bookresources/cpphtp7/CompilerExercise.pdf).

##Limitations

The buffer size of this compiler is only 100 elements. Notice how much more SML code was created in response to the Simple code. This means that the compiler could run out of memory if the submitted Simple program is too large or contains many complex arithmetic statements. 

This program also only works with integer arithmetic, meaning if the above program was executed with the three numbers 3, 3 & 5, the average would print as 3 instead of 3.66. Also, the only arithmetic operators currently supported are `+`, `-`, `*`, and `/`, though I plan to allow for operators `^` and `%` in the future.
