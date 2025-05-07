#include <iostream>
#include <vector>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <algorithm>
#include <string>

// Game constants
const int WIDTH = 40;
const int HEIGHT = 20;

// Entity struct
struct Entity {
    int x, y;
    char symbol;
    int health;
    
    Entity(int x = 0, int y = 0, char symbol = '?', int health = 1) 
        : x(x), y(y), symbol(symbol), health(health) {}
};

// Game state
struct GameState {
    Entity player;
    std::vector<Entity> zombies;
    std::vector<Entity> items;
    int ammo;
    int score;
    int wave;
    bool gameOver;
    std::string message;
};

// Function declarations
void initGame(GameState& game);
void handleInput(GameState& game);
void updateGame(GameState& game);
void renderGame(const GameState& game);
void spawnZombies(GameState& game, int count);
void spawnItems(GameState& game, int count);
void shoot(GameState& game, int dirX, int dirY);
bool isWalkable(int x, int y);

void initGame(GameState& game) {
    // Initialize player
    game.player = Entity(WIDTH / 2, HEIGHT / 2, '@', 100);
    
    // Initialize game state
    game.ammo = 15;
    game.score = 0;
    game.wave = 1;
    game.gameOver = false;
    game.message = "Welcome to Zombie Survival! WASD to move, Arrow keys to shoot.";
    
    // Spawn initial zombies
    spawnZombies(game, 5);
    
    // Spawn initial items
    spawnItems(game, 3);
}

void handleInput(GameState& game) {
    if (_kbhit()) {
        char key = _getch();
        int newX = game.player.x;
        int newY = game.player.y;
        
        // Movement
        switch (key) {
            case 'w': case 'W': newY--; break;
            case 's': case 'S': newY++; break;
            case 'a': case 'A': newX--; break;
            case 'd': case 'D': newX++; break;
            case 'q': case 'Q': game.gameOver = true; break;
            
            // Shooting with arrow keys
            case 72: shoot(game, 0, -1); break;  // Up
            case 80: shoot(game, 0, 1); break;   // Down
            case 75: shoot(game, -1, 0); break;  // Left
            case 77: shoot(game, 1, 0); break;   // Right
        }
        
        // Check if movement is valid
        if (isWalkable(newX, newY)) {
            game.player.x = newX;
            game.player.y = newY;
            
            // Check for item collection
            for (size_t i = 0; i < game.items.size(); i++) {
                if (game.items[i].x == newX && game.items[i].y == newY) {
                    if (game.items[i].symbol == 'A') {
                        game.ammo += 5;
                        game.message = "Picked up ammo: +5";
                    } else {
                        game.player.health += 20;
                        game.message = "Picked up health: +20";
                    }
                    game.items.erase(game.items.begin() + i);
                    break;
                }
            }
        }
    }
}

void updateGame(GameState& game) {
    // Move zombies towards player
    for (auto& zombie : game.zombies) {
        // Simple movement: move towards player
        if (rand() % 2 == 0) { // Only move every other frame
            int dx = (zombie.x < game.player.x) ? 1 : (zombie.x > game.player.x) ? -1 : 0;
            int dy = (zombie.y < game.player.y) ? 1 : (zombie.y > game.player.y) ? -1 : 0;
            
            // Randomly choose horizontal or vertical movement
            if (rand() % 2 == 0 && dx != 0) {
                zombie.x += dx;
            } else if (dy != 0) {
                zombie.y += dy;
            }
            
            // Check for collision with player
            if (zombie.x == game.player.x && zombie.y == game.player.y) {
                game.player.health -= 10;
                game.message = "Ouch! Zombie attack: -10 health";
                
                if (game.player.health <= 0) {
                    game.gameOver = true;
                    return;
                }
            }
        }
    }
    
    // Remove dead zombies
    game.zombies.erase(
        std::remove_if(game.zombies.begin(), game.zombies.end(),
            [&game](const Entity& z) { 
                if (z.health <= 0) {
                    game.score += 10;
                    return true;
                }
                return false;
            }),
        game.zombies.end()
    );
    
    // Check if wave is cleared
    if (game.zombies.empty()) {
        game.wave++;
        game.ammo += 10;
        game.message = "Wave " + std::to_string(game.wave-1) + " cleared! +10 ammo. Next wave starting...";
        
        // Spawn items between waves
        spawnItems(game, 3);
        
        // Start new wave
        spawnZombies(game, 5 + game.wave);
    }
}

void renderGame(const GameState& game) {
    system("cls");
    
    // Create temporary grid for rendering
    char grid[HEIGHT][WIDTH];
    
    // Initialize with empty space
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x] = '.';
        }
    }
    
    // Add items
    for (const auto& item : game.items) {
        grid[item.y][item.x] = item.symbol;
    }
    
    // Add zombies
    for (const auto& zombie : game.zombies) {
        grid[zombie.y][zombie.x] = zombie.symbol;
    }
    
    // Add player
    grid[game.player.y][game.player.x] = game.player.symbol;
    
    // Render grid
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << grid[y][x];
        }
        std::cout << std::endl;
    }
    
    // Render stats
    std::cout << "Health: " << game.player.health 
              << " | Ammo: " << game.ammo 
              << " | Score: " << game.score 
              << " | Wave: " << game.wave 
              << " | Zombies: " << game.zombies.size() << std::endl;
    
    // Render controls
    std::cout << "Controls: WASD to move, Arrow keys to shoot, Q to quit" << std::endl;
    
    // Render message
    std::cout << game.message << std::endl;
}

void spawnZombies(GameState& game, int count) {
    for (int i = 0; i < count; i++) {
        Entity zombie;
        zombie.symbol = 'Z';
        zombie.health = 1 + (game.wave / 3);
        
        // Spawn at edge of map
        int side = rand() % 4;
        switch (side) {
            case 0: // Top
                zombie.x = rand() % WIDTH;
                zombie.y = 0;
                break;
            case 1: // Right
                zombie.x = WIDTH - 1;
                zombie.y = rand() % HEIGHT;
                break;
            case 2: // Bottom
                zombie.x = rand() % WIDTH;
                zombie.y = HEIGHT - 1;
                break;
            case 3: // Left
                zombie.x = 0;
                zombie.y = rand() % HEIGHT;
                break;
        }
        
        game.zombies.push_back(zombie);
    }
}

void spawnItems(GameState& game, int count) {
    for (int i = 0; i < count; i++) {
        Entity item;
        
        // 50/50 chance for ammo or health
        if (rand() % 2 == 0) {
            item.symbol = 'A'; // Ammo
        } else {
            item.symbol = '+'; // Health
        }
        
        // Find valid position
        bool validPosition = false;
        while (!validPosition) {
            item.x = rand() % WIDTH;
            item.y = rand() % HEIGHT;
            
            // Check if position is empty
            validPosition = true;
            
            // Check for player
            if (item.x == game.player.x && item.y == game.player.y) {
                validPosition = false;
                continue;
            }
            
            // Check for zombies
            for (const auto& zombie : game.zombies) {
                if (zombie.x == item.x && zombie.y == item.y) {
                    validPosition = false;
                    break;
                }
            }
            
            // Check for other items
            for (const auto& existingItem : game.items) {
                if (existingItem.x == item.x && existingItem.y == item.y) {
                    validPosition = false;
                    break;
                }
            }
        }
        
        game.items.push_back(item);
    }
}

void shoot(GameState& game, int dirX, int dirY) {
    if (game.ammo <= 0) {
        game.message = "Out of ammo!";
        return;
    }
    
    game.ammo--;
    
    int x = game.player.x;
    int y = game.player.y;
    
    // Bullet travels in direction
    for (int i = 0; i < 10; i++) {
        x += dirX;
        y += dirY;
        
        // Check boundaries
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
            break;
        }
        
        // Check for zombie hit
        for (auto& zombie : game.zombies) {
            if (zombie.x == x && zombie.y == y) {
                zombie.health--;
                
                if (zombie.health <= 0) {
                    game.message = "Zombie killed! +10 points";
                } else {
                    game.message = "Hit! Zombie health: " + std::to_string(zombie.health);
                }
                
                return; // Bullet stops on hit
            }
        }
    }
}

bool isWalkable(int x, int y) {
    // Check boundaries
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return false;
    }
    
    return true;
}

int main() {
    // Seed random number generator
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Initialize game
    GameState game;
    initGame(game);
    
    // Game loop
    while (!game.gameOver) {
        renderGame(game);
        handleInput(game);
        updateGame(game);
        Sleep(100); // 100ms delay (10 FPS)
    }
    
    // Game over screen
    system("cls");
    std::cout << "\n\n\n";
    std::cout << "  GAME OVER!\n\n";
    std::cout << "  Final Score: " << game.score << "\n";
    std::cout << "  Waves Survived: " << game.wave - 1 << "\n\n";
    std::cout << "  Press any key to exit...\n";
    
    _getch();
    
    return 0;
}