#include "start_menu.h"

// color declarations
static const SDL_Color WHITE = {255, 255, 255};

static const SDL_Color CYAN = {0, 191, 255};

static const SDL_Color BUTTON_HOVER_COLOR = {0, 118, 255};

// draws an element to the screen
static void render_menu_elem(SDL_Renderer *renderer, TTF_Font *font, 
		SDL_Rect *pos, SDL_Color color, const char *label) {
  SDL_Texture *texture;

  SDL_Surface *menu_button = TTF_RenderText_Solid(font, label, color);
  texture = SDL_CreateTextureFromSurface(renderer, menu_button);
  
  SDL_FreeSurface(menu_button);
  SDL_RenderCopy(renderer, texture, NULL, pos);
  SDL_DestroyTexture(texture);
}

static bool check_borders(SDL_Rect pos, int x, int y) {
  return x >= pos.x && x <= pos.x + pos.w && y >= pos.y && y <= pos.y + pos.h;
}

int render_menu(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *click_sound) {
  // set the background color 
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);

  // draw the background
  SDL_RenderClear(renderer);

  // draw the title
  SDL_Rect title_rect;

  // set the dimensions and coordinates of the title
  title_rect.w = TITLE_WIDTH;
  title_rect.h = TITLE_HEIGHT;
  title_rect.x = WIDTH / 2 - title_rect.w / 2;
  title_rect.y = TITLE_OFFSET - title_rect.h / 2;

  // draw the title
  render_menu_elem(renderer, font, &title_rect, CYAN, "PONG");

  // delcare the labels of the buttons
  const char *labels[NUMBER_OF_BUTTONS] = {"Hard", "Medium", "Easy"};

  // at first all buttons are not selected
  bool selected[NUMBER_OF_BUTTONS] = {false, false, false};

  // positions of the buttons
  SDL_Rect positions[NUMBER_OF_BUTTONS];

  // the y offset between the buttons
  int y_offset = 100;

  // draw all buttons and initialise their positions
  for (int i = 0; i < NUMBER_OF_BUTTONS; ++i) {
    SDL_Texture *texture;
    SDL_Surface *menu = TTF_RenderText_Solid(font, labels[i], WHITE);
    texture = SDL_CreateTextureFromSurface(renderer, menu);

    // initialise positions
    positions[i].w = menu->w;
    positions[i].h = menu->h;
    positions[i].x = WIDTH / 2 - positions[i].w / 2;
    positions[i].y = HEIGHT / 2 - positions[i].h + y_offset * i;

    // draw the buttons
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, texture, NULL, &positions[i]);
    SDL_DestroyTexture(texture);
  }

  SDL_Event event;

  // coordinates of the event
  int x, y;

  int frame_count, last_frame, last_time;
  frame_count = last_frame = last_time = 0;
  while (true) {

    // current time
    last_frame = SDL_GetTicks(); 

    // calculates the frames per second
    if (last_frame >= (last_time + SECOND_IN_MILLISECONDS)) {
      last_time = last_frame;
      frame_count = 0;
    }

    // increase the frame count
    ++frame_count;

    // make sure there are no more than 60 frames per second
    int timer_fps = SDL_GetTicks() - last_frame;
    if (timer_fps < SECOND_IN_MILLISECONDS / FPS) {
      SDL_Delay(SECOND_IN_MILLISECONDS / FPS - timer_fps);
    }

    while(SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: // quit
	  return -1;
        case SDL_MOUSEMOTION: 
          // get the coordinates of the event
	  x = event.motion.x;
          y = event.motion.y;

          for (int i = 0; i < NUMBER_OF_BUTTONS; ++i) {
	    // check if the a button is selected
            if (check_borders(positions[i], x, y)) {
              if (!selected[i]) {
                selected[i] = 1;
		render_menu_elem(renderer, font, &positions[i], 
				  BUTTON_HOVER_COLOR, labels[i]);
              }
            } else { // check if a previously selected button is deselected
              if (selected[i]) {
                selected[i] = 0;
		render_menu_elem(renderer, font, &positions[i], 
				   WHITE, labels[i]);
              }
            }
          }
          break;
        case SDL_MOUSEBUTTONDOWN:
	  // get the coordinates of the event
          x = event.button.x;
          y = event.button.y;
          for (int i = 0; i < NUMBER_OF_BUTTONS; ++i) {
	    // check if the button was pressed
            if (check_borders(positions[i], x, y)) {
	      Mix_PlayChannel(-1, click_sound, 0);
              return i;
            }
          }
          break;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            // if the user pressed ESC quit
	    return -1;
          }
          break;
      }
    }
    // push all drawings
    SDL_RenderPresent(renderer);
  }
}

