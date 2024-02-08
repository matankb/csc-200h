#include <chrono>
#include <iostream>
#include <math.h>
#include <stdio.h>

using namespace std::chrono;

/**
 * =======
 *  UTILS
 * =======
 */

int max(int a, int b) {
  return a > b ? a : b;
}

int max_all(int a, int b, int c, int d) {
  return max(a, max(b, max(c, d)));
}

int array_max(int *array, int n) {
  int max = 0;

  for (int i = 0; i < n; i++) {
    if (array[i] > max) {
      max = array[i];
    }
  }

  return max;
}

int get_cell(int *board, int n, int x, int y) {
  return board[(x * n) + y];
}

void set_cell(int *board, int n, int x, int y, int value) {
  board[(x * n) + y] = value;
}

void print_table(int *table, int n) {
  for (int i = 0; i < n * n; i++) {
    if (i % n == 0) {
      printf("\n");
    }

    printf("%d ", table[i]);
  }

  printf("\n");
}

/**
 * ==================
 * DYNAMIC STRUCTURES
 * ==================
 */

class Players {
  public:
  int zeros;
  int ones;
  int ties;
  Players(int zeros, int ones) {
    this->zeros = zeros;
    this->ones = ones;

    // TODO: make this struct better
    this->ties = 0;
  }
};

class CachedTables {
  public:
  int *horizontal;
  int *vertical;
  int *diagonal_even; // line x = y
  int *diagonal_odd;  // line x = -y

  CachedTables(int n) {
    this->horizontal = create_cached_table(n);
    this->vertical = create_cached_table(n);
    this->diagonal_even = create_cached_table(n);
    this->diagonal_odd = create_cached_table(n);
    this->n = n;
    this->maximums = new Players(0, 0);
  }

  // TODO: destructor

  // populate the table for a given index
  // with the longest chain that each cell is a part of
  void populate(int *board, int n, int index) {
    int player = board[index];

    bool is_top = index < n;
    bool is_leftmost = index % n == 0;
    bool is_rightmost = index % n == n - 1;

    // reset to one
    horizontal[index] = 1;
    vertical[index] = 1;
    diagonal_even[index] = 1;
    diagonal_odd[index] = 1;

    // check vertical - the cell above
    if (!is_top && board[index - n] == player) {
      vertical[index] += vertical[index - n];
    }

    // check horizontal - the cell to the left
    if (!is_leftmost && board[index - 1] == player) {
      horizontal[index] += horizontal[index - 1];
    }

    // check diagonal_even - the cell to the above-right
    if (!is_top && !is_rightmost && board[index - n + 1] == player) {
      diagonal_even[index] += diagonal_even[index - n + 1];
    }

    // check diagonal_odd - the cell to the above-left
    if (!is_top && !is_leftmost && board[index - n - 1] == player) {
      diagonal_odd[index] += diagonal_odd[index - n - 1];
    }
  }

  int get_winner(int *board) {
    int max_zeros = 0;
    int max_ones = 0;

    get_maximums(horizontal, board, &max_zeros, &max_ones);
    get_maximums(vertical, board, &max_zeros, &max_ones);
    get_maximums(diagonal_even, board, &max_zeros, &max_ones);
    get_maximums(diagonal_odd, board, &max_zeros, &max_ones);

    if (max_zeros > max_ones) {
      return 0;
    } else if (max_ones > max_zeros) {
      return 1;
    } else {
      return -1;
    }
  }

  private:
  int n;
  Players *maximums;
  int *create_cached_table(int n) {
    int *tables = new int[n * n];

    for (int i = 0; i < n * n; i++) {
      // each cell is a chain of 1 by itself
      tables[i] = 1;
    }

    return tables;
  }

  void get_maximums(int *table, int *board, int *max_zeros, int *max_ones) {
    for (int i = 0; i < (n * n); i++) {
      if (board[i] == 0 && table[i] > *max_zeros) {
        *max_zeros = table[i];
      }

      if (board[i] == 1 && table[i] > *max_ones) {
        *max_ones = table[i];
      }
    }
  };
};

int processed = 0;

void process(int *board, int n, int index, Players *remaining, CachedTables *tables, Players *wins) {
  if (index == n * n) {
    processed++;
    if (processed % 1000000 == 0) {
      // printf("Processed: %d\n", processed);
    }
    int winner = tables->get_winner(board);
    if (winner == 0) {
      wins->zeros++;
    } else if (winner == 1) {
      wins->ones++;
    } else {
      wins->ties++;
    }
    return;
  }

  // Generate permutations recursively

  if (remaining->zeros > 0) {
    board[index] = 0;
    tables->populate(board, n, index);
    remaining->zeros--;

    process(board, n, index + 1, remaining, tables, wins);

    remaining->zeros++;
  }

  if (remaining->ones > 0) {
    board[index] = 1;
    tables->populate(board, n, index);
    remaining->ones--;

    process(board, n, index + 1, remaining, tables, wins);

    remaining->ones++;
  }
}

Players *calculate_original_remaining(int n) {
  int cells = n * n;
  Players *remaining = new Players(floor(cells / 2), ceil(cells / 2));

  if (cells % 2 != 0) {
    remaining->ones++;
  }

  return remaining;
}

// put a random combination of 0s and 1s on the board
void generate_random_board(int *board, int n, int zeros, int ones) {
  for (int i = 0; i < n * n; i++) {
    board[i] = 0;
  }

  for (int i = 0; i < ones; i++) {
    int index = rand() % (n * n);
    while (board[index] != 0) {
      index = rand() % (n * n);
    }
    board[index] = 1;
  }
}

void run_random_sample(int n, int trials) {
  int *board = new int[n * n];

  srand(time(NULL));

  CachedTables *tables = new CachedTables(n);
  Players *remaining = calculate_original_remaining(n);
  Players *wins = new Players(0, 0);

  for (int i = 0; i < trials; i++) {
    // generate an independent random board
    generate_random_board(board, n, remaining->zeros, remaining->ones);

    if (i % 100 == 0) {
      printf("Trial: %d (%0.001f%%)\n", i, ((float)i / (float)trials) * 100);
    }

    for (int j = 0; j < n * n; j++) {
      tables->populate(board, n, j);
    }

    int winner = tables->get_winner(board);

    if (winner == 0) {
      wins->zeros++;
    } else if (winner == 1) {
      wins->ones++;
    } else {
      wins->ties++;
    }
  }

  printf("Zero wins: %d (%0.01f%%)\n", wins->zeros, ((float)wins->zeros / (float)trials) * 100);
  printf("One wins: %d (%0.01f%%)\n", wins->ones, ((float)wins->ones / (float)trials) * 100);
  printf("Ties: %d (%0.01f%%)\n", wins->ties, ((float)wins->ties / (float)trials) * 100);
  printf("Trials: %d\n", trials);
}

void run_exact(int n) {

  int cells = n * n;
  int *board = new int[n * n];

  for (int i = 0; i < n * n; i++) {
    board[i] = 0;
  }

  CachedTables *tables = new CachedTables(n);
  Players *wins = new Players(0, 0);
  Players *remaining = calculate_original_remaining(n);

  process(board, n, 0, remaining, tables, wins);

  printf("Zero wins: %d (%0.01f%%)\n", wins->zeros, ((float)wins->zeros / (float)processed) * 100);
  printf("One wins: %d (%0.01f%%)\n", wins->ones, ((float)wins->ones / (float)processed) * 100);
  printf("Ties: %d (%0.01f%%)\n", wins->ties, ((float)wins->ties / (float)processed) * 100);
  printf("Total: %d\n", processed);
}

int main() {
  printf("Starting\n");

  auto start = high_resolution_clock::now();

  int n = 500;
  int samples = 100000;

  printf("\n==== EXACT ====\n");
  run_exact(n);

  printf("\n==== RANDOM SAMPLE ====\n");
  // run_random_sample(n, samples);

  auto end = high_resolution_clock::now();
  auto duration = duration_cast<seconds>(end - start);
  std::cout << "Time: " << duration.count() << "s";
  return 0;
}