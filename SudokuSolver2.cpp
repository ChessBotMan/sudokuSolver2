#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>

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
                        grid[i][j] = 0; // undo
                    }
                }
                return false; // no valid number found
            }
        }
    }
    return true; // all cells filled
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

int main() {
    srand((unsigned)time(0));

    int original[9][9];

    cout << "SUDOKU SOLVER" << endl;
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

    {
        int grid[9][9];
        copyGrid(grid, original);
        bool fixed[9][9];
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                fixed[i][j] = (original[i][j] != 0);

        cout << "========================================" << endl;
        cout << " Method 2: Simulated Annealing" << endl;
        cout << "========================================" << endl;

        auto start = chrono::high_resolution_clock::now();

        // Try multiple restarts since SA is stochastic
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

    cout << " Done!" << endl;

    return 0;
}
