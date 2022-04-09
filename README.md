Provide a README.md providing entry points to each of the implemented features and
explain them where necessary

Entry Point: Name of file, name of function and line number

Implemented Features:

- Status Effect System 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> combat_System.cpp | line 284 </li>
	</ul>
    </ul>
    
- Item Menu 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_init.cpp |createItemMenu() | line 1186 </li>
	</ul>
    </ul>
  
 - Objective Counter
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line 1898 </li>
	</ul>
    </ul>
    
 - Item/ Artifact sprites
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> components.hpp | line 32, line 190 </li>
		<li> world_init.cpp | line 472 </li>
	</ul>
    </ul>
    
 - Floor 0: Tutorial
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line 947 , line 2567  </li>
	</ul>
    </ul> 
    
 - Healing Solution/ Healing Item Drop
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_init.cpp |  line 506 </li>
	</ul>
    </ul>
    
 - Damage Text
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_init.cpp | line 2123 </li>
	</ul>
    </ul> 
    
 - Chest Mechanics
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line 1735 </li>
	</ul>
    </ul>    
    
 - Stat Calculations and Equiptment System
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> combat_system.cpp | line 370 (stat calc) </li>
		<li> combat_system.cpp | line 403 (equip) </li>
	</ul>
    </ul>    
    
- Attack Menu / Menu Hot Keys
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.hpp | line 2719 </li>
	</ul>
    </ul>   
    
- Move buttons/ Move Actions 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.hpp | line 2719 </li>
	</ul>
    </ul>  
    
- Pass data between Rooms 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> save_system.cpp | entire file </li>
		<li> world_system.cpp | line 1980 </li>
	</ul>
    </ul>    
    
- Objective System 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> room_system.hpp | line 49 </li>
	</ul>
    </ul>     
    
- Saving and Loading 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> saveSystem.cpp/ saveSystem.hpp | entire file </li>
		<li> World_Sysem.cpp | line 2006 </li>
		
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
		<li> world_System.cpp| line 1897  </li>
	</ul>
    </ul>      
    
- Room System
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> room_system.cpp | line 4  </li>
	</ul>
    </ul>    
    
- Head-Up Text Display
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_init.cpp | line 2179  </li>
	</ul>
    </ul>    
    
- Collection Menu and New Icons for Modes
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line 1504  </li>
	</ul>
    </ul>    
    
- Caveling Enemy
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> ai_system.cpp | line 195  </li>
	</ul>
    </ul>      
    
- Knockback Effect
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> physics_System.cpp | line 255  </li>
	</ul>
    </ul>   
    
- Queue based on Speed stat
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> turn_order_system.cpp | line 7, line 18, line 54 </li>
	</ul>
    </ul>   
    
- Parallax background
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line 2876 </li>
	</ul>
    </ul>        

- Saving on Quit
   <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_init.cpp | line 844  </li>
	</ul>
    </ul>        

- Scaling 
   <ul>
	<li> Entry point: </li>
	 <ul>
		<li> common.cpp | line 106 </li>
	</ul>
    </ul>     


Team 6 - User Feedback Report
User Testing Performed:

The main user testing performed on our game was the three crossplay sessions that happened during class time. Multiple groups had the chance to test our game and give us feedback which we took to improve the user experience in our game.

User Feedback & Our Response:

“I think having keyboard hotkeys for move and attack would be convenient”

At first, to do an action in our game like attack, move, end turn, etc. the user would have needed to use the mouse to click on buttons corresponding to those actions. During our first crossplay session many users expressed that they wanted hotkeys, so that they can go into action modes by pressing hotkeys instead of using the mouse. During our next crossplay session, many users told us that the hotkeys worked great, although they were disappointed that we had no hotkey to go back from an action mode. For instance, if you were in attack mode, you would have to use the mouse to click the back button to go back to the main UI. In response to this, we added a hotkey in the escape key that will take you out of an action mode. We also repeated this functionality when the user clicks the right mouse button when in the action mode menu.

“The UI can be a bit confusing“

During our first crossplay session, we received comments on the readability of our UI, specifically how it could be confusing at times. This helped us focus on, for the next two milestones to improve the UI to make it more understandable for the user. One of the main points made was how the action mode buttons were too big and took up too much space at the bottom of the game screen, with this feedback we decided to make the buttons more concise and smaller and moved them to the right side of the screen. Secondly, some users explained how they did not know what the objective was for the room they were in. We agreed that it was confusing, and with that we added a part of the UI at the top left of the screen which informed the user on their room objective(like kill enemies, or click on tiles), and how close they were to reaching that objective for the room. Finally, another point made by users was that they did not find it very user friendly to go back into the attack mode to figure out which attack they had prepared next. This feedback inspired us to show which attack they had prepared in the attack mode button.

Tutorial Improvements

For our last two crossplay sessions, we noticed that users were still confused about how to play the game even after playing through the tutorial we made. This prompted us to revamp our tutorial to help users better understand how to play our game, which will be added to the last milestone. This was done by expanding the tutorial map, adding visual indicators for where to click, and adding a popup box with more in depth instructions on how to play the game.

Global End Turn Button

Many users told us that it was annoying to go and navigate to the end turn button to end their turn, and that it could be easier to just have a hotkey to end your turn no matter which action mode you were in. With that, we made it so that the Enter key is used to end your turn from anywhere in the game.

Attack Range

Most of our feedback for the attack system centred around the fact that users did not know how far they could attack. This led to confusion as to how close the player had to be to be able to attack enemies. To help players visualize the attack range, we added a shader like the ep range shader but it shows the range of the player’s attack.

Confusion with Doors

Almost every user we saw play our game had confusion with handling doors in our game. Many thought that they could just move to the door and move through it, which is not the case. In almost all cases we had to explain that to go through the door the user had to click on the door. Understandably we want the doors to work more intuitively, so we added the ability to just move to the door and be able to go through it automatically.

“How much health do enemies have?”

Some of the users during our playtest explained that it would be useful to see how much health an enemy had, citing the fact that it could influence if they would want to attack that enemy or avoid it. We thought this made sense, so we added a visible health bar above each enemy which indicates the amount of health the enemy has left.
