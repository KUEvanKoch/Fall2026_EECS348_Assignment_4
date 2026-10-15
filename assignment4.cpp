//Program: EECS 348 Assignment 4
//Description: C++ program that solves Sudoku puzzles, providing all valid solutions, or displaying no solution if there are no solutions
//Input: Puzzle text files
//Output: Terminal output of the solved puzzles
//Code Sources: ChatGPT (main program), ChatGPT (modifications)
//Author: Evan Koch
//Collaborators: No collaborators besides ChatGPT
//Creation Date: 10/15/26
//Revision Date: 10/15/26
//Revisions: Added prologue comments

#include <iostream> // Include input and output functionality.
#include <fstream> // Include file input functionality.
#include <string> // Include string functionality.
#include <vector> // Include vector functionality.
using namespace std; // Modified by ChatGPT: Keep standard library names unqualified.

// Define the Sudoku solver class.
class Sudoku {
private:
    static const int SIZE = 9; // Define the dimensions of the Sudoku board.
    int board[SIZE][SIZE]; // Store the working Sudoku board.
    int original[SIZE][SIZE]; // Store the original puzzle for display.
    int solutionCount; // Count the solutions found.

    // Check whether a digit can be placed in a cell.
    bool isValid(int row, int col, int digit) const {
        // Check the row and column.
        for (int i = 0; i < SIZE; i++) {
            if (board[row][i] == digit) // Check whether the digit already exists in the row.
                return false; // Reject the digit if it already exists in the row.

            if (board[i][col] == digit) // Check whether the digit already exists in the column.
                return false; // Reject the digit if it already exists in the column.
        }

        // Find the top-left corner of the 3x3 grid.
        int startRow = (row / 3) * 3; // Calculate the first row of the subgrid.
        int startCol = (col / 3) * 3; // Calculate the first column of the subgrid.

        // Check the 3x3 grid.
        for (int r = startRow; r < startRow + 3; r++) {
            for (int c = startCol; c < startCol + 3; c++) {
                if (board[r][c] == digit) // Check whether the digit already exists in the subgrid.
                    return false; // Reject the digit if it already exists in the subgrid.
            }
        }

        return true; // Allow the digit if no conflicts are found.
    }

    // Modified by ChatGPT: Validate all pre-filled digits before solving.
    bool validateBoard() const {
        // Check every cell in the original puzzle.
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                int digit = board[row][col]; // Get the current cell value.

                if (digit == 0) // Skip empty cells.
                    continue; // Move to the next cell.

                // Modified by ChatGPT: Check whether the current clue conflicts with another clue.
                for (int i = 0; i < SIZE; i++) {
                    if (i != col && board[row][i] == digit) // Check for duplicate digits in the row.
                        return false; // Reject the puzzle if the row contains a duplicate.

                    if (i != row && board[i][col] == digit) // Check for duplicate digits in the column.
                        return false; // Reject the puzzle if the column contains a duplicate.
                }

                // Modified by ChatGPT: Find the current cell's 3x3 subgrid.
                int startRow = (row / 3) * 3; // Calculate the first row of the subgrid.
                int startCol = (col / 3) * 3; // Calculate the first column of the subgrid.

                // Modified by ChatGPT: Check the subgrid for duplicate clues.
                for (int r = startRow; r < startRow + 3; r++) {
                    for (int c = startCol; c < startCol + 3; c++) {
                        if ((r != row || c != col) && board[r][c] == digit) // Ignore the current cell while checking duplicates.
                            return false; // Reject the puzzle if the subgrid contains a duplicate.
                    }
                }
            }
        }

        return true; // Accept the puzzle if no conflicts are found.
    }

    // Recursively find and print every solution.
    void solveRecursive() {
        int bestRow = -1; // Store the row of the best empty cell.
        int bestCol = -1; // Store the column of the best empty cell.
        int fewestCandidates = 10; // Initialize the smallest candidate count.

        // Find an empty cell with the fewest valid candidates.
        // This optimization can significantly reduce the search time.
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (board[row][col] != 0) // Skip cells that are already filled.
                    continue; // Move to the next cell.

                int candidateCount = 0; // Count valid candidates for this cell.

                for (int digit = 1; digit <= SIZE; digit++) { // Try each possible digit.
                    if (isValid(row, col, digit)) // Check whether the digit is valid.
                        candidateCount++; // Count the valid candidate.
                }

                // An empty cell with no candidates means this path
                // cannot lead to a solution.
                if (candidateCount == 0) // Check whether this cell has no candidates.
                    return; // Stop exploring this impossible branch.

                if (candidateCount < fewestCandidates) { // Check whether this cell is more constrained.
                    fewestCandidates = candidateCount; // Save the smallest candidate count.
                    bestRow = row; // Save the selected row.
                    bestCol = col; // Save the selected column.
                }
            }
        }

        // No empty cells remain, so a complete solution was found.
        if (bestRow == -1) { // Check whether every cell is filled.
            solutionCount++; // Increment the number of solutions.

            cout << "\nSolution " << solutionCount << ":\n"; // Print the solution number.
            printBoard(board); // Print the completed board.
            return; // Return to the previous recursive call.
        }

        // Try every valid candidate for the selected empty cell.
        for (int digit = 1; digit <= SIZE; digit++) { // Try digits from 1 through 9.
            if (isValid(bestRow, bestCol, digit)) { // Check whether this candidate is valid.
                board[bestRow][bestCol] = digit; // Place the candidate in the board.

                // Recursively solve the remaining empty cells.
                solveRecursive(); // Search for solutions with this candidate.

                // Backtrack to try the next candidate.
                board[bestRow][bestCol] = 0; // Reset the cell to empty.
            }
        }
    }

public:
    Sudoku() : solutionCount(0) { // Initialize the solution counter.
        for (int row = 0; row < SIZE; row++) { // Visit each row.
            for (int col = 0; col < SIZE; col++) { // Visit each column.
                board[row][col] = 0; // Initialize the working board as empty.
                original[row][col] = 0; // Initialize the original board as empty.
            }
        }
    }

    // Read a Sudoku puzzle from a file.
    bool loadFromFile(const string& filename) {
        ifstream inputFile(filename); // Open the puzzle file.

        if (!inputFile) { // Check whether the file opened successfully.
            cerr << "Error: Could not open " << filename << '\n'; // Report the file error.
            return false; // Stop loading the puzzle.
        }

        // Modified by ChatGPT: Read into a temporary board so failed input does not partially overwrite the puzzle.
        int loadedBoard[SIZE][SIZE] = {}; // Initialize a temporary board.

        for (int row = 0; row < SIZE; row++) { // Read each row.
            for (int col = 0; col < SIZE; col++) { // Read each column.
                string value; // Store the next input token.

                if (!(inputFile >> value)) { // Check whether a cell value can be read.
                    cerr << "Error: Invalid puzzle format in " << filename << '\n'; // Report incomplete input.
                    return false; // Stop loading the puzzle.
                }

                if (value == "_") { // Check whether the cell is empty.
                    loadedBoard[row][col] = 0; // Represent an empty cell with zero.
                } else {
                    // Modified by ChatGPT: Require exactly one character from 1 through 9.
                    if (value.size() != 1 || value[0] < '1' || value[0] > '9') { // Reject malformed cell values.
                        cerr << "Error: Invalid cell value in " << filename << '\n'; // Report the invalid value.
                        return false; // Stop loading the puzzle.
                    }

                    loadedBoard[row][col] = value[0] - '0'; // Convert the digit character to an integer.
                }
            }
        }

        // Modified by ChatGPT: Reject any extra tokens after the expected 81 cells.
        string extraValue; // Store any unexpected extra input.

        if (inputFile >> extraValue) { // Check whether more data remains in the file.
            cerr << "Error: Extra data found in " << filename << '\n'; // Report the extra data.
            return false; // Reject the incorrectly formatted file.
        }

        // Modified by ChatGPT: Copy the successfully loaded board into the working board.
        for (int row = 0; row < SIZE; row++) { // Visit each row.
            for (int col = 0; col < SIZE; col++) { // Visit each column.
                board[row][col] = loadedBoard[row][col]; // Copy the validated input value.
            }
        }

        // Save the original puzzle for display.
        for (int row = 0; row < SIZE; row++) { // Visit each row.
            for (int col = 0; col < SIZE; col++) { // Visit each column.
                original[row][col] = board[row][col]; // Preserve the original cell value.
            }
        }

        // Modified by ChatGPT: Reject puzzles with conflicting pre-filled digits.
        if (!validateBoard()) { // Validate the original Sudoku clues.
            cerr << "Error: Conflicting digits in " << filename << '\n'; // Report the invalid puzzle.
            return false; // Do not attempt to solve an invalid puzzle.
        }

        return true; // Report that the puzzle loaded successfully.
    }

    // Print a Sudoku board, displaying zeroes as underscores.
    void printBoard(const int grid[SIZE][SIZE]) const {
        for (int row = 0; row < SIZE; row++) { // Visit each row.
            for (int col = 0; col < SIZE; col++) { // Visit each column.
                if (grid[row][col] == 0) // Check whether the cell is empty.
                    cout << "_"; // Print an underscore for an empty cell.
                else
                    cout << grid[row][col]; // Print the digit in a filled cell.

                if (col < SIZE - 1) // Check whether this is not the last column.
                    cout << ' '; // Separate cell values with spaces.
            }
            cout << '\n'; // Move to the next line after each row.
        }
    }

    // Print the original puzzle and find all solutions.
    void solve(const string& filename) {
        cout << "\n========================================\n"; // Print the heading separator.
        cout << "Puzzle file: " << filename << '\n'; // Print the puzzle filename.
        cout << "========================================\n"; // Print the heading separator.

        cout << "\nOriginal puzzle:\n"; // Label the original puzzle.
        printBoard(original); // Print the unmodified puzzle.

        solutionCount = 0; // Reset the solution counter.

        solveRecursive(); // Search for all solutions.

        if (solutionCount == 0) { // Check whether any solution was found.
            cout << "\nNo solution found\n"; // Report that no solution exists.
        }

        cout << "\nSolutions found: " << solutionCount << "\n"; // Print the total number of solutions.
    }
};

int main() { // Start the program.
    vector<string> filenames = { // Define the five puzzle filenames.
        "puzzle1.txt", // Specify the first puzzle file.
        "puzzle2.txt", // Specify the second puzzle file.
        "puzzle3.txt", // Specify the third puzzle file.
        "puzzle4.txt", // Specify the fourth puzzle file.
        "puzzle5.txt" // Specify the fifth puzzle file.
    };

    for (const string& filename : filenames) { // Process each puzzle file.
        Sudoku puzzle; // Create a solver for the current puzzle.

        if (puzzle.loadFromFile(filename)) { // Continue only if loading and validation succeed.
            puzzle.solve(filename); // Print the puzzle and find its solutions.
        }
    }

    return 0; // End the program successfully.
}
