#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include "Ponkemo.h"

using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

struct Animation {
    vector<SDL_Rect> frames;
    int currentFrame;
    int frameDelay;
    int frameTimer;
    bool isPlaying;
};

void resetGame(Ponkemo& ponkemo, Ponkemo& chardriza, Animation& ponkemoAttackAnim, Animation& chardrizaAnim,
               bool& gameOver, vector<string>& battleLog, string& endGameMessage, PonkemoLevel& ponkemoLevel) {
    ponkemo = {"Ponkemo", 100, 100, 50, 50, 12, 20, 10, 30, 30};
    chardriza = {"Chardriza", 100, 100, 50, 50, 8, 15, 10, 25, 30};

    ponkemoAttackAnim.currentFrame = 0;
    ponkemoAttackAnim.frameTimer = 0;
    ponkemoAttackAnim.isPlaying = false;

    chardrizaAnim.currentFrame = 0;
    chardrizaAnim.frameTimer = 0;
    chardrizaAnim.isPlaying = false;

    gameOver = false;
    battleLog.clear();
    endGameMessage = "";

    ponkemoLevel = PonkemoLevel(); // Reset the level system
}

int main() {
    srand(time(0));

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0 || IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        cout << "Initialization failed: " << SDL_GetError() << " | " << IMG_GetError() << endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Ponkemo Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window creation failed: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer creation failed: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        cout << "Warning: Could not load font: " << TTF_GetError() << endl;
        cout << "Continuing without text display" << endl;
    }

    TTF_Font* endGameFont = TTF_OpenFont("arial.ttf", 48);
    if (!endGameFont) {
        cout << "Warning: Could not load end game font: " << TTF_GetError() << endl;
        endGameFont = font;
    }

    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    if (!backgroundTexture) {
        cout << "Failed to load background.png: " << IMG_GetError() << endl;
    }

    SDL_Texture* ponkemoSpritesheet = IMG_LoadTexture(renderer, "ponkemo_spritesheet.png");
    if (!ponkemoSpritesheet) {
        cout << "Failed to load ponkemo_spritesheet.png: " << IMG_GetError() << endl;
    }

    SDL_Texture* chardrizaSpritesheet = IMG_LoadTexture(renderer, "chardriza_spritesheet.png");
    if (!chardrizaSpritesheet) {
        cout << "Failed to load chardriza_spritesheet.png: " << IMG_GetError() << endl;
    }

    Animation ponkemoAttackAnim;
    ponkemoAttackAnim.currentFrame = 0;
    ponkemoAttackAnim.frameDelay = 150;
    ponkemoAttackAnim.frameTimer = 0;
    ponkemoAttackAnim.isPlaying = false;
    ponkemoAttackAnim.frames.push_back({0, 0, 64, 64});
    for (int i = 0; i < 5; ++i) {
        ponkemoAttackAnim.frames.push_back({64 + i * 64, 0, 64, 64});
    }

    Animation chardrizaAnim;
    chardrizaAnim.currentFrame = 0;
    chardrizaAnim.frameDelay = 150;
    chardrizaAnim.frameTimer = 0;
    chardrizaAnim.isPlaying = false;
    for (int i = 0; i < 8; ++i) {
        chardrizaAnim.frames.push_back({i * 64, 0, 64, 64});
    }

    Ponkemo ponkemo = {"Ponkemo", 100, 100, 50, 50, 12, 20, 10, 30, 30};
    Ponkemo chardriza = {"Chardriza", 100, 100, 50, 50, 8, 15, 10, 25, 30};
    PonkemoLevel ponkemoLevel; // Create a PonkemoLevel instance for Ponkemo

    bool running = true, gameOver = false;
    SDL_Event e;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    vector<string> battleLog;
    int lastAttackTime = 0;

    string endGameMessage = "";
    int blinkTimer = 0;
    const int BLINK_SPEED = 500;
    bool showEndGameText = true;

    while (running) {
        int currentTime = SDL_GetTicks();

        if (ponkemoAttackAnim.isPlaying) {
            ponkemoAttackAnim.frameTimer += currentTime - lastAttackTime;
            if (ponkemoAttackAnim.frameTimer >= ponkemoAttackAnim.frameDelay) {
                ponkemoAttackAnim.frameTimer = 0;
                ponkemoAttackAnim.currentFrame++;
                if (ponkemoAttackAnim.currentFrame >= (int)ponkemoAttackAnim.frames.size()) {
                    ponkemoAttackAnim.currentFrame = 0;
                    ponkemoAttackAnim.isPlaying = false;
                }
            }
        }

        if (chardrizaAnim.isPlaying) {
            chardrizaAnim.frameTimer += currentTime - lastAttackTime;
            if (chardrizaAnim.frameTimer >= chardrizaAnim.frameDelay) {
                chardrizaAnim.frameTimer = 0;
                chardrizaAnim.currentFrame++;
                if (chardrizaAnim.currentFrame >= (int)chardrizaAnim.frames.size()) {
                    chardrizaAnim.currentFrame = 0;
                    chardrizaAnim.isPlaying = false;
                }
            }
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (!gameOver && e.type == SDL_KEYDOWN) {
                int choice = 0;
                switch (e.key.keysym.sym) {
                    case SDLK_1: choice = 1; break;
                    case SDLK_2: choice = 2; break;
                    case SDLK_3: choice = 3; break;
                }
                if (choice) {
                    ponkemoAttackAnim.currentFrame = 1;
                    ponkemoAttackAnim.isPlaying = true;
                    attack(ponkemo, chardriza, choice, battleLog);
                    if (chardriza.hp > 0) {
                        int eChoice = rand() % 3 + 1;
                        chardrizaAnim.currentFrame = 0;
                        chardrizaAnim.isPlaying = true;
                        attack(chardriza, ponkemo, eChoice, battleLog);
                    }

                    if (ponkemo.hp <= 0) {
                        battleLog.push_back("Game Over! " + chardriza.name + " wins!");
                        endGameMessage = "Game Over! " + chardriza.name + " Wins!";
                        gameOver = true;
                    }
                    else if (chardriza.hp <= 0) {
                        battleLog.push_back("Victory! " + ponkemo.name + " wins!");
                        endGameMessage = "Victory! " + ponkemo.name + " Wins!";
                        ponkemoLevel.gainExp(64); // Gain 64 EXP when Ponkemo wins
                        gameOver = true;
                    }
                }
            }
            if (gameOver && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    resetGame(ponkemo, chardriza, ponkemoAttackAnim, chardrizaAnim, gameOver, battleLog, endGameMessage, ponkemoLevel);
                }
                else if (e.key.keysym.sym == SDLK_SPACE) {
                    running = false;
                }
            }
        }

        if (backgroundTexture) {
            SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        const int characterY = 280;

        if (ponkemoSpritesheet) {
            SDL_Rect ponkemoRect = {150, characterY, 150, 150};
            SDL_Rect srcRect = ponkemoAttackAnim.frames[ponkemoAttackAnim.currentFrame];
            SDL_RenderCopy(renderer, ponkemoSpritesheet, &srcRect, &ponkemoRect);
        }
        drawHealthBar(renderer, 150, characterY - 30, 150, 15, ponkemo.hp, ponkemo.maxHp);
        string p1Status = ponkemo.name + " - HP: " + to_string(ponkemo.hp) + " EP: " + to_string(ponkemo.ep) + " Level: " + to_string(ponkemoLevel.level);
        renderText(renderer, font, p1Status, 150, characterY - 80, white);

        if (chardrizaSpritesheet) {
            SDL_Rect chardrizaRect = {SCREEN_WIDTH - 350, characterY-100,150,150};
            SDL_Rect srcRect = chardrizaAnim.frames[chardrizaAnim.currentFrame];
            SDL_RenderCopy(renderer, chardrizaSpritesheet, &srcRect, &chardrizaRect);
        }
        drawHealthBar(renderer, SCREEN_WIDTH - 350, characterY - 100, 150, 15, chardriza.hp, chardriza.maxHp);
        string p2Status = chardriza.name + " - HP: " + to_string(chardriza.hp) + " EP: " + to_string(chardriza.ep);
        renderText(renderer, font, p2Status, SCREEN_WIDTH - 350, characterY - 150, white);

        SDL_Rect menuBox = {0, SCREEN_HEIGHT - 150, SCREEN_WIDTH, 150};
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &menuBox);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &menuBox);

        renderText(renderer, font, "Choose an action:", 20, SCREEN_HEIGHT - 130, black);
        renderText(renderer, font, "1. Basic Attack (No EP)", 20, SCREEN_HEIGHT - 100, black);
        renderText(renderer, font, "2. Medium Attack (-10 EP)", 20, SCREEN_HEIGHT - 70, black);
        renderText(renderer, font, "3. Strong Attack (-30 EP)", 20, SCREEN_HEIGHT - 40, black);

        int logY = SCREEN_HEIGHT - 130;
        for (size_t i = 0; i < battleLog.size() && i < 4; ++i) {
            renderText(renderer, font, battleLog[battleLog.size() - 1 - i], 400, logY, black);
            logY += 30;
        }

        if (gameOver && !endGameMessage.empty()) {
            blinkTimer += currentTime - lastAttackTime;
            if (blinkTimer >= BLINK_SPEED) {
                showEndGameText = !showEndGameText;
                blinkTimer = 0;
            }

            if (showEndGameText) {
                SDL_Color endGameColor = (endGameMessage.find("Game Over") != string::npos) ?
                                        SDL_Color{255, 0, 0, 255} : SDL_Color{0, 255, 0, 255};

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                SDL_Rect overlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_RenderFillRect(renderer, &overlayRect);

                SDL_Surface* textSurface = TTF_RenderText_Solid(endGameFont, endGameMessage.c_str(), endGameColor);
                if (textSurface) {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    if (textTexture) {
                        int textW = textSurface->w;
                        int textH = textSurface->h;
                        SDL_Rect textRect = {(SCREEN_WIDTH - textW) / 2, (SCREEN_HEIGHT - textH) / 2, textW, textH};
                        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                        SDL_DestroyTexture(textTexture);
                    }
                    SDL_FreeSurface(textSurface);
                }

                SDL_Color instructionColor = {255, 255, 255, 255};
                renderText(renderer, font, "Press Enter to Play Again, Space to Exit",
                          (SCREEN_WIDTH - 400) / 2, SCREEN_HEIGHT / 2 + 50, instructionColor);
            }
        }

        SDL_RenderPresent(renderer);
        lastAttackTime = currentTime;
    }

    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (ponkemoSpritesheet) SDL_DestroyTexture(ponkemoSpritesheet);
    if (chardrizaSpritesheet) SDL_DestroyTexture(chardrizaSpritesheet);
    if (font) TTF_CloseFont(font);
    if (endGameFont && endGameFont != font) TTF_CloseFont(endGameFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
