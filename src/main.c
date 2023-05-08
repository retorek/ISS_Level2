#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define FONT_PATH   "assets/pacifico/Pacifico.ttf"






const int SCREEN_WIDTH = 656;
const int SCREEN_HEIGHT = 400;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct letter{
    SDL_Texture* letter_texture;
    double x;
    double y;
    double originalX;
    double originalY;
    int position;
};
struct letter Letter;

struct letter letters[10];

int decode[6] = {0, 0, 0, 0, 0, 0};

double letters_x[6] = {25.14, 131.8943, 235.5178, 340.6578, 445.7978, 550.9378};

double letters_y = 121.2934;

SDL_Texture* load_texture(const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void draw_texture(SDL_Texture* texture, double x, double y) {
    SDL_Rect rect = { x, y, 0, 0 };
    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}


SDL_Texture* load_and_crop_image(const char* filename, double x, double y, double w, double h) {
    SDL_Surface* surface = IMG_Load(filename);
    SDL_Surface* cropped_surface = SDL_CreateRGBSurface(surface->flags, w, h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    SDL_Rect rect = { x, y, w, h };
    SDL_BlitSurface(surface, &rect, cropped_surface, NULL);
    SDL_Texture* cropped_texture = SDL_CreateTextureFromSurface(renderer, cropped_surface);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(cropped_surface);
    return cropped_texture;
}

void draw_white_box(SDL_Texture* texture, double x, double y, double w, double h) {
    // Get the dimensions of the texture
    int tex_w, tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);

    // Calculate the source rectangle
    SDL_Rect src_rect = { 0, 0, tex_w, tex_h };

    // Calculate the destination rectangle
    SDL_Rect dst_rect = { x, y, w, h };

    // Render the texture
    SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);

    // Fill the original place with a white box
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Set the color to white
    SDL_Rect fill_rect = { x, y, tex_w, tex_h };
    SDL_RenderFillRect(renderer, &fill_rect);
}

int check_box(double x, double y){
    if(x > 120 && x < 540 && y > 219 && y < 387){
        return 1;
    }else{
        return 0;
    }
    return 1;
}

int check_letter_nbr(double x, double y){
    for(int i = 0; i < 6; i++){
        if(x > letters_x[i] && x < (letters_x[i] + 84) && y > letters_y && y < (letters_y + 84)){
            return 1;
        }
    }
    return 0;
}

int get_letter_nbr(double x, double y, double n){
    for(int i = 0; i < 6; i++){
        if(x > letters_x[i] && x < (letters_x[i] + 84) && y > letters_y && y < (letters_y + 84)){
            return i;
        }
    }
    return n;
}

int find_empty_box(int n){
    for(int i = 0; i < 6; i++){
        if(decode[i] == 0) return i;
    }
    return n;
}

int get_letter(int x, int y){
    x = (int)(x - 120) / 84;
    y = (int)(y - 219) / 84;
    if(y == 0){
        return x;
    }else{
        return x + 5;
    }
}

int check_right_letter(int letter_box, int letter_nbr){
    if(letter_box == 0 && letter_nbr == 7){
        return 1;
    }else if(letter_box == 1 && letter_nbr == 1){
        return 1;
    }else if(letter_box == 2 && letter_nbr == 6){
        return 1;
    }else if((letter_box == 3 || letter_box == 4) && (letter_nbr == 3 || letter_nbr == 5)){
        return 1;
    }else if(letter_box == 5 && letter_nbr == 9){
        return 1;
    }
    return 0;
}

int check_level_success(void){
    for(int i = 0; i < 6; i++){
        if(decode[i] != 2) return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {



    (void) argc;
    (void) argv;


    // Initialize SDL2 and create a window and renderer
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();


    window = SDL_CreateWindow("The Big 3: Level 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("assets/pacifico/Pacifico.ttf", 32);
    //SDL_Color color = {0, 0, 0, 0};

    SDL_Texture* background_texture = load_texture("assets/level2_background.png");
    SDL_Texture* cropped_letters_texture = load_and_crop_image("assets/level2_background.png", 0, 219, SCREEN_WIDTH, SCREEN_HEIGHT - 219);
    SDL_Texture* letters_texture = load_texture("assets/letters.png");
    SDL_Texture* hint1_texture = load_texture("assets/caeserCipherDisk.png");



    SDL_Event event;
    int letter_box = 0;
    double x;
    double y;

    while(!check_level_success()){

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            }else if(event.type == SDL_MOUSEBUTTONDOWN){
                x = event.button.x;
                y = event.button.y;
            }
        }

        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        draw_texture(hint1_texture, 0, 0);
        draw_texture(background_texture, 0, 0);
        draw_white_box(cropped_letters_texture, 0, 219, SCREEN_WIDTH, SCREEN_HEIGHT - 219);

        letters[0].x = 120;
        letters[0].y = 219;
        letters[0].originalX = letters[0].x;
        letters[0].originalY = letters[0].y;
        letters[0].letter_texture = load_and_crop_image("assets/letters.png", 0, 0, 84, 84);
        letters[0].position = -1;
        draw_texture(letters[0].letter_texture, letters[0].x, letters[0].y);

        for(int i = 1; i < 5; i++){
            letters[i].x = letters[i-1].x + 84;
            letters[i].y = letters[i-1].y;
            letters[i].originalX = letters[i].x;
            letters[i].originalY = letters[i].y;
            letters[i].letter_texture = load_and_crop_image("assets/letters.png", i * 84, 0, 84, 84);
            letters[i].position = -1;
            draw_texture(letters[i].letter_texture, letters[i].x, letters[i].y);
        }

        for(int i = 5; i < 10; i++){
            letters[i].x = letters[i - 5].x;
            letters[i].y = letters[i - 5].y + 84;
            letters[i].originalX = letters[i].x;
            letters[i].originalY = letters[i].y;
            letters[i].letter_texture = load_and_crop_image("assets/letters.png", (i - 5) * 84, 84, 84, 84);
            letters[i].position = -1;
            draw_texture(letters[i].letter_texture, letters[i].x, letters[i].y);
        }


        //Box Filling
        if(check_box(x, y)){

            int letter_nbr = get_letter(x, y);
            int nbr_x = (letter_nbr > 4) ? letter_nbr - 5: letter_nbr;
            int nbr_y = (letter_nbr > 4) ? 1 : 0;

            SDL_Texture* letter_texture = load_and_crop_image("assets/letters.png", nbr_x*84, nbr_y*84, 84, 84);

            if(decode[letter_box] != 0){
                letter_box = find_empty_box(letter_box);

                printf("Not empty and box: %i \n", letter_box);
            }

            decode[letter_box] = 1;

            if(check_right_letter(letter_box, letter_nbr)){
                decode[letter_box] = 2;
            }

            draw_white_box(letter_texture, 120 + nbr_x*84, 219 + nbr_y*84, 84, 84);

            draw_texture(letter_texture, letters_x[letter_box], letters_y);


        }else if(check_letter_nbr(x, y)){

            letter_box = get_letter_nbr(x, y, letter_box);

            if(decode[letter_box] != 0){
                SDL_Texture* empty_box_texture = load_and_crop_image("assets/level2_background.png", 25.14, 121.2934, 84, 84);
                draw_texture(empty_box_texture, letters_x[letter_box], letters_y);
            }

        }


        SDL_RenderPresent(renderer);



    }


    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(cropped_letters_texture);
    SDL_DestroyTexture(letters_texture);



    SDL_Color color1 = {255, 68, 51, 0};

    SDL_Surface* surface1 = TTF_RenderText_Solid(font, "LEVEL 2 Passed", color1);
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Rect rect1 = {SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 40, surface1->w, surface1->h};
    SDL_RenderCopy(renderer, texture1, NULL, &rect1);


    SDL_Delay(1000);





    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
    return 0;

}
