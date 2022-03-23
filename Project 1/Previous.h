//
//  Previous.h

#ifndef PREVIOUS_H
#define PREVIOUS_H

#include "globals.h"

class Previous
{
  public:
    // Constructor/Destructor
    Previous(int nRows, int nCols);
    
    // Mutator
    bool dropACrumb(int r, int c);
    
    // Accessor
    void showPreviousMoves() const;
    
  private:
    int m_rows;
    int m_cols;
    int board[MAXROWS][MAXCOLS];
};

#endif /* PREVIOUS_H */
