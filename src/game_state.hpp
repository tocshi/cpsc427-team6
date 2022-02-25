#include <iostream>
#include <cstdint>
#include <fstream>
#include<string>


// strong type to prevent any errors
enum class GameStates {
	SPLASH_SCREEN,  //TRUE GO TO MAIN MENU STATE
	INTRO_SCENE, ////TRUE GO TO MAIN MENU STATE

	MAIN_MENU,
	ARCHIVE_MENU,
	OPTIONS_MENU,

	// STARTING GAME
	GAME_START,


	GAME_OVER_MENU,

	PAUSE_MENU ,
	BATTLE_MENU =PAUSE_MENU+1,
	MOVEMENT_MENU,
	ATTCK_MENU,
	ITEM_MENU,

	ENEMY_TURN, 
	CUTSCENE // CURRENTLY NOT IMPLEMENETED

};
