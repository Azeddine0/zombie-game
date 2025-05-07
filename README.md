# 🧟 Zombie Survival Console Game

A simple yet addictive zombie survival game built in C++ for the Windows console. Survive endless waves of zombies, collect items, and test your reflexes!

## 🎮 Gameplay

You are a lone survivor trapped in a 2D grid world. Your mission: survive as many zombie waves as possible by moving, shooting, and collecting resources.

- Move using `W`, `A`, `S`, `D`
- Shoot in 4 directions using the **arrow keys**
- Collect **ammo (A)** and **health (+)** items
- Watch out for zombie attacks — they do 10 damage!
- Clear waves to earn score and trigger new, harder waves

## 🧠 Features

- ASCII-based grid map rendering
- Real-time input and movement (no "Enter" needed)
- Dynamic zombie AI that chases the player
- Item pickup system (health/ammo)
- Increasing difficulty with each wave
- Game stats and feedback messages
- Clean game over screen with final score

## 🛠 Requirements

- Windows OS
- C++ compiler (e.g., MSVC, MinGW)
- Console that supports `conio.h` and `windows.h`

## 🚀 How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/zombie-survival-cpp.git
   cd zombie-survival-cpp

## compile the game

g++ -o zombie_game mainj.cpp
