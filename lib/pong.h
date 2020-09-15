#ifndef PONG
#define PONG

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define WIDTH 1200
#define HEIGHT 680 

#define SCORE_SIZE 128

#define NUMBER_OF_BUTTONS 3

#define SECOND_IN_MILLISECONDS 1000
#define FPS 60

#define FONT_SIZE 62

#define PI 3.141592653589

typedef enum difficulty_speed_increase {
  EASY = 0,
  MEDIUM = 3,
  HARD = 6
} difficulty_t;

#endif
