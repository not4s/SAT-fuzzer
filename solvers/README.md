# SAT Solvers

- `sat.c` The main file for the SAT solver
- `*.c`, `.h` Some number of source files in the root directory needed to build the solver
- `Makefile` The Makefile builds the solver using the `make ub` command. The resulting executable is named `./sat`. The resulting executable is instrumented using ASan and UBSan and produces coverage information in gcov format.
- `runsat.sh` This wrapper script allows the solver to be run after being built using the provided Makefile. This exists because ASan and UBSan expect certain environment variables to contain various configuration options.

