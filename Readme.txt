# Compiling the Program

To compile the program, you can simply run the `make` command. This will compile
all the source files using the gcc compiler and create an executable named
"main".

```bash
make
```

The raw `gcc` command used by the `make` command is as follows:

```bash
gcc -O3 -o main main.c schedulers.c task_management.c lib/libssvector.c
```


# Running the program

To execute the program, you can run the `main` executable. By default, it
assumes that there is a file named "TaskSpec.txt" in the current working
directory and it will output the results to a file named "Output.txt".

```bash
./main
```

You can also specify custom input and output files when running the program.
Here is an example of how to do that:

```bash
./main <input file> <output file>
```

In this case, the input file argument will replace the default "TaskSpec.txt"
and the output file argument will replace the default "Output.txt".

You can use "stdout" and "stdin" as arguments to output to standard output or
read from standard input, respectively. For example:

```bash
cat TaskSpec.txt | ./main stdin stdout
```

Note that these keywords for arguments are reserved for standard input and
output. Therefore, they will not be interpret as "stdin" and "stdout" files.


## Debug Flag

If you want to enable debug information, you can pass the `-v` flag before the
`<input-file>` and `<output-file>` arguments. This will output additional
details such as processes changed and the current state of queues.

Here is an example of how to use the debug flag:

```bash
./main -v TaskSpec.txt Output.txt
```

Note that when using the `-v` flag, you must specify both the input and output
files. If only the `-v` flag is provided, the program will exit with an error.
