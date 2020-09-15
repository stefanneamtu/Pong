#ifndef BALL
#define BALL

#define BALL_SIZE 16
#define BASE_BALL_SPEED 12
#define BALL_SPEED_MULTIPLIER 0.75

// ball data structure
typedef struct ball {
  SDL_Rect rect;
  float velocity_x;
  float velocity_y;
  float speed;
} ball_t;

// returns a pointer to a newly created ball
ball_t *new_ball(void) {
  ball_t *ball = malloc(sizeof(ball_t));
  if (!ball) {
    fprintf(stderr, "Failed to allocate memory for ball!\n");
    exit(EXIT_FAILURE);
  }
  ball->rect.w = ball->rect.h = BALL_SIZE;
  ball->speed = BASE_BALL_SPEED;
  return ball;
}

// frees the allocated memory
void free_ball(ball_t *ball) {
  free(ball);
}

// moves the ball according to the velocity
void move_ball(ball_t *ball) {
  ball->rect.x += ball->velocity_x;
  ball->rect.y += ball->velocity_y;
}

#endif
