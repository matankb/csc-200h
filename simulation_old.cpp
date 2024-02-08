#include <chrono>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int max(int a, int b) {
  return a > b ? a : b;
}

int max_all(int a, int b, int c, int d) {
  return max(a, max(b, max(c, d)));
}

int cell(int *board, int n, int x, int y) {
  return board[(x * n) + y];
}

int calculate_chain_from_cell(int *board, int n, int x, int y, int *horizontal, int *vertical, int *diagonal_one, int *diagonal_two) {
  int player = cell(board, n, x, y);

  if (cell(board, n, x - 1, y) == player) {
    horizontal[x * n + y] = horizontal[(x - 1) * n + y] + 1;
  }
}

// Horizontal check

// given a cell on a board, the longest chain that it is a part of
// optimization: create "considered" array
int calculate_chain_from_cell(int *board, int n, int x, int y) {
  int player = cell(board, n, x, y);

  // Horizontal check
  // todo: the names "horizontal" and "vertical" might be wrong

  int horizontal = 1;

  // check below
  for (int i = x + 1; i < n; i++) {
    if (cell(board, n, i, y) == player) {
      horizontal++;
    } else {
      break;
    }
  }

  // check above
  for (int i = x - 1; i >= 0; i--) {
    if (cell(board, n, i, y) == player) {
      horizontal++;
    } else {
      break;
    }
  }

  // Vertical check

  int vertical = 1;

  for (int i = y + 1; i < n; i++) {
    if (cell(board, n, x, i) == player) {
      vertical++;
    } else {
      break;
    }
  }
  for (int i = y - 1; i >= 0; i--) {
    if (cell(board, n, x, i) == player) {
      vertical++;
    } else {
      break;
    }
  }

  // First diagonal check - both x and y increasing/decreasing
  // similar to equation (x = y)

  int diagonal_one = 1;

  for (int i = 1; ((x + i) < n) && (y + i) < n; i++) {
    if (cell(board, n, x + i, y + i) == player) {
      diagonal_one++;
    } else {
      break;
    }
  }

  for (int i = 1; ((x - i) >= 0) && (y - i) >= 0; i++) {
    if (cell(board, n, x - i, y - i) == player) {
      diagonal_one++;
    } else {
      break;
    }
  }

  // First diagonal check - x and y increasing/decreasing
  // (similar to equation x = -y)

  int diagonal_two = 1;

  for (int i = 1; ((x + i) < n) && (y - i) >= 0; i++) {
    if (cell(board, n, x + i, y - i) == player) {
      diagonal_two++;
    } else {
      break;
    }
  }

  for (int i = 1; ((x - i) >= 0) && (y + i) < n; i++) {
    if (cell(board, n, x - i, y + i) == player) {
      diagonal_two++;
    } else {
      break;
    }
  }

  int max = max_all(horizontal, vertical, diagonal_one, diagonal_two);

  /*
    printf("Cell: Row = %d, Col = %d, Player = %d\n", x, y, player);
    printf("Horizontal: %d\nVertical: %d\nDiagonal 1: %d\nDiagonal 2: %d\n", horizontal, vertical, diagonal_one, diagonal_two);
    printf("Max: %d\n", max);
    printf("\n");
    */

  // return max of horizontal, vertical, and diagonals
  return max;
}

int calculate_winner(int *board, int n) {
  int longest_zero_chain_length = 0;
  int longest_one_chain_length = 0;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      int chain = calculate_chain_from_cell(board, n, i, j);
      if (cell(board, n, i, j) == 0) {
        if (chain > longest_zero_chain_length) {
          longest_zero_chain_length = chain;
        }
      } else {
        if (chain > longest_one_chain_length) {
          longest_one_chain_length = chain;
        }
      }
    }
  }

  // printf("Longest chain of 0s: %d\nLongest chain of 1s: %d", longest_zero_chain_length, longest_one_chain_length);

  return longest_zero_chain_length == longest_one_chain_length ? -1 : longest_zero_chain_length > longest_one_chain_length ? 0
                                                                                                                           : 1;
}

void print_board(int *board, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      printf("%d ", board[i * n + j]);
    }
    printf("\n");
  }
  printf("\n");
}

void generate_half_half_permutations(int *arr, int index, int zeros_remaining, int ones_remaining, int board_dimensions, int *zero_wins, int *one_wins, int *ties) {
  if (ones_remaining == 0 && zeros_remaining == 0) {
    int winner = calculate_winner(arr, board_dimensions);
    // int winner = 0;
    // printf("Winner: %d\n", winner);
    // print_board(arr, board_dimensions);
    if (winner == 0) {
      (*zero_wins)++;
    } else if (winner == 1) {
      (*one_wins)++;
    } else {
      (*ties)++;
    }
    return;
  }

  // Generate permutations recursively
  if (zeros_remaining > 0) {
    arr[index] = 0;
    generate_half_half_permutations(arr, index + 1, zeros_remaining - 1, ones_remaining, board_dimensions, zero_wins, one_wins, ties);
  }
  if (ones_remaining > 0) {
    arr[index] = 1;
    generate_half_half_permutations(arr, index + 1, zeros_remaining, ones_remaining - 1, board_dimensions, zero_wins, one_wins, ties);
  }
}

/**
 * Brute-force method
 * Generate all possible n x n boards, then count each one
 */
int simulate_brute_force(int n) {
  int board_size = n * n;
  int *board = (int *)malloc(board_size * sizeof(int));

  int zero_wins = 0;
  int one_wins = 0;
  int ties = 0;

  double zeros_remaining = floor(board_size / 2);
  double ones_remaining = ceil(board_size / 2);

  if (board_size % 2 != 0) {
    ones_remaining++;
  }

  // printf("Board size: %d\n Zeros remaining: %f\nOnes remaining: %f\n", board_size, zeros_remaining, ones_remaining);

  // permute(board, 0, board_size - 1, n * n, n, &zero_wins, &one_wins, &ties);
  generate_half_half_permutations(board, 0, zeros_remaining, ones_remaining, n, &zero_wins, &one_wins, &ties);

  printf("Zero wins: %d\nOne wins: %d\nTies: %d\n", zero_wins, one_wins, ties);

  printf("\nTotal runs: %d\n\n", zero_wins + one_wins + ties);

  return 0;
}

#include <chrono>
using namespace std::chrono;

int main() {
  auto start = high_resolution_clock::now();
  int n = 6;
  printf("Starting simulation\n\n");
  simulate_brute_force(n);
  printf("Simulation finished");
  auto end = high_resolution_clock::now();
  auto duration = duration_cast<seconds>(end - start);
  std::cout << "Time: " << duration.count() << "s";
  return 0;
}

/*

  int static_board[5][5] = {
      {0, 0, 0, 1, 0},
      {1, 1, 1, 0, 1},
      {0, 0, 1, 1, 0},
      {0, 0, 1, 1, 0},
      {0, 1, 0, 0, 1}};

  int n = 5;

  // copy static_board into int** board
  int *board = (int *)malloc(n * n * sizeof(int *));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      board[i * n + j] = static_board[i][j];
    }
  }

  calculate_winner(board, n);
  */