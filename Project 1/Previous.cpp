//
//  Previous.cpp

#include "Previous.h"
#include <iostream>
using namespace std;


// The constructor initializes a Previous object that corresponds to an Arena with nRows rows and nCols columns. You may assume (i.e., you do not have to check) that it will be called with a first argument that does not exceed MAXROWS and a second that does not exceed MAXCOLS, and that neither argument will be less than 1.

Previous::Previous(int nRows, int nCols)
{
    m_rows = nRows;
    m_cols = nCols;
}

// The dropACrumb function is to be called to notify the Previous object that the player has arrived at or stayed at a grid point. The function returns false if row r, column c is not within bounds; otherwise, it returns true after recording what it needs to. This function expects its parameters to be expressed in the same coordinate system as the arena (e.g., row 1, column 1 is the upper-left-most position).

bool Previous::dropACrumb(int r, int c)
{
    if (r > m_rows || r < 1 || c > m_cols || c < 1)
        return false;
    
    int row = r-1;
    int col = c-1;
    
    if (board[row][col] < 65 || board[row][col] > 90)
        board[row][col] = 65;
    else
        ++board[row][col];
    
    return true;
}

void Previous::showPreviousMoves() const
{
    // Clearing the screen
    clearScreen();
    
    // Drawing the board
    int r, c;
    char letter;
    for (r = 0; r < m_rows; r++)
    {
        for (c = 0; c < m_cols; c++)
        {
            if (board[r][c] >= 65 && board[r][c] <= 90)
            {
                letter = board[r][c];
                cout << letter;
            }
            else
                cout << '.';
        }
        cout << endl;
    }
    cout << endl;
}
