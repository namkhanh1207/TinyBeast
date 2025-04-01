#include "Ponkemo.h"
PonkemoLevel::PonkemoLevel(){
    level =1;
    exp =0;
    expToUp =64;
}
void drawHealthBar(SDL_Renderer* renderer, int x, int y, int width, int height, int current, int max) {
    SDL_Rect bg = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &bg);

    int barWidth = (current * width) / max;
    SDL_Rect fill = {x, y, barWidth, height};
    if (current > max/2) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    else if (current > max/4) SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fill);

    SDL_Rect outline = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &outline);
}

bool attack(Ponkemo &attacker, Ponkemo &defender, int attackType, std::vector<std::string> &battleLog) {
    int damage = 0;
    std::string message;
    switch (attackType) {
        case 1:
            damage = attacker.basicAttack;
            message = attacker.name + " uses Basic Attack!";
            break;
        case 2:
            if (attacker.ep < attacker.medAttackCost) {
                message = attacker.name + " doesn't have enough EP!";
                battleLog.push_back(message);
                return false;
            }
            damage = attacker.medAttack;
            attacker.ep -= attacker.medAttackCost;
            message = attacker.name + " uses Medium Attack!";
            break;
        case 3:
            if (attacker.ep < attacker.strongAttackCost) {
                message = attacker.name + " doesn't have enough EP!";
                battleLog.push_back(message);
                return false;
            }
            damage = attacker.strongAttack;
            attacker.ep -= attacker.strongAttackCost;
            message = attacker.name + " uses Strong Attack!";
            break;
        default:
            return false;
    }

    defender.hp = std::max(0, defender.hp - damage);
    battleLog.push_back(message);
    battleLog.push_back("Deals " + std::to_string(damage) + " damage!");
    return true;
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y,SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
void PonkemoLevel::gainExp (int a) {
    exp +=a;
    if (exp>expToUp){
        levelUp();
    }
}
void PonkemoLevel::levelUp (){
    exp-=expToUp;
    level++;
    if(level<=5){
        expToUp=expToUp*2;
    }
    else if (level>5&&level<=10) {
        expToUp+=1024;
    }
    std::cout << "Level up! Your level is " << level << std::endl;
}
