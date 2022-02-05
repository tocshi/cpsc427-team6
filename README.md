Provide a README.md providing entry points to each of the implemented features and
explain them where necessary

Entry Point: Name of file, name of function and line number

Implemented Features:
- Random spawning
    - Player spawns in a random location upon game restart
    - Enemy spawns in a random location upon game restart
    Entry points: 
world_system.cpp | spawn_player_random_location() | line 391
world_system.cpp | spawn_enemy_random_location() | line 414

- Collision with walls
    - Entities cannot phase through walls
    Entry point: physics_system.cpp | PhysicsSystem::step() | line 43

- Player Logic (Stats)
    - Player has HP, MP, EP
    Entry point: components.hpp | line 113

- Placeholder Assets
    - Various sprites added for entities
    Entry point: various sprites in data/ directory

- Physics (Movement)
    - Player moves upon click
    Entry point: world_system.cpp | on_mouse() | line 535

- Fog of War
    - Player has a limited field of vision, the rest is obscured by a fog
    Entry point: world_system.cpp | create_fog_of_war() | line 357

- Turn Based Action
    - The player and the enemy moves based on turn
    Entry point: world_system.cpp | set_is_player_turn() | line 539
		world_system.cpp | get_is_player_turn() | line 543
		world_system.cpp | set_is_ai_turn() | line 547
		world_system.cpp | get_is_ai_turn() | line 551

- Slime AI
    - When close to player, will follow player
    - When far from player, will randomly move up/down
    Entry point: ai_system.cpp | slime_logic() | line 24

	- UI buttons
    - Start Game Button
    - Quit Button
    Entry point: world_system.cpp | on_mouse() | line 512
		world_init.cpp | createMenuStart() | line 480
		world_init.cpp | createMenuQuit() | line 512

- Render 
    - Layer variable added to RenderRequest for sorting render order
    Entry point: render_system.cpp | RenderSystem::draw() | line 210
