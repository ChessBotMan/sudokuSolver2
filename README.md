# Quad-Method Sudoku Solver (C++)

A command-line C++ Sudoku solver that benchmarks four distinct algorithmic approaches to solve a standard 9x9 Sudoku puzzle. It compares deterministic, brute-force methods against stochastic, heuristic-based meta-algorithms, measuring execution times down to the millisecond.

### ⚠️ Known Issues

**Methods 3 (Genetic Algorithm) and 4 (Tabu Search) are currently highly experimental and buggy.** Because Sudoku has extremely tight, rigid constraints, standard crossover/mutation and neighborhood search functions often struggle to reach a global optimum (zero conflicts) without getting permanently trapped in local minima. These two algorithms will frequently fail to find a valid solution in their current state and are pending a future patch to refine their cost functions and neighborhood generation.

---

## ✨ Features

* **Interactive CLI Input:** Prompts the user to enter the puzzle row by row.
* **Algorithmic Benchmarking:** Measures and displays the execution time (`ms`) for each solving method using the C++ `<chrono>` library.
* **Four Distinct Solvers:**
1. Backtracking (Depth-First Search)
2. Simulated Annealing (Thermodynamic optimization)
3. Genetic Algorithm (Evolutionary optimization) *(WIP)*
4. Tabu Search (Memory-based local search) *(WIP)*



---

## 🧠 The Algorithms

### Method 1: Backtracking (Deterministic / Exact)

A standard brute-force approach. It recursively places valid digits (1-9) into empty cells. If a placement leads to an unsolvable state later on, it "backtracks" (undoes the placement) and tries the next digit.

* **Reliability:** 100%. Guaranteed to find a solution if one exists, usually in under a millisecond.

### Method 2: Simulated Annealing (Stochastic)

Inspired by the metallurgical process of heating and controlled cooling. The grid is populated with randomly shuffled 1-9 digits in each 3x3 box. The algorithm randomly swaps two mutable cells within a box. If the swap reduces row/column conflicts, it's kept. If it increases conflicts, it *might* still be kept based on a steadily decreasing "temperature."

* **Reliability:** High, but occasionally requires random restarts to escape deep local minima.

### Method 3: Genetic Algorithm (Evolutionary) *[BUGGY]*

Simulates natural selection. It generates a "population" of random grids. Grids with fewer conflicts (higher fitness) are selected to "breed" (crossover), passing their 3x3 boxes to a child grid. Random mutations (swaps) are introduced to maintain genetic diversity.

* **Current Status:** Struggles to converge on a perfect 0-conflict solution.

### Method 4: Tabu Search (Local Search) *[BUGGY]*

Evaluates the local "neighborhood" of a grid state by testing multiple potential swaps within a 3x3 box and taking the absolute best one, even if it temporarily increases the total conflict cost. It maintains a "Tabu List" of recently swapped cells to prevent the algorithm from endlessly cycling back and forth between the same states.

* **Current Status:** Neighborhood evaluation is currently too narrow, leading to stagnation.

---

## 🚀 Compilation & Usage

### 1. Compiling the Code

Compile this program using any standard C++11 (or higher) compiler. Optimization flags (like `-O3`) are highly recommended due to the millions of iterations required by the stochastic algorithms.

```bash
g++ -O3 -std=c++11 sudoku_solver.cpp -o sudoku

```

### 2. Running the Solver

Execute the compiled binary:

```bash
./sudoku

```

### 3. Inputting a Puzzle

The program expects you to input 9 numbers per row, separated by spaces. Use `0` to represent empty cells.

**Example Input:**

```text
Row 1: 5 3 0 0 7 0 0 0 0
Row 2: 6 0 0 1 9 5 0 0 0
Row 3: 0 9 8 0 0 0 0 6 0
Row 4: 8 0 0 0 6 0 0 0 3
Row 5: 4 0 0 8 0 3 0 0 1
Row 6: 7 0 0 0 2 0 0 0 6
Row 7: 0 6 0 0 0 0 2 8 0
Row 8: 0 0 0 4 1 9 0 0 5
Row 9: 0 0 0 0 8 0 0 7 9

```
