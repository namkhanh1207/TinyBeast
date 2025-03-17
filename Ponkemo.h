#ifndef PONKEMO_H
#define PONKEMO_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

struct Ponkemo {
    std::string name;
    int hp, maxHp, ep, maxEp;
    int basicAttack, medAttack, medAttackCost;
    int strongAttack, strongAttackCost;
};

void drawHealthBar(SDL_Renderer* renderer, int x, int y, int width, int height, int current, int max);
bool attack(Ponkemo &attacker, Ponkemo &defender, int attackType, std::vector<std::string> &battleLog);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);

#endif
