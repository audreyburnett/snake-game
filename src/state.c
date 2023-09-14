#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.

  game_state_t* default_state = malloc(sizeof(game_state_t));
  default_state->num_rows = 18;
  default_state->num_snakes = 1;
  default_state->snakes = malloc(sizeof(struct snake_t));
  (default_state->snakes)->tail_row = 2;
  (default_state->snakes)->tail_col = 2;
  (default_state->snakes)->head_row = 2;
  (default_state->snakes)->head_col = 4;
  (default_state->snakes)->live = true;
  int num_col = 20;
  
  default_state->board = malloc(sizeof(char*) * (default_state->num_rows + 1));

  for (int i = 0; i < (default_state->num_rows + 1); i++) {
      default_state->board[i] = malloc(sizeof(char) * 21);
  }

  for (int i = 0; i <= num_col; i++) {
    for (int j = 0; j < (default_state->num_rows + 1); j++) {
      if (i == 0 || i == num_col - 1) {
        default_state->board[j][i] = '#';
      } else if (i == num_col) {
        default_state->board[j][i] = '\0';
      } else if (j == 0 || j == default_state->num_rows -1) {
        default_state->board[j][i] = '#';
      } else if (j == 2) {
        if (i == 2) {
          default_state->board[j][i] = 'd';
        } else if (i == 3) {
          default_state->board[j][i] = '>';
        } else if (i == 4) {
          default_state->board[j][i] = 'D';
        } else if (i == 9) {
          default_state->board[j][i] = '*';
        } else {
          default_state->board[j][i] = ' ';
        }
      } else {
        default_state->board[j][i] = ' ';
      }
    }
  }
  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  free(state -> snakes);
  for (int i = 0; i < (state->num_rows + 1); i++) {
    free(state->board[i]);
  }
  free(state -> board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  unsigned int NUM_ROWS = state -> num_rows;
  for (int i = 0; i < NUM_ROWS; i ++) {
      fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w'|| c == 'a' || c == 's'|| c== 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'W'|| c == 'A' || c == 'S'|| c== 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
    c = 'w';
  } else if (c == '<') {
    c = 'a';
  } else if (c == 'v') {
    c = 's';
  } else if (c == '>') {
    c = 'd';
  } else {
    c = '?';
  }
  return c;
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
    c = '^';
  } else if (c == 'A') {
    c = '<';
  } else if (c == 'S') {
    c = 'v';
  } else if (c == 'D') {
    c = '>';
  } else {
    c = '?';
  }
  return c;
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row -1;
  } else {
    return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col -1;
  } else {
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  struct snake_t our_snake = (state -> snakes[snum]);
  char c1 = state -> board[our_snake.head_row][our_snake.head_col];
  char c2 = state -> board[our_snake.head_row][our_snake.head_col];
  unsigned int row = get_next_row(our_snake.head_row, c1);
  unsigned int col = get_next_col(our_snake.head_col, c2);  
  return state -> board[row][col];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  struct snake_t *our_snake = &(state -> snakes[snum]);

  unsigned int col = our_snake ->head_col;
  unsigned int row = our_snake ->head_row;
  char head = state -> board[row][col];
  state -> board[row][col] = head_to_body(head);

  unsigned int new_row = get_next_row(row, head);
  unsigned int new_col = get_next_col(col, head);  
  state -> board[new_row][new_col] = head;
  our_snake -> head_col = new_col;
  our_snake -> head_row = new_row;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  struct snake_t *our_snake = &(state -> snakes[snum]);

  unsigned int col = our_snake ->tail_col;
  unsigned int row = our_snake ->tail_row;
  char tail = state -> board[row][col];
  state -> board[row][col] = ' ';

  unsigned int new_row = get_next_row(row, tail);
  unsigned int new_col = get_next_col(col, tail);  
  state -> board[new_row][new_col] = body_to_tail(state -> board[new_row][new_col]);
  our_snake -> tail_col = new_col;
  our_snake -> tail_row = new_row;
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  int num_snakes = state -> num_snakes;
  for (int i = 0; i < num_snakes; i ++) {
    struct snake_t *our_snake = &(state -> snakes[i]);
    if (is_snake(next_square(state, i)) || next_square(state, i) == '#') {
      our_snake -> live = false;
      unsigned int col = our_snake ->head_col;
      unsigned int row = our_snake ->head_row;
      state -> board[row][col] = 'x';
    } else if (next_square(state, i) == '*') {
      update_head(state, i);
      add_food(state);
    } else {
      update_head(state, i);
      update_tail(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  game_state_t* state = malloc(sizeof(game_state_t));
  state -> num_snakes = 0;
  state -> snakes = NULL;
  state -> board = malloc(sizeof(char*));
  
  char c;
  unsigned int row = 0;
  
  while (!feof(fp)) {
      state->board = realloc(state->board, sizeof(char*) * (row + 1));
      int col = 0;
      c = fgetc(fp);
      state->board[row] = malloc(sizeof(char) * 1);

      while((c != '\n') && !feof(fp)) {
          state->board[row] = realloc(state->board[row], sizeof(char) * (col + 2));
          state->board[row][col] = c;
          
          col++;
          c = fgetc(fp);
      }
      state->board[row][col] = '\0';
      row++;
  }

  fclose(fp);
  
  if (!(row == 0)) {
      row -= 1;
  }
  
  state->num_rows = row;
  return state;
}
  

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  unsigned int row = (state->snakes[snum]).tail_row;
  unsigned int col = (state->snakes[snum]).tail_col;
  char c = state->board[row][col];

  if (c == 'w') {
      row -= 1; 
  } else if (c == 'a') {
      col -= 1;
  } else if (c == 's') {
      row += 1;
  } else if (c == 'd') {
      col += 1;
  }

  c = state->board[row][col];
  while (!(c == 'W' || c == 'A' || c == 'S' || c == 'D')) {
      row = get_next_row(row, c);
      col = get_next_col(col, c);
      c = state->board[row][col];
  }    
  
  (state->snakes[snum]).head_row = row;
  (state->snakes[snum]).head_col = col;
    
  return;
}
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  
  unsigned int snake_num = 0;

  for (int row = 0; row < state->num_rows; row++) {
      int col = 0;
      char c = state->board[row][col];
      while (c != '\0') {
          if (is_tail(c)) {
              state->snakes = realloc(state->snakes, sizeof(struct snake_t) * (snake_num + 1));
              //state->snakes[snake_num] = (snake_t) malloc(sizeof(struct snake_t));
              state->snakes[snake_num].tail_row = row;
              state->snakes[snake_num].tail_col = col;
              state->snakes[snake_num].live = true;
              find_head(state, snake_num);
              snake_num++;
          }
          col++;
          c = state->board[row][col];
      }
  }

  state->num_snakes = snake_num;
  return state;
}

