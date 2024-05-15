# General
This repo contains a breakout game in C++ using SDL2.

You can select a level from the menu and navigate through the pages with keyboard, click on the page buttons or use mouse wheel.

You can play by moving mouse or by using keyboard.

You can escape a game by pressing echap.
One second after the end of the game, it will escape automatically.

There is a system of life, you have three lives, if there is only one ball and it touch the bottom of the screen, you will lose a life.

## Bricks
There is three types of bricks:
- rectangular
- triangular
- hexagonal

## Power-ups
### Bonus
- Extend paddle
- Add ball
- Duplicate ball

### Malus
- Speed up ball
- reverse rendering


# Build Instructions
## Clone this repo
git clone https://github.com/LucasDltg/BrickBreaker  
cd BrickBreaker

## Create a build folder
mkdir build  
cd build  

## Build
cmake ..  
make  

## Run
./BrickBreaker
