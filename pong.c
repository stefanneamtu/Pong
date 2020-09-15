#include "pong.h"
#include "paddle.h"
#include "ball.h"
#include "start_menu.h"

static const SDL_Color WHITE = {255, 255, 255};

// serves the ball
static void serve(paddle_t *left_paddle, paddle_t *right_paddle, ball_t *ball) {
  // put all paddles in the middle
  left_paddle->rect.y = (HEIGHT / 2) - (left_paddle->rect.h / 2);
  right_paddle->rect.y = left_paddle->rect.y;

  // decide the starting position of the ball
  if (left_paddle->score > right_paddle->score) {
    ball->rect.x = left_paddle->rect.x + (left_paddle->rect.w * 4);
    ball->velocity_x = ball->speed / 2;
  } else {
    ball->rect.x = right_paddle->rect.x - (right_paddle->rect.w * 4);
    ball->velocity_x = -ball->speed / 2;
  }

  // put the ball in the middle (height wise)
  ball->rect.y = HEIGHT / 2 - (BALL_SIZE / 2);

  // go straight
  ball->velocity_y = 0;
}

// prints text at coordinates x and y
static void write_to_screen(char *text, int x, int y, SDL_Color color, 
                       TTF_Font *font, SDL_Renderer *renderer) {

  SDL_Rect new_rectangle;

  SDL_Surface *surface;
  SDL_Texture *texture;

  surface = TTF_RenderText_Solid(font, text, color);
  texture = SDL_CreateTextureFromSurface(renderer, surface);

  // set the dimensions
  new_rectangle.w = surface->w;
  new_rectangle.h = surface->h;
  new_rectangle.x = x - new_rectangle.w;
  new_rectangle.y = y - new_rectangle.h;

  SDL_FreeSurface(surface);
  SDL_RenderCopy(renderer, texture, NULL, &new_rectangle);
  SDL_DestroyTexture(texture);
}

// pong math rules for bouncing
static double get_bounce_direction(SDL_Rect ball, SDL_Rect paddle) {
  double intersection_point = paddle.y + paddle.h / 2 - ball.y - BALL_SIZE / 2;
  double normalised_point = intersection_point / paddle.h * 2;
  return normalised_point * 5 * PI / 12;
}

// updates the positions of the drawings
static void update(paddle_t *left_paddle, paddle_t *right_paddle, ball_t *ball,
		Mix_Chunk *hit_sound, Mix_Chunk *score_sound) {

  // bounce the ball if it hits a paddle
  if (SDL_HasIntersection(&ball->rect, &right_paddle->rect)) {
    Mix_PlayChannel(-1, hit_sound, 0);
    double bounce = get_bounce_direction(ball->rect, right_paddle->rect);
    ball->velocity_x = -ball->speed * cos(bounce);
    ball->velocity_y = ball->speed * -sin(bounce);
  }

  if (SDL_HasIntersection(&ball->rect, &left_paddle->rect)) {
    Mix_PlayChannel(-1, hit_sound, 0);
    double bounce = get_bounce_direction(ball->rect, left_paddle->rect);
    ball->velocity_x = ball->speed * cos(bounce);
    ball->velocity_y = ball->speed * -sin(bounce);
  }

  // if the ball hits the left or right border, update the score and restart
  if (ball->rect.x <= 0) {
    Mix_PlayChannel(-1, score_sound, 0);
    ++right_paddle->score;
    serve(left_paddle, right_paddle, ball);
  }
  if (ball->rect.x + BALL_SIZE >= WIDTH) {
    Mix_PlayChannel(-1, score_sound, 0);
    ++left_paddle->score;
    serve(left_paddle, right_paddle, ball);
  }
  
  // update the ball's position
  if (ball->rect.y <= 0 || ball->rect.y + BALL_SIZE >= HEIGHT) {
    Mix_PlayChannel(-1, hit_sound, 0);
    ball->velocity_y = -ball->velocity_y;
  }
  move_ball(ball);
  
  // move the right paddle based on the ball's position
  if (ball->rect.y > right_paddle->rect.y + (right_paddle->rect.h / 2)) {
    go_down(right_paddle);
  }
  if (ball->rect.y < right_paddle->rect.y + (right_paddle->rect.h / 2)) {
    go_up(right_paddle);
  }
}

// takes input from the player
static void input(paddle_t *left_paddle, bool *running) {
  SDL_Event event;
  const Uint8 *keystates = SDL_GetKeyboardState(NULL);
  
  // exit the game
  while (SDL_PollEvent(&event) && *running) {
    *running = (event.type != SDL_QUIT);
  }
  if (keystates[SDL_SCANCODE_ESCAPE]) {
    *running = false;
  }

  // go up
  if (keystates[SDL_SCANCODE_UP]) {
    go_up(left_paddle);
  }

  // go down
  if (keystates[SDL_SCANCODE_DOWN]) {
    go_down(left_paddle);
  }
}

// renders all the game objects
static void render_game(paddle_t *left_paddle, paddle_t *right_paddle, ball_t *ball,
     int *frame_count, int last_frame, SDL_Renderer *renderer, TTF_Font *font) {

  // set the background color (in this case to black)
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);

  // draw the background
  SDL_RenderClear(renderer);

  // increase the frame count
  ++*frame_count;

  // make sure there are no more than 60 frames per second
  int timer_fps = SDL_GetTicks() - last_frame;
  if (timer_fps < SECOND_IN_MILLISECONDS / FPS) {
    SDL_Delay(SECOND_IN_MILLISECONDS / FPS - timer_fps);
  }

  // pick a color
  SDL_Color color = CYAN;

  // set the color of the drawings
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

  // add drawings here
  
  // generate the highest score
  int record = 0;
  FILE* fp = fopen("utility_files/high score", "r");
  if (fp) {
	fscanf(fp, "%d", &record);
	fclose(fp);
  } else {
	exit(EXIT_FAILURE);
  }

  if (left_paddle->score > record) {
	record = left_paddle->score;
	fp = fopen("utility_files/high score", "w");
	if (fp) {
	  fprintf(fp, "%d", record);
	  fclose(fp);
    } else {
	  exit(EXIT_FAILURE);
	}
  }

  char high_score[SCORE_SIZE];
  sprintf(high_score, "Highest Score: %d", record);
  //draw the highest score
  write_to_screen(high_score, WIDTH / 2 + FONT_SIZE * 4, FONT_SIZE * 1, 
                    WHITE, font, renderer);

  // draw the paddles
  SDL_RenderFillRect(renderer, &left_paddle->rect);
  SDL_RenderFillRect(renderer, &right_paddle->rect);

  // draw the ball
  SDL_RenderFillRect(renderer, &ball->rect);

  char score[SCORE_SIZE];
  // convert the score to a string
  sprintf(score, "%d    %d", left_paddle->score, right_paddle->score);

  // draw the score
  write_to_screen(score, WIDTH / 2 + FONT_SIZE, FONT_SIZE * 2, 
                    WHITE, font, renderer);
  
  // push all drawings
  SDL_RenderPresent(renderer);
}

// initialises the subsystems
static void initialise_subsystems(void) {
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "Failed at SDL init: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

int main(void) {

  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;

  Mix_Chunk *hit_sound, *score_sound, *background_music;

  initialise_subsystems();

  //SDL_WINDOW_FULLSCREEN instead of 0 to make it fullscreen
  if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
    fprintf(stderr, "Failed at SDL create window and renderer\n");
    exit(EXIT_FAILURE);
  }
 
  if (TTF_Init() < 0) {
    fprintf(stderr, "Failure at TTF init!\n");
    exit(EXIT_FAILURE);
  }

  font = TTF_OpenFont("utility_files/Peepo.ttf", FONT_SIZE);
  if (font == NULL) {
    fprintf(stderr, "Font not loaded!\n");
    exit(EXIT_FAILURE);
  }

  // load the sounds
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY / 2, MIX_DEFAULT_FORMAT, 
		  MIX_DEFAULT_CHANNELS, 64);

  hit_sound = Mix_LoadWAV("sounds/beeep.wav");
  if (hit_sound != NULL) {
    Mix_VolumeChunk(hit_sound, MIX_MAX_VOLUME);
  } else {
    fprintf(stderr, "Failed to load hit sound!\n");
    exit(EXIT_FAILURE);
  }
  score_sound = Mix_LoadWAV("sounds/peeeeeep.wav");
  if (score_sound != NULL) {
    Mix_VolumeChunk(score_sound, MIX_MAX_VOLUME);
  } else {
    fprintf(stderr, "Failed to load score sound!\n");
    exit(EXIT_FAILURE);
  }
  background_music = Mix_LoadWAV("sounds/background_music.wav");
  if (background_music != NULL) {
    Mix_VolumeChunk(background_music, MIX_MAX_VOLUME);
  } else {
    fprintf(stderr, "Failed to load background music!\n");
    exit(EXIT_FAILURE);
  }
  Mix_PlayChannel(-1, background_music, -1);

  bool running = true;

  difficulty_t difficulty;
  // exit or adjust the difficulty based on the selection
  switch(render_menu(renderer, font, hit_sound)) {
    case 0:
      difficulty = HARD;
      break;
    case 1:
      difficulty = MEDIUM;
      break;
    case 2:
      difficulty = EASY;
      break;
    default:
      running = false;
  }
  Mix_VolumeChunk(background_music, 30);

  // initialise the paddles 
  paddle_t *left_paddle, *right_paddle;
  left_paddle = new_paddle();
  right_paddle = new_paddle();
  right_paddle->rect.x = WIDTH - right_paddle->rect.w - 32;
  right_paddle->speed = COMPUTER_PADDLE_SPEED + difficulty;

  // initialise the ball
  ball_t *ball;
  ball = new_ball();
  ball->speed += BALL_SPEED_MULTIPLIER * difficulty;

  serve(left_paddle, right_paddle, ball);

  int frame_count, last_frame, last_time;
  last_time = last_frame = frame_count = 0;
  while (running) {

    // current time
    last_frame = SDL_GetTicks();

    // calculates the frames per second
    if (last_frame >= (last_time + SECOND_IN_MILLISECONDS)) {
      last_time = last_frame;
      frame_count = 0;
    }

    update(left_paddle, right_paddle, ball, hit_sound, score_sound);

    input(left_paddle, &running);

    render_game(left_paddle, right_paddle, ball, &frame_count, 
             last_frame, renderer, font);
  }
  free(ball);
  free(left_paddle);
  free(right_paddle);

  // close the things
  TTF_CloseFont(font);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  Mix_FreeChunk(hit_sound);
  Mix_FreeChunk(score_sound);
  Mix_FreeChunk(background_music);
  Mix_CloseAudio();

  SDL_Quit();

  return EXIT_SUCCESS;
}
