#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;

void copyGrid(int dest[9][9], int src[9][9]) {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            dest[i][j] = src[i][j];
}

void printGrid(int grid[9][9]) {
    cout << "+-------+-------+-------+" << endl;
    for (int i = 0; i < 9; i++) {
        cout << "| ";
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0)
                cout << ". ";
            else
                cout << grid[i][j] << " ";
            if ((j + 1) % 3 == 0) cout << "| ";
        }
        cout << endl;
        if ((i + 1) % 3 == 0)
            cout << "+-------+-------+-------+" << endl;
    }
}

bool isValid(int grid[9][9], int row, int col, int num) {
    for (int j = 0; j < 9; j++)
        if (grid[row][j] == num) return false;

    for (int i = 0; i < 9; i++)
        if (grid[i][col] == num) return false;

    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (int i = boxRow; i < boxRow + 3; i++)
        for (int j = boxCol; j < boxCol + 3; j++)
            if (grid[i][j] == num) return false;

    return true;
}

bool solveBacktracking(int grid[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (isValid(grid, i, j, num)) {
                        grid[i][j] = num;
                        if (solveBacktracking(grid))
                            return true;
                        grid[i][j] = 0; 
                    }
                }
                return false; 
            }
        }
    }
    return true;
}

int countConflicts(int grid[9][9]) {
    int conflicts = 0;

    for (int i = 0; i < 9; i++) {
        int count[10] = {0};
        for (int j = 0; j < 9; j++)
            count[grid[i][j]]++;
        for (int n = 1; n <= 9; n++)
            if (count[n] > 1) conflicts += count[n] - 1;
    }

    for (int j = 0; j < 9; j++) {
        int count[10] = {0};
        for (int i = 0; i < 9; i++)
            count[grid[i][j]]++;
        for (int n = 1; n <= 9; n++)
            if (count[n] > 1) conflicts += count[n] - 1;
    }

    return conflicts;
}

void fillBoxes(int grid[9][9], bool fixed[9][9]) {
    for (int boxR = 0; boxR < 3; boxR++) {
        for (int boxC = 0; boxC < 3; boxC++) {
            bool used[10] = {false};
            int sr = boxR * 3, sc = boxC * 3;

            for (int i = sr; i < sr + 3; i++)
                for (int j = sc; j < sc + 3; j++)
                    if (fixed[i][j]) used[grid[i][j]] = true;

            int missing[9], mCount = 0;
            for (int n = 1; n <= 9; n++)
                if (!used[n]) missing[mCount++] = n;

            for (int i = mCount - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int tmp = missing[i]; missing[i] = missing[j]; missing[j] = tmp;
            }

            int idx = 0;
            for (int i = sr; i < sr + 3; i++)
                for (int j = sc; j < sc + 3; j++)
                    if (!fixed[i][j])
                        grid[i][j] = missing[idx++];
        }
    }
}

bool solveSimulatedAnnealing(int grid[9][9], bool fixed[9][9]) {
    fillBoxes(grid, fixed);

    double temperature = 2.0;
    double coolingRate = 0.99999;
    int maxIter = 2000000;

    int currentCost = countConflicts(grid);

    for (int iter = 0; iter < maxIter && currentCost > 0; iter++) {
        int boxR = rand() % 3;
        int boxC = rand() % 3;
        int sr = boxR * 3, sc = boxC * 3;

        int cells[9][2];
        int cCount = 0;
        for (int i = sr; i < sr + 3; i++)
            for (int j = sc; j < sc + 3; j++)
                if (!fixed[i][j]) {
                    cells[cCount][0] = i;
                    cells[cCount][1] = j;
                    cCount++;
                }

        if (cCount < 2) continue;

        int a = rand() % cCount;
        int b = rand() % cCount;
        while (b == a) b = rand() % cCount;

        int r1 = cells[a][0], c1 = cells[a][1];
        int r2 = cells[b][0], c2 = cells[b][1];

        int tmp = grid[r1][c1];
        grid[r1][c1] = grid[r2][c2];
        grid[r2][c2] = tmp;

        int newCost = countConflicts(grid);
        int delta = newCost - currentCost;

        if (delta < 0 || (double)rand() / RAND_MAX < exp(-delta / temperature)) {
            currentCost = newCost;
        } else {
            tmp = grid[r1][c1];
            grid[r1][c1] = grid[r2][c2];
            grid[r2][c2] = tmp;
        }

        temperature *= coolingRate;

        if (iter % 200000 == 0 && iter > 0 && currentCost > 0) {
            temperature = 2.0;
            fillBoxes(grid, fixed);
            currentCost = countConflicts(grid);
        }
    }

    return currentCost == 0;
}

struct Individual {
    int grid[9][9];
    int cost;
};

bool compareIndividuals(const Individual& a, const Individual& b) {
    return a.cost < b.cost;
}

bool solveGeneticAlgorithm(int grid[9][9], bool fixed[9][9]) {
    const int POP_SIZE = 100;
    const int MAX_GEN = 3000;
    vector<Individual> pop(POP_SIZE);

    for (int p = 0; p < POP_SIZE; p++) {
        copyGrid(pop[p].grid, grid);
        fillBoxes(pop[p].grid, fixed);
        pop[p].cost = countConflicts(pop[p].grid);
        if (pop[p].cost == 0) {
            copyGrid(grid, pop[p].grid);
            return true;
        }
    }

    for (int gen = 0; gen < MAX_GEN; gen++) {
        sort(pop.begin(), pop.end(), compareIndividuals);
        if (pop[0].cost == 0) {
            copyGrid(grid, pop[0].grid);
            return true;
        }

        vector<Individual> nextGen;
        for (int i = 0; i < POP_SIZE / 10; i++) {
            nextGen.push_back(pop[i]);
        }

        while (nextGen.size() < POP_SIZE) {
            int p1 = rand() % (POP_SIZE / 2);
            int p2 = rand() % (POP_SIZE / 2);
            Individual child = pop[p1];

            for (int b = 0; b < 3; b++) { 
                int boxR = rand() % 3; int boxC = rand() % 3;
                int sr = boxR * 3, sc = boxC * 3;
                for (int i = sr; i < sr + 3; i++) {
                    for (int j = sc; j < sc + 3; j++) {
                        child.grid[i][j] = pop[p2].grid[i][j];
                    }
                }
            }

            if (rand() % 100 < 80) { 
                int boxR = rand() % 3; int boxC = rand() % 3;
                int sr = boxR * 3, sc = boxC * 3;
                int cells[9][2]; int cCount = 0;
                for (int i = sr; i < sr + 3; i++)
                    for (int j = sc; j < sc + 3; j++)
                        if (!fixed[i][j]) { cells[cCount][0] = i; cells[cCount][1] = j; cCount++; }
                
                if (cCount >= 2) {
                    int a = rand() % cCount; int b = rand() % cCount;
                    while(a == b) b = rand() % cCount;
                    int tmp = child.grid[cells[a][0]][cells[a][1]];
                    child.grid[cells[a][0]][cells[a][1]] = child.grid[cells[b][0]][cells[b][1]];
                    child.grid[cells[b][0]][cells[b][1]] = tmp;
                }
            }

            child.cost = countConflicts(child.grid);
            nextGen.push_back(child);
        }
        pop = nextGen;

        if (gen % 300 == 0 && gen > 0) {
            for (int p = POP_SIZE / 2; p < POP_SIZE; p++) {
                copyGrid(pop[p].grid, grid);
                fillBoxes(pop[p].grid, fixed);
                pop[p].cost = countConflicts(pop[p].grid);
            }
        }
    }

    sort(pop.begin(), pop.end(), compareIndividuals);
    copyGrid(grid, pop[0].grid);
    return pop[0].cost == 0;
}

bool solveTabuSearch(int grid[9][9], bool fixed[9][9]) {
    fillBoxes(grid, fixed);
    int currentCost = countConflicts(grid);
    if (currentCost == 0) return true;

    int bestOverallCost = currentCost;
    int tabuList[9][9] = {0}; 
    int tabuTenure = 25;
    int maxIter = 50000;

    for (int iter = 0; iter < maxIter && currentCost > 0; iter++) {
        int bestMoveCost = 9999;
        int bestR1 = -1, bestC1 = -1, bestR2 = -1, bestC2 = -1;

        int boxR = rand() % 3; int boxC = rand() % 3;
        int sr = boxR * 3, sc = boxC * 3;

        int cells[9][2]; int cCount = 0;
        for (int i = sr; i < sr + 3; i++)
            for (int j = sc; j < sc + 3; j++)
                if (!fixed[i][j]) { cells[cCount][0] = i; cells[cCount][1] = j; cCount++; }

        for (int attempt = 0; attempt < 15; attempt++) {
            if (cCount < 2) break;
            
            int a = rand() % cCount; int b = rand() % cCount;
            while (b == a) b = rand() % cCount;

            int r1 = cells[a][0], c1 = cells[a][1];
            int r2 = cells[b][0], c2 = cells[b][1];

            int tmp = grid[r1][c1]; grid[r1][c1] = grid[r2][c2]; grid[r2][c2] = tmp;
            int newCost = countConflicts(grid);
            tmp = grid[r1][c1]; grid[r1][c1] = grid[r2][c2]; grid[r2][c2] = tmp;

            bool isTabu = (tabuList[r1][c1] > iter || tabuList[r2][c2] > iter);
            
            if (!isTabu || newCost < bestOverallCost) {
                if (newCost < bestMoveCost) {
                    bestMoveCost = newCost;
                    bestR1 = r1; bestC1 = c1; bestR2 = r2; bestC2 = c2;
                }
            }
        }

        if (bestR1 != -1) {
            int tmp = grid[bestR1][bestC1];
            grid[bestR1][bestC1] = grid[bestR2][bestC2];
            grid[bestR2][bestC2] = tmp;

            currentCost = bestMoveCost;
            if (currentCost < bestOverallCost) bestOverallCost = currentCost;

            tabuList[bestR1][bestC1] = iter + tabuTenure;
            tabuList[bestR2][bestC2] = iter + tabuTenure;
        } else {
            fillBoxes(grid, fixed);
            currentCost = countConflicts(grid);
            memset(tabuList, 0, sizeof(tabuList));
        }
    }
    return currentCost == 0;
}

int main() {
    srand((unsigned)time(0));

    int original[9][9];

    cout << "SUDOKU SOLVER (4 ALGORITHMS)" << endl;
    cout << endl;
    cout << "Enter the Sudoku puzzle row by row." << endl;
    cout << "Use 0 for empty cells." << endl;
    cout << "Enter 9 numbers per row separated by spaces." << endl;
    cout << endl;

    for (int i = 0; i < 9; i++) {
        cout << "Row " << (i + 1) << ": ";
        for (int j = 0; j < 9; j++) {
            cin >> original[i][j];
        }
    }

    cout << endl;
    cout << "Input puzzle:" << endl;
    printGrid(original);
    cout << endl;

    // METHOD 1: BACKTRACKING
    {
        int grid[9][9];
        copyGrid(grid, original);

        cout << " Method 1: Backtracking" << endl;

        auto start = chrono::high_resolution_clock::now();
        bool solved = solveBacktracking(grid);
        auto end = chrono::high_resolution_clock::now();

        double elapsed = chrono::duration<double, milli>(end - start).count();

        if (solved) {
            printGrid(grid);
            cout << "Time taken: " << elapsed << " ms" << endl;
        } else {
            cout << "No solution found." << endl;
            cout << "Time taken: " << elapsed << " ms" << endl;
        }
        cout << endl;
    }

    bool fixed[9][9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            fixed[i][j] = (original[i][j] != 0);

    {
        int grid[9][9];
        cout << " Method 2: Simulated Annealing" << endl;

        auto start = chrono::high_resolution_clock::now();
        bool solved = false;
        for (int attempt = 0; attempt < 10 && !solved; attempt++) {
            copyGrid(grid, original);
            solved = solveSimulatedAnnealing(grid, fixed);
        }
        auto end = chrono::high_resolution_clock::now();
        
        double elapsed = chrono::duration<double, milli>(end - start).count();

        if (solved) {
            printGrid(grid);
            cout << "Time taken: " << elapsed << " ms" << endl;
        } else {
            cout << "Could not find solution (SA is stochastic, try again)." << endl;
            cout << "Time taken: " << elapsed << " ms" << endl;
        }
        cout << endl;
    }

    {
        int grid[9][9];
        cout << " Method 3: Genetic Algorithm" << endl;

        auto start = chrono::high_resolution_clock::now();
        bool solved = false;
        for (int attempt = 0; attempt < 10 && !solved; attempt++) {
            copyGrid(grid, original);
            solved = solveGeneticAlgorithm(grid, fixed);
        }
        auto end = chrono::high_resolution_clock::now();

        double elapsed = chrono::duration<double, milli>(end - start).count();

        if (solved) {
            printGrid(grid);
            cout << "Time taken: " << elapsed << " ms" << endl;
        } else {
            cout << "Could not find solution (GA is stochastic, try again)." << endl;
            cout << "Time taken: " << elapsed << " ms" << endl;
        }
        cout << endl;
    }

    {
        int grid[9][9];
        cout << " Method 4: Tabu Search" << endl;

        auto start = chrono::high_resolution_clock::now();
        bool solved = false;
        for (int attempt = 0; attempt < 10 && !solved; attempt++) {
            copyGrid(grid, original);
            solved = solveTabuSearch(grid, fixed);
        }
        auto end = chrono::high_resolution_clock::now();

        double elapsed = chrono::duration<double, milli>(end - start).count();

        if (solved) {
            printGrid(grid);
            cout << "Time taken: " << elapsed << " ms" << endl;
        } else {
            cout << "Could not find solution (TS is stochastic, try again)." << endl;
            cout << "Time taken: " << elapsed << " ms" << endl;
        }
        cout << endl;
    }

    cout << " Done!" << endl;
    return 0;
}