Provide a README.md providing entry points to each of the implemented features and
explain them where necessary

Entry Point: Name of file, name of function and line number

Implemented Features:
- Random spawning
    - Player spawns in a random selection of a predetermined location upon game restart
    - Multiple enemies spawn in random selections of predetermined locations upon game restart
    - Spawn campfires in random selections of predetermined locations upon game restart
    Entry points: 
world_system.cpp | spawn_player_random_location() | line 726
world_system.cpp | spawn_enemies_random_location() | line 738
world_system.cpp | spawn_items_random_location() | line 749

- Collision with walls
    - Entities cannot phase through walls
    Entry point: physics_system.cpp | PhysicsSystem::step() | line 41

- Player Logic
    - Player can select the action type, which affects how a left click is handled
    - A player can attack and move. Players can deal and take damage from enemies.
    Entry point: world_system.cpp | on_mouse() | line 918

- Physics (Movement)
    - Player moves upon click
    Entry point: world_system.cpp | on_mouse() | line 1176

- Shaders
    - Player has a limited field of vision, the rest is obscured by a fog
    - Player's movement range is represented by a fading blue circle
    Entry points: 
    world_system.cpp | create_fog_of_war() | line 705
    shaders/fog.fs.glsl
    world_system.cpp | create_ep_range() | line 697

- Game state
    - The player and the enemy take actions based on turn
    Entry point: world_system.cpp | doTurnOrderLogic() | line 1428

- Slime AI
    - When close to player, will follow player and attack if close enough (aggro state)
    - When far from player, will randomly move (idle state)
    Entry point: ai_system.cpp | slime_logic() | line 27
    
 - Plant Shooter AI
    - Stationary enemy
    - When player enters its range, it will enter aggro state and shoot projectiles at the player
    Entry point: ai_system.cpp | slime_logic() | line 111

- UI buttons
    - Create buttons for player actions
    - Accept player input and perform actions based on the button
    Entry point: world_system.cpp | on_mouse() | line 918
		world_init.cpp | createAttackButton() | line 702
		world_init.cpp | createBackButton | line 821
		world_init.cpp | createMoveButton | line 732
		world_init.cpp | createGuardButton | line 762
		world_init.cpp | createItemButton | line 792
		world_init.cpp | createPauseButton | line 934
		world_init.cpp | createCollectionButton | line 962

- Render 
    - Layer variable added to RenderRequest for sorting render order
    Entry point: render_system.cpp | RenderSystem::draw() | line 401

- EP depletion 
   - EP Bar Fill (display) world_init.cpp | createEPFill() | line 718
   - check if player in motion and decrement EP world_system.cpp| step() | line 322

- Tilemap parsing/rendering, random map generation
   - Parse Tiled map format and create entities using parsed info
   Entry point: tilemap.cpp | Parse() | line 4
   
- Camera panning
   - Entry point: render_system.cpp | drawTexturedMesh | line 17

- Spritesheet animation
   - Entry point: world_system.cpp | step() | line 491

- Text rendering
   - Entry points: 
   world_system.cpp | logText() | line 1410
   render_system.cpp | drawText() | line 213
