Provide a README.md providing entry points to each of the implemented features and
explain them where necessary

Entry Point: Name of file, name of function and line number

Implemented Features:

- Instanced GL draw calls
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> render_system.cpp | drawInstancedTiles() | line 356 </li>
	</ul>
    </ul>
    
- Dynamic 2D shadows
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_init.cpp | createShadow() | line TO_BE_UPDATED </li>
	</ul>
    </ul>
    
- Enemy HP bar display
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line TO_BE_UPDATED </li>
	</ul>
    </ul>

- Particle Effects
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | setupParticleEmitter() | line TO_BE_UPDATED </li>
	</ul>
    </ul>
 
 - Transitions and hiding load times
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line TO_BE_UPDATED </li>
	</ul>
    </ul>
    
 - Tutorial Rework
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | line TO_BE_UPDATED </li>
	</ul>
    </ul>
    
 - New enemies (Living Rock, Living Pebble, Apparition)
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> ai_system.cpp | line TO_BE_UPDATED </li>
	</ul>
    </ul>
 - Cutscene rework 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> cutscene.cpp | entire file </li>
	</ul>
    </ul>
 - Title rework 
    <ul>
	<li> Entry point: </li>
	 <ul>
		<li> world_system.cpp | loadPlayerCollectionTitleScreen() & update_background_collection()| line tbd </li>
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

Almost every user we saw play our game had confusion with handling doors in our game. Many thought that they could just move to the door and move through it, which is not the case. In almost all cases we had to explain that to go through the door the user had to click on the door. To deal with this, we made it part of the tutorial to explain how to use doors by clicking on them.

“How much health do enemies have?”

Some of the users during our playtest explained that it would be useful to see how much health an enemy had, citing the fact that it could influence if they would want to attack that enemy or avoid it. We thought this made sense, so we added a visible health bar above each enemy which indicates the amount of health the enemy has left.
