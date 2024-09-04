# DIMACS Fuzzer Documentation

This tool is a fuzzer designed to test SAT solvers by generating inputs in the DIMACS format, a common way to describe boolean formulas in Conjunctive Normal Form (CNF). The goal of the fuzzer is to discover bugs in SAT solvers, particularly those built in C.

### Resources on the DIMACS Format:
- CNF in DIMACS format explained: https://fairmut3x.wordpress.com/2011/07/29/cnf-conjunctive-normal-form-dimacs-format-explained/
- DIMACS CNF Format Specification (PDF): http://www.domagoj-babic.com/uploads/ResearchProjects/Spear/dimacs-cnf.pdf

## Fuzzer Interface

The fuzzer is invoked using the following command:

`fuzz-sat /path/to/SUT /path/to/inputs seed`

- `/path/to/SUT`: Path to the directory containing the System Under Test (SUT), which should be a SAT solver built with gcov coverage, AddressSanitizer (ASan), and UndefinedBehaviorSanitizer (UBSan) enabled.
  
- `/path/to/inputs`: Directory containing a non-empty set of well-formed DIMACS files used as input samples for fuzzing.

- `seed`: An integer used to initialize the random number generator, if required by the fuzzer.

## Execution Details

The fuzzer operates using the following scripts:

### `runsat.sh`
This script executes the SUT with a given input formula. It expects a single argument: the path to a DIMACS file containing the boolean formula. The script will:
- Determine whether the formula is satisfiable.
- Optionally print a model if one exists.
- Report errors if AddressSanitizer (ASan) or UndefinedBehaviorSanitizer (UBSan) detect any issues.

### `build.sh`
This script builds the SUT, ensuring that it includes coverage instrumentation (gcov), ASan, and UBSan.

## Fuzzer Output

The fuzzer generates a directory named `fuzzed-tests` in the current working directory. This folder will contain up to 20 test cases, each of which triggers undefined behavior in the SUT.

## Fuzzer Workflow

The fuzzer follows these steps:

1. **Generate Input**: A DIMACS file is generated as input for the SUT.
   
2. **Run SUT**: The fuzzer runs the SUT using the `runsat.sh` script, passing the generated input as an argument. The SUT is terminated after a configurable timeout if it doesn't complete execution.

3. **Check for Undefined Behavior**: If the SUT encounters undefined behavior (e.g., detected by ASan or UBSan), the corresponding test case is saved in the `fuzzed-tests` directory.

4. **Repeat**: The process is repeated, generating and testing new inputs until a predefined number of test cases or timeout is reached.

---

By following this process, the fuzzer continuously probes the SUT with random inputs, aiming to uncover bugs, memory errors, or other types of vulnerabilities. The generated test cases can be analyzed to understand the specific conditions under which the SUT fails.
