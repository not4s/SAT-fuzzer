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

## Building the Fuzzer with CMake
### Build Instructions:
1. **Install CMake**:  
If you don't have CMake installed, follow [this guide](https://cmake.org/install/) to install it.
2. **Configure and Build**:
To build the project, run the following commands from the project root:
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```
This will generate the `fuzz-sat` binary in the `bin/` directory under `build/`.

3. **Running the Fuzzer**:
After building, run the fuzzer with the following command:
  ```bash
  ./bin/fuzz-sat /path/to/SUT /path/to/inputs seed
  ```
Ensure that the SUT and input files are properly set up before running the fuzzer.

## Running Unit Tests
The includes **Google Test** for unit testing. The tests cover key components such as formula transformations, utility functions, and fuzzing logic. You can run the tests as follows:

1. **Build Tests**:
CMake also handles building the tests. Simply run:

  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```
This will compile the test suite along with the main project.

2. **Run Tests**:
After the build completes, you can run the tests:

  ```bash
  ./bin/runTests
  ```
Alternatively, you can use CMake’s built-in testing functionality with `ctest`:

  ```bash
  ctest
  ```
This will run all the unit tests and provide a summary of the results.


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
