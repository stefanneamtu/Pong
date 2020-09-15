#ifndef PADDLE
#define PADDLE

#define PADDLE_WIDTH 16
#define LEFT_PADDING 32

#define PLAYER_PADDLE_SPEED 8
#define COMPUTER_PADDLE_SPEED 5

static const SDL_Color CYAN = {0, 191, 255};

// paddle data structure
typedef struct paddle {
  SDL_Rect rect;
  SDL_Color color;
  int score;
  float speed;
} paddle_t;

// returns a pointer to a new paddle
paddle_t *new_paddle(void) {
  paddle_t *paddle = malloc(sizeof(paddle_t));
  if (!paddle) {
    fprintf(stderr, "Failed to allocate memory for paddle!\n");
    exit(EXIT_FAILURE);
  }

  // rgb color for cyan
  paddle->color = CYAN;
  
  paddle->score = 0;
  paddle->speed = PLAYER_PADDLE_SPEED;

  paddle->rect.x = LEFT_PADDING;
  paddle->rect.h = HEIGHT / 4;
  paddle->rect.y = HEIGHT / 2 + paddle->rect.h / 2;
  paddle->rect.w = PADDLE_WIDTH;
 
  return paddle;
}

// free the allocated memory
void free_paddle(paddle_t *paddle) {
  free(paddle);
}

// move the paddle up
void go_up(paddle_t *paddle) {
  paddle->rect.y -= paddle->speed;
  if (paddle->rect.y < 0) {
    paddle->rect.y = 0;
  }
}

// move the paddle down
void go_down(paddle_t *paddle) {
  paddle->rect.y += paddle->speed;
  if (paddle->rect.y + paddle->rect.h > HEIGHT) {
    paddle->rect.y = HEIGHT - paddle->rect.h;
  }
}

#endif
