#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct Ponkemo {
    string name;
    int hp, ep;
    int basicAttck, medAttck, medAttckCost;
    int strgAttck, strgAttckCost;
};

void drawHealthBar(SDL_Renderer* renderer, int x, int y, int width, int height, int current, int max) {
    SDL_Rect outline = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &outline);

    int barWidth = (current * width) / max;
    SDL_Rect fill = {x, y, barWidth, height};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &fill);
}

void attck(Ponkemo &attcker, Ponkemo &defder, int attckType) {
    int damage = 0;
    if (attckType == 1) {
        damage = attcker.basicAttck;
        cout << attcker.name << " uses Basic Attack!" << endl;
    } else if (attckType == 2 && attcker.ep >= attcker.medAttckCost) {
        damage = attcker.medAttck;
        attcker.ep -= attcker.medAttckCost;
        cout << attcker.name << " uses Medium Attack!" << endl;
    } else if (attckType == 3 && attcker.ep >= attcker.strgAttckCost) {
        damage = attcker.strgAttck;
        attcker.ep -= attcker.strgAttckCost;
        cout << attcker.name << " uses Strong Attack!" << endl;
    } else {
        cout << attcker.name << " doesn't have enough EP!" << endl;
        return;
    }
    defder.hp -= damage;
    cout << "Deals " << damage << " damage!" << endl;
    if (defder.hp <= 0) cout << defder.name << " is defeated!" << endl;
}

int main() {
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Ponkemo Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Ponkemo ponkemo = {"Ponkemo", 100, 50, 12, 20, 10, 30, 30};
    Ponkemo chardriza = {"Chardriza", 100, 50, 8, 15, 10, 25, 30};

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                int choice = 0;
                switch (e.key.keysym.sym) {
                    case SDLK_1: choice = 1; break;
                    case SDLK_2: choice = 2; break;
                    case SDLK_3: choice = 3; break;
                }
                if (choice) attck(ponkemo, chardriza, choice);
                if (chardriza.hp > 0) {
                    int eChoice = rand() % 3 + 1;
                    attck(chardriza, ponkemo, eChoice);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawHealthBar(renderer, 50, 80, 200, 20, ponkemo.hp, 100);
        drawHealthBar(renderer, 600, 80, 200, 20, chardriza.hp, 100);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
