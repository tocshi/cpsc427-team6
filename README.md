Provide a README.md providing entry points to each of the implemented features and
explain them where necessary

Entry Point: Name of file, name of function and line number

Implemented Features:
- Random spawning
    - Player spawns in a random selection of a predetermined location upon game restart
    - Multiple enemies spawn in random selections of predetermined locations upon game restart
    - Spawn campfires in random selections of predetermined locations upon game restart
    <br /> Entry points: 
		- world_system.cpp | spawn_player_random_location() | line 726
		- world_system.cpp | spawn_enemies_random_location() | line 738
		- world_system.cpp | spawn_items_random_location() | line 749

- Collision with walls
    - Entities cannot phase through walls
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> physics_system.cpp | PhysicsSystem::step() | line 41 </li>
	</ul>
    </ul>

- Player Logic
    - Player can select the action type, which affects how a left click is handled
    - A player can attack and move. Players can deal and take damage from enemies.
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | on_mouse() | line 918 </li>
	</ul>
    </ul>

- Physics (Movement)
    - Player moves upon click
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | on_mouse() | line 1176 </li>
	</ul>
    </ul>

- Shaders
    - Player has a limited field of vision, the rest is obscured by a fog
    - Player's movement range is represented by a fading blue circle
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | create_fog_of_war() | line 705 </li>
		<li>shaders/fog.fs.glsl | shader file</li>
		<li>world_system.cpp | create_ep_range() | line 697</li>
		<li>shaders/ep_range.fs.glsl | shader file</li>
	</ul>
    </ul>

- Game state
    - The player and the enemy take actions based on turn
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | doTurnOrderLogic() | line 1428 </li>
	</ul>
    </ul> 

- Slime AI
    - When close to player, will follow player and attack if close enough (aggro state)
    - When far from player, will randomly move (idle state)
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> ai_system.cpp | slime_logic() | line 27 </li>
	</ul>
    </ul> 
    
 - Plant Shooter AI
    - Stationary enemy
    - When player enters its range, it will enter aggro state and shoot projectiles at the player
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> ai_system.cpp | slime_logic() | line 111 </li>
	</ul>
    </ul>

- UI buttons
    - Create buttons for player actions
    - Accept player input and perform actions based on the button
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | on_mouse() | line 918 </li>
		<li>world_init.cpp | createAttackButton() | line 702</li>
		<li>world_init.cpp | createBackButton | line 821</li>
		<li>world_init.cpp | createMoveButton | line 732</li>
		<li>world_init.cpp | createGuardButton | line 762</li>
		<li>world_init.cpp | createItemButton | line 792</li>
		<li>world_init.cpp | createPauseButton | line 934</li>
		<li>world_init.cpp | createCollectionButton | line 962</li>
	</ul>
    </ul>

- Render 
    - Layer variable added to RenderRequest for sorting render order
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> render_system.cpp | RenderSystem::draw() | line 401 </li>
	</ul>
    </ul>

- EP depletion
   - EP Bar Fill (display) world_init.cpp | createEPFill() | line 718
   - check if player in motion and decrement EP world_system.cpp| step() | line 322

- Tilemap parsing/rendering, random map generation
   - Parse Tiled map format and create entities using parsed info
   <ul>
	<li> Entry point: </li>
	 <ul>
		<li> tilemap.cpp | Parse() | line 4 </li>
	</ul>
    </ul>
   
- Camera panning
   <ul>
	<li> Entry point: </li>
	 <ul>
		<li> render_system.cpp | drawTexturedMesh | line 17 </li>
	</ul>
    </ul>

- Spritesheet animation
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | step() | line 491 </li>
	</ul>
    </ul>

- Text rendering
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | logText() | line 1410 </li>
		<li> render_system.cpp | drawText() | line 213 </li>
	</ul>
    </ul>

- Status Effect System 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | StatusType() | line 333 </li>
		<li> combat_system.cpp | apply_status() | line 77 </li>
		<li> ai_system.cpp  | StatusEffect test_poison () | line 41 </li>
	</ul>
    </ul>
    
- Item Menu 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | StatusType() | line 333 </li>
		<li> combat_system.cpp | apply_status() | line 77 </li>
		<li> ai_system.cpp  | StatusEffect test_poison () | line 41 </li>
	</ul>
    </ul>
  
 - Objective Counter
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | StatusType() | line 333 </li>
		<li> combat_system.cpp | apply_status() | line 77 </li>
		<li> ai_system.cpp  | StatusEffect test_poison () | line 41 </li>
	</ul>
    </ul>
    
 - Item/ Artifact sprites
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>
    
 - Floor 0: Tutorial
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul> 
    
 - Healing Solution/ Healing Item Drop
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>
    
 - Damage Text
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul> 
    
 - Chest Mechanics
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>    
    
 - Stat Calculations and Equiptment System
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>    
    
- Attack Menu / Menu Hot Keys
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>   
    
- Move buttons/ Move Actions 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>  
    
- Pass data between Rooms 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>    
    
- Objective System 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | TEXTURE_ASSET_ID | line 32 </li>
	</ul>
    </ul>     
    
- Saving and Loading 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> saveSystem.cpp/ saveSystem.hpp | entire file </li>
		<li> World_Sysem.cpp | TEXTURE_ASSET_ID | line 1980 </li>
		
	</ul>
    </ul>
    
- Cutscene State 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>     
    
- Turn Order UI 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>      
    
- Room System
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>    
    
- Head-Up Text Display
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>    
    
- Collection Menu and New Icons for Modes
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>    
    
- Caveling Enemy
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>      
    
- Knockback Effect
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>   
    
- Queue based on Speed stat
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_System.cpp/ saveSystem.hpp | line 823  </li>
	</ul>
    </ul>        
