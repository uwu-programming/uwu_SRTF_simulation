# Shortest Remaining Time First Process Scheduling Simulator
This shortest reamining time first (SRTF) process scheduling simulator simulates how an operating system would schedule the available processes in a system, and allocate the processes to the available processor to complete their execution.

This simulator uses **calculator** and **mathematical expression** to simulate the functionality of a process scheduler, on how it would allocate the processes (mathmatical expression) and deal with problem such as data dependency (operator precedence).

This simulator program is written in **C** programming language; the UI is done with C's **GTK** (GTK4) library.

## Getting started
### Running executable file on Windows
To run the simulator program directly on Windows operating system, navigate to the [bin](bin/) directory. The directory should consist of all required `.dll` file and a [main.exe](bin/main.exe) executable file to allow directly running the program.

### Manually compile the .c files
To compile the `.c` files into an executable file, you will need to have a C compiler alongside with the GTK4 dependency. The following section provide a step by step guide on how to use [MSYS2](https://www.msys2.org/) software development environment to install a C compiler alongside with the GTK4 dependency.

1. Visit [MSYS2](https://www.msys2.org/#installation) website and download the installer.
2. Run the downloaded installer to install MSYS2 to your device.
3. After the installation is completed, run the installed `msys2.exe` and you shall see a MSYS2 shell similar as below: ![An image of MSYS2 shell](miscellaneous/msys2exe.png)
4. Run the following command in the MSYS2 shell to install a gcc compiler for compiling C code:<br> `pacman -S mingw-w64-x86_64-gcc`
5. After the previous command has completed its execution, run the following command to install GTK4 packages:<br>
`pacman -S mingw-w64-x86_64-gtk4`
6. After that, run the following command to install the toolchain: <br>`pacman -S mingw-w64-x86_64-toolchain base-devel`
7. Then, locate the `bin` directory where the gcc compiler is installed to (e.g. `C:/msys64/mingw64/bin`) and add this directory to your computer's environment path.
8. Close and restart the MSYS2 shell and test if the gcc compiler has successfully installed by running the following command:<br> `where gcc`
9. If it is verified that the gcc compiler has successfully installed, you can then use the MSYS2 shell to change the directory into the [simulator directory](simulator/).
10. Finally, to compile the C code, run: <br>`gcc $(pkg-config --cflags gtk4) -o main main.c $(pkg-config --libs gtk4) -I../data_structure -I../math_utility -I../scheduler ../data_structure/linked_list.c ../data_structure/stack.c ../math_utility/expression_notation.c ../scheduler/process_data_dependency.c ../scheduler/process_scheduler.c ../scheduler/virtual_process.c ../scheduler/processor.c`
11. You should see the compiler compile the C code and output `main.exe` at the end; you can then run the executable file to start the simulator program.

## Using the simulator
When you first start the simulator, you should see the following state:
![Default simulator state](miscellaneous/simulator.png)

### Process
A **process** in this simulator is represented by a **mathematical expression**. Each mathematical expression can contain several operand and operator. While each **operation** in an expression represents a **thread** of a process. E.g. the expression `(a+b)*(c+d)` can represent a process; while the operation `(a+b)`, `(c+d)`, and `(ans1)*(ans2)` each represents a thread of the process respectively.

You can add a process (expression) to the simulator by clicking the **Add process** button at the lower part of the simulator. A valid expression can be constructed by combining lower case alphabet as operand, alongside with special characters that represent operator.

#### Operand
Operand can be represented by lower case alphabet: `abcdefghijklmnopqrstuvwxyz`
#### Operator
Operator can be represented by special characters:
- `+` as addition
- `-` as deduction
- `*` as multiplication
- `/` as division
- `( )` as parentheses
  
#### Example 
- `(a+b) * (c+d)`
- a + b * c
- a - b - c

#### Operator precedence
The simulator supports operator precedence as the following:
- Precedence 3: `( )`
- Precedence 2: `*`, `/`
- Precedence 1: `+`, `-`

A higher precedence means the operation would be calculated first before the other operations being calculated.

### Reset simulator
The **Reset simulator** button allows the user to reset the simulator and start off a new session by entering the amount of processor in the new simulation.

### Auto
The **Auto** button allows the simulator to automatically proceed to next timeframe every 1 second.

### Stop auto
The **Stop auto** button stops the simulator from automatically proceed to the next timeframe.

### Next timeframe
The **Next timeframe** button instructs the simulator to proceed to the next timeframe. (only available when Auto is stopped)

## Simulation
The simulation is done by simulating an expression as a computer process; while each operation in an expression can be considered as a thread. The burst time of a process is represented by the amount of operation inside an expression.
For example, the expression `a+b+c+d` would have a burst time of 3 as there exists 3 operator for 3 operation. 

And to simulate a processor executing a process, consider that each operation represents a thread and that each thread takes 1 unit time to be completed. Therefore, the operation `a+b` takes 1 unit time to be completed, and the simulator would represent the answer of `a+b` with a new variable. For example, the expression `a+b+c+d`:
1. `a+b` completed in `t1` and the simulator represents the answer of it as `A`.
2. Then at `t2` the simulator can proceed to the next thread, that is `A+c`, and represent the answer of `A+c` with a new variable, `B`.
3. Then at `t3` the simulator can proceed with the last thread of `B+d` and represent the answer with a new variable, `C`.

|         | t0 - t1 | t1 - t2 | t2 - t3 |
|---------|---------|---------|---------|
| **C1**  | a+b = A | A+c = B | B+d = C |

*`t1`, `t2`, `t3` represents timeframe 1, 2, 3 respectively

*C1 represents processor core 1

### Multicore simulation and data dependency
Threads of a process can be completed concurrently if data dependency does not exist. For example, the expression `(a+b) * (c+d)`, the operation `(a+b)` and `(c+d)` does not depends on each other, and in a multicore simulation, these two threads can be done concurrently. For example, the expression `(a+b)*(c+d)`
|         | t0 - t1 | t1 - t2 |
|---------|---------|---------|
| **C1**  | a+b = A | A*B = C |
| **C2**  | c+d = B |   N/A   |
| **C3**  |   N/A   |   N/A   |