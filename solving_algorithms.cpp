#include <iostream>
#include <stdlib.h>

#include "solving_algorithms.h"

using namespace std;

/*
 * Checks if cell has a single candidate. If it does, solve_cell().
 *
 * Returns 'true' if a cell has been changed
 */
bool single_candidate(array< array<Cell, 9>, 9>&board, int x, int y) {
  if ((board[x][y].candidates.count() == 0) && (board[x][y].val == -1)) {
    // No candidates and space is empty - unsolvable!
    cout << endl << "Board is unsolvable :(" << endl;
    print_board(board);
    exit(EXIT_FAILURE);
  }
  else if (board[x][y].candidates.count() == 1) {
    // if only 1 bit is set, cell is solvable!
    solve_cell(board, x, y);
    return true;
  }

  return false;
} /* single_candidate() */

/*  #############################
 *  #   UNIQUE VALUE CHECKERS   #
 *  #############################
 */

/*
 * These functions check the Cell's row/col/sector to find out if the Cell
 * has a candidate that is unique to its row/col/sector. For example:
 * Cell2 candidates 2, 3, 4, 7, 8, 9:  1 1 1 0 0 1 1 1 0
 * Cell3 candidates 6, 7, 9         :  1 0 1 1 0 0 0 0 0  OR
 * Cell4 candidates 2, 3, 4, 6, 7, 9:  1 0 1 1 0 1 1 1 0
 * etc...                             -------------------
 *                                     1 1 1 1 0 1 1 1 0  XOR
 * Cell candidates  1, 3, 7, 8, 9   :  1 1 1 0 0 0 1 0 1
 *                                    -------------------
 *                                     0 0 0 1 0 1 0 1 1  AND
 * Cell candidates  1, 3, 7, 8, 9   :  1 1 1 0 0 0 1 0 1
 *                                    -------------------
 *                                     0 0 0 0 0 0 0 0 1
 * So the Cell has a unique candidate of 1 in its row/col/sector, therefore
 * its value must be 1.
 */

/*
 * Checks if cell has a unique candidate for the row that it's in. If it does,
 * solve_cell().
 *
 * Returns 'true' if a cell has been changed.
 */
bool unique_in_row(array< array<Cell, 9>, 9>&board, int x, int y) {
  bitset<9> unique_tester =  { 0 };

  for (int i = 0; i < 9; i++) {
    // OR each Cell's candidates field to get all present candidates in row
    if (y != i) {
      unique_tester |= board[x][i].candidates;
    }
  }

  unique_tester ^= board[x][y].candidates;
  unique_tester &= board[x][y].candidates;

  if (unique_tester.count() == 1) {
    // set Cell's candidates to new bitset if there is a unique value in its row
    board[x][y].candidates = unique_tester;
    solve_cell(board, x, y);
    return true;
  }

  return false;
} /* unique_in_row() */

/*
 * Checks if cell has a unique candidate for the col that it's in. If it does,
 * solve_cell().
 *
 * Returns 'true' if a cell has been changed.
 */
bool unique_in_col(array< array<Cell, 9>, 9>&board, int x, int y) {
  bitset<9> unique_tester =  { 0 };

  for (int i = 0; i < 9; i++) {
    // OR each Cell's candidates field to get all present candidates in col
    if (x != i) {
      unique_tester |= board[i][y].candidates;
    }
  }

  unique_tester ^= board[x][y].candidates;
  unique_tester &= board[x][y].candidates;

  if (unique_tester.count() == 1) {
    // set Cell's candidates to new bitset if there is a unique value in its col
    board[x][y].candidates = unique_tester;
    solve_cell(board, x, y);
    return true;
  }

  return false;
} /* unique_in_col() */
/*
 * Checks if cell has a unique candidate for the sector that it's in. If it
 * does, solve_cell().
 *
 * Returns 'true' if a cell has been changed.
 */

bool unique_in_sector(array< array<Cell, 9>, 9>&board, int x, int y) {
  array<array<int, 2>, 9> coords = get_sector_coords(get_sector(x, y));
  bitset<9> unique_tester =  { 0 };
  int a = 0;
  int b = 0;

  for (int i = 0; i < 9; i++) {
    a = coords[i][0];
    b = coords[i][1];
    if ((a != x) || (b != y)) {
      // OR each Cell's candidates field to get all present candidates in sector
      unique_tester |= board[a][b].candidates;
    }
  }

  unique_tester ^= board[x][y].candidates;
  unique_tester &= board[x][y].candidates;

  if (unique_tester.count() == 1) {
    // set Cell's candidates to new bitset if there is a unique value in its col
    board[x][y].candidates = unique_tester;
    solve_cell(board, x, y);

    return true;
  }

  return false;
} /* unique_in_sector() */

/*
 * If 2 Cells in the same row/col only have 2 options (the same 2 options),
 * AND are in the same sector, those 2 Cells are the place where those options
 * must lie.
 */
bool unique_pair_in_sector(array< array<Cell, 9>, 9>&board, int x, int y) {
  if (board[x][y].narrowed_down) {
    return false;
  }

  array<array<int, 2>, 9> coords = get_sector_coords(get_sector(x, y));
  int a = 0;
  int b = 0;

  for (int i = 0; i < 9; i++) {
    a = coords[i][0];
    b = coords[i][1];


    if ((a != x) || (b != y)) {
      if ((board[a][b].candidates == board[x][y].candidates) &&
          (board[a][b].candidates.count() == 2)) {
        bitset<9> temp_candidates = board[x][y].candidates;
        // extract candidates from bitset
        int candidate[2] = {0, 0};
        int index = 0;
        for (int j =  8; j >= 0; j--) {
          if (board[a][b].candidates[j] == 1) {
            candidate[index] = j + 1;
            index++;
          }
        }

        if (a == x) {
          // Cells are in the same row, so remove those 2 candidates from the row
          remove_candidate_row(board, x, y, candidate[0]);
          remove_candidate_row(board, x, y, candidate[1]);
        }
        else if (b == y) {
          // Cells are in the same column, so remove those 2 candidates from the col
          remove_candidate_col(board, x, y, candidate[0]);
          remove_candidate_col(board, x, y, candidate[1]);
        }

        remove_candidate_sector(board, x, y, candidate[0]);
        remove_candidate_sector(board, x, y, candidate[1]);

        // adds those candidates back to the 2 Cells since we didn't solve anything
        board[x][y].candidates = temp_candidates;
        board[a][b].candidates = temp_candidates;

        // This field is to prevent an infinite loop from re-calling this
        // function on a pair of Cells that have 2 candidates (since no Cell is solved)
        board[x][y].narrowed_down = true;
        board[a][b].narrowed_down = true;

        return true;
      }
    }
  }
  return false;
} /* unique_pair_in_sector() */

/*
 * pt1: if there is a pair of Cells in a sector that are in the same
 *      row/column, and there is a value that is unique to both of
 *      those Cells in the sector, that value can be removed from the
 *      corresponding row/column (bitwise AND the two Cells and check
 *      the rest of the sector for existence. if there are unique values,
 *      remove them from the corresponding row/col)
 *
 * pt2: if there is a pair of Cells in a sector that are in the same
 *      row/column, and there is a value in both of those columns that are
 *      unique to that row/column, that value can be removed from the
 *      corresponding row/column (bitwise AND the two cells and check
 *      for existence in the corresponding row/col)
 */
bool pointing_pair(array< array<Cell, 9>, 9>&board, int x, int y) {
  array< array<int, 2>, 9> coords = get_sector_coords(get_sector(x, y));

  return false;
} /* pointing_pair() */

/*
 * Wrapper function to run all solving algorithms on the cell passed in.
 */
void run_solving_algorithms(array< array<Cell, 9>, 9>&board) {
  bool changed = false;
  while (1) {
    changed = false;
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        if (board[i][j].val == -1) {
          if (single_candidate(board, i, j) == true) {
            cout << "single candidate" << endl;
            changed |= true;
            continue;
          }
          changed |= false;
          if (unique_in_row(board, i, j)) {
            cout << "unique row" << endl;
            changed |= true;
            continue;
          }
          changed |= false;
          if (unique_in_col(board, i, j)) {
            cout << "unique col" << endl;
            changed |= true;
            continue;
          }
          changed |= false;
          if (unique_in_sector(board, i, j)) { // I think this works?
            cout << "unique_sector" << endl;
            changed |= true;
            continue;
          }
          changed |= false;
          if (unique_pair_in_sector(board, i, j)) { // I think this works?
            cout << "unique pair sector" << endl;
            changed |= true;
            continue;
          }
          changed |= false;
          if (pointing_pair(board, i, j)) {
            cout << "pointing pair" << endl;
            changed |= true;
            continue;
          }
          changed |= false;
        }
      }
    }
    if (!changed) {
      return;
      for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
          cout << i << ", " << j << ":    " << board[i][j].candidates.to_string() << endl;
        }
      }
    }
  }
} /* run_solving_algorithms() */

