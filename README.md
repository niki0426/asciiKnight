Idea and Concept (Project Concept)
The project represents a combat action game developed in a graphical ASCII mode. 
The player controls a hero (@) who must navigate a closed arena, defeat waves of enemies, and preserve their health points (HP). 
The core gameplay involves surviving progressively difficult encounters and culminates in a final confrontation with a powerful Boss.

Gameplay Mechanics
Hero Actions: 
The player starts in the center of the arena with 5 HP and can move left/right, perform jumps and double jumps, and attack in four directions.
Combat: Every contact with an enemy reduces the hero's health by 1 HP (or more for special enemies), while a successful attack eliminates the enemy.
Wave Progression: 
Enemies appear in waves. After clearing a wave, the number of opponents increases by 2 to 4 for the next stage.
Boss Fight: 
The final wave features a Boss (B) that occupies a 3x3 space, is highly durable, and follows specific behavior patterns like long-range attacks
Controls:
Movement
A: Move Left
D: Move Right
W: Jump / Double Jump

Combat
I: Attack Up
J: Attack Left
K: Attack Down
L: Attack Right

Enemies and Behavior
E (Basic Walker): Walks back and forth on platforms.
J (Jumper): Jumps when in close proximity to the player.
F (Flier): Flies and periodically dives toward the hero.
C (Crawler): Adheres to and moves along walls.
B (Boss): Large 3x3 enemy with multiple health points and unique attack patterns.

Arena Environment
Platforms: Solid elements that the player can stand on but cannot pass through.
Arena Borders (#): Impassable walls that define the limits of the level.

End Conditions
Victory: Occurs when the Boss is defeated, triggering a "YOU WIN" message.
Defeat: Occurs if the hero's HP reaches 0, resulting in a "GAME OVER" message.
