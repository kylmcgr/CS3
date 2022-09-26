#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"

const char WINDOW_TITLE[] = "CS 3";
int WINDOW_WIDTH = 500;
int WINDOW_HEIGHT = 1000;
const double MS_PER_S = 1e3;
SDL_Texture* gTexture = NULL;
TTF_Font *gFont = NULL;



/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
mouse_handler_t mouse_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

// SDL_Surface* loadSurface( std::string path );
//
// SDL_Surface* loadSurface( std::string path ){
// 	//The final optimized image
// 	SDL_Surface* optimizedSurface = NULL;
//
// 	//Load image at specified path
// 	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
// 	if( loadedSurface == NULL )
// 	{
// 		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
// 	}
// 	else
// 	{
// 		//Convert surface to screen format
// 		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
// 		if( optimizedSurface == NULL )
// 		{
// 			printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
// 		}
//
// 		//Get rid of old loaded surface
// 		SDL_FreeSurface( loadedSurface );
// 	}
//
// 	return optimizedSurface;
// }
Mix_Chunk *loadEffects(char * effects_name){
  Mix_Chunk *effect = Mix_LoadWAV(effects_name);
  if( effect == NULL )
  {
      printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
      // success = false;
  }
  return effect;
}

Mix_Music *loadMedia(char* music_name)
{
    //Loading success flag
    // bool success = true;

    //Load prompt texture
    // if( !gPromptTexture.loadFromFile( "21_sound_effects_and_music/prompt.png" ) )
    // {
    //     printf( "Failed to load prompt texture!\n" );
    //     success = false;
    // }

    //Load music
    Mix_Music *music = Mix_LoadMUS( music_name );
    if( music == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        // success = false;
    }

// put
    //Load sound effects
    // gScratch = Mix_LoadWAV( "path" );
    // if( gScratch == NULL )
    // {
    //     printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    //     success = false;
    // }

    return music;
}

SDL_Texture* loadTexture(char* image_name){
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(image_name);
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", image_name, IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", image_name, SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

SDL_Texture *loadFromRenderedText( char* textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    // free();
    SDL_Texture* mTexture = NULL;
    //Render text surface
    TTF_Font *font = TTF_OpenFont("images/font.ttf", 25);
    if(font == NULL){
      printf( "Unable to render font! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText, textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        // else
        // {
        //     //Get image dimensions
        //     mWidth = textSurface->w;
        //     mHeight = textSurface->h;
        // }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }

    //Return success
    TTF_CloseFont(font);
    return mTexture;
}

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)
    };
    return pixel;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_scene_position(vector_t window_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t pixel_center_offset = vec_subtract(window_pos, window_center);
    double scale = 1/get_scene_scale(window_center);
    vector_t scene_center_offset = vec_multiply(scale, pixel_center_offset);
    vector_t scene_pos = {
        .x = round(center.x + scene_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(center.y - scene_center_offset.y)
    };
    return scene_pos;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:  return LEFT_ARROW;
        case SDLK_UP:    return UP_ARROW;
        case SDLK_RIGHT: return RIGHT_ARROW;
        case SDLK_DOWN:  return DOWN_ARROW;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
}

bool sdl_is_done(scene_t *scene) {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                free(event);
                TTF_Quit();
                return true;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event->key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                key_event_type_t type =
                    event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                key_handler(scene, key, type, held_time);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (mouse_handler == NULL) break;
                int *mousex = malloc(sizeof(int));
                int *mousey = malloc(sizeof(int));
                SDL_GetMouseState( mousex, mousey );
                vector_t window_center = get_window_center();
                vector_t xy = {*mousex, *mousey};
                xy = get_scene_position(xy, window_center);
                mouse_handler(scene, xy.x, xy.y);
                free(mousex);
                free(mousey);
                break;
        }
    }
    free(event);
    return false;
}

void sdl_clear(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, 255
    );
    free(x_points);
    free(y_points);
}

void sdl_show(void) {
    // Draw boundary lines
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = min_pixel.x;
    boundary->y = max_pixel.y;
    boundary->w = max_pixel.x - min_pixel.x;
    boundary->h = min_pixel.y - max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderDrawRect(renderer, boundary);
    free(boundary);

    SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene) {
    sdl_render_scene_back(scene, NULL);
}

SDL_Rect *getRect(list_t *shape){
  vector_t window_center = get_window_center();
  SDL_Rect *DestR = malloc(sizeof(SDL_Rect));
  vector_t *p = list_get(shape, 0);
  vector_t xy = {p->x, p->y};
  vector_t wh = {p->x, p->y};
  for (size_t i = 1; i < list_size(shape); i++) {
    p = list_get(shape, i);
    if (p->x < xy.x){
      xy.x = p->x;
    }
    if (p->y > xy.y){
      xy.y = p->y;
    }
    if (p->x > wh.x){
      wh.x = p->x;
    }
    if (p->y < wh.y){
      wh.y = p->y;
    }
  }
  xy = get_window_position(xy, window_center);
  wh = get_window_position(wh, window_center);
  DestR->x = xy.x;
  DestR->y = xy.y;
  DestR->w = wh.x - xy.x;
  DestR->h = wh.y - xy.y;
  return DestR;
}

void sdl_render_scene_back(scene_t *scene, SDL_Texture *background){
  sdl_clear();
  size_t body_count = scene_bodies(scene);
  if (background != NULL){
    SDL_RenderCopy(renderer, background, NULL, NULL);
  }
  for (size_t i = 0; i < body_count; i++) {
      body_t *body = scene_get_body(scene, i);
      list_t *shape = body_get_shape(body);
      SDL_Texture* texture = (SDL_Texture*) body_get_texture(body);
      SDL_Texture* text = (SDL_Texture*) body_get_text(body);
      SDL_Rect *DestR = getRect(shape);
      if (texture != NULL){
      	SDL_RenderCopy(renderer, texture, NULL, DestR);
      }
      else {
        sdl_draw_polygon(shape, body_get_color(body));
      }
      if (text != NULL) {
        double shift_factor = 0.25;
        double scale_factor = 0.5;
        DestR->x = DestR->x + shift_factor * DestR->w;
        DestR->y = DestR->y + shift_factor * DestR->h;
        DestR->w = DestR->w * scale_factor;
        DestR->h = DestR->h * scale_factor;
        SDL_RenderCopy(renderer, text, NULL, DestR);
      }
      list_free(shape);
      free(DestR);
  }
  sdl_show();
}

void sdl_on_key(key_handler_t handler) {
    key_handler = handler;
}

void sdl_on_mouse(mouse_handler_t handler) {
    mouse_handler = handler;
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}
