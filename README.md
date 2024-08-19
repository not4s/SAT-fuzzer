This is a DIMACS fuzzer, a format describing a boolean formula in conjunctive normal form (CNF).
Some information about this format can be found at the following links:

- <https://fairmut3x.wordpress.com/2011/07/29/cnf-conjunctive-normal-form-dimacs-format-explained/>
- <http://www.domagoj-babic.com/uploads/ResearchProjects/Spear/dimacs-cnf.pdf>

The aim of the fuzzer is to find bugs in SAT solvers that were built in C

The interface of the resulting fuzzer is `fuzz-sat /path/to/SUT /path/to/inputs seed` where:

- `/path/to/SUT` refers to the source directory of the SUT containing the built solver (with gcov coverage information, ASan, and UBSan enabled). The `runsat.sh` script expects a single command line argument, the path to a file containing the input formula, and prints out whether the formula was satisfiable, optionally a model, and the error reports of ASan and UBSan if any issues were detected.
- `/path/to/inputs` refers to a directory containing a non-empty set of well-formed DIMACS files.
- `seed` is an integer that you should use to initialize a random number generator if you need one.

## The build script
The `build.sh` script is used to build 

## The fuzzer output

Your fuzzer creates a directory `fuzzed-tests` in the **current working directory** that contains at **most 20** test cases that are known to trigger an undefined behavior in the SUT.

The fuzzer runs in the following steps:

1. Generate an input.
2. Run the SUT (using the provided `runsat.sh` script) on the input, killing the SUT after a timeout of your choice.
3. If an undefined behavior was triggered, consider saving the test case in `fuzzed-tests`
4. Go back to step 1.
