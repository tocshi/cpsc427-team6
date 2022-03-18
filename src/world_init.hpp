#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "tilemap.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float PLAYER_BB_WIDTH = 64.f;
const float PLAYER_BB_HEIGHT = 64.f;
const float ENEMY_BB_WIDTH = 64.f;
const float ENEMY_BB_HEIGHT = 64.f;
const float PLANT_PROJECTILE_BB_WIDTH = 16.f;
const float PLANT_PROJECTILE_BB_HEIGHT = 16.f;
const float BOSS_BB_WIDTH = 0.6f * 150.f;
const float BOSS_BB_HEIGHT = 0.6f * 150.f;
const float ARTIFACT_BB_WIDTH = 0.6f * 150.f;
const float ARTIFACT_BB_HEIGHT = 0.6f * 150.f;
const float CONSUMABLE_BB_WIDTH = 0.6f * 150.f;
const float CONSUMABLE_BB_HEIGHT = 0.6f * 150.f;
const float EQUIPABLE_BB_WIDTH = 0.6f * 150.f;
const float EQUIPABLE_BB_HEIGHT = 0.6f * 150.f;
const float CHEST_BB_WIDTH = 0.6f * 150.f;
const float CHEST_BB_HEIGHT = 0.6f * 150.f;
const float DOOR_BB_WIDTH = 0.6f * 150.f;
const float DOOR_BB_HEIGHT = 0.6f * 150.f;
const float SIGN_BB_WIDTH = 64.f;
const float SIGN_BB_HEIGHT = 64.f;
const float STAIR_BB_WIDTH = 0.6f * 150.f;
const float STAIR_BB_HEIGHT = 0.6f * 150.f;
const float WALL_BB_WIDTH = 100.f;
const float WALL_BB_HEIGHT = 100.f;
const float START_BB_WIDTH = 0.6f * 900.f;
const float START_BB_HEIGHT = 0.6f * 150.f;
const float QUIT_BB_WIDTH = 0.6f * 900.f;
const float QUIT_BB_HEIGHT = 0.6f * 150.f;
const float TITLE_BB_WIDTH = 0.6f * 870.f;
const float TITLE_BB_HEIGHT = 0.6f * 300.f;
const float STAT_BB_WIDTH = 0.6f * 500.f;
const float STAT_BB_HEIGHT = 0.6f * 105.f;
const float ACTIONS_BAR_BB_HEIGHT = 200.f;
const float ACTIONS_BAR_BB_WIDTH = 1800.f;
const float ACTIONS_BUTTON_BB_HEIGHT = 75.f;
const float ACTIONS_BUTTON_BB_WIDTH = 300.f;
const float PAUSE_BUTTON_BB_HEIGHT = 50.f;
const float PAUSE_BUTTON_BB_WIDTH = 50.f;
const float COLLECTION_MENU_BB_WIDTH = 800.f;
const float COLLECTION_MENU_BB_HEIGHT = 700.f;
const float ARTIFACT_CARD_BB_WIDTH = 600.f;
const float ARTIFACT_CARD_BB_HEIGHT = 150.f;
const float ARTIFACT_IMAGE_BB_WIDTH = 80.f;
const float ARTIFACT_IMAGE_BB_HEIGHT = 80.f;
const float DESCRIPTION_DIALOG_BB_WIDTH = 400.f;
const float DESCRIPTION_DIALOG_BB_HEIGHT = 400.f;
const float MODE_BB_WIDTH = 800.f;
const float MODE_BB_HEIGHT = 100.f;
const float POINTER_BB_WIDTH = 64.f;
const float POINTER_BB_HEIGHT = 64.f;
// want to make fog small so it can be rendered a lot TODO: this is a bad implementation and will need to be changed later to use shadows or drawing circles or something better
const float FOG_BB_WIDTH = 0.6f * 100.f;
const float FOG_BB_HEIGHT = 0.6f * 100.f;

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// the background
Entity createBackground(RenderSystem* renderer, vec2 position);
// Player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
// Enemy (split into different enemies for future)
Entity createEnemy(RenderSystem* renderer, vec2 pos);
// Plant Shooter
Entity createPlantShooter(RenderSystem* renderer, vec2 pos);
// Plant Projectile
Entity createPlantProjectile(RenderSystem* renderer, vec2 pos, vec2 dir, Entity owner);
// Caveling
Entity createCaveling(RenderSystem* renderer, vec2 pos);
// Boss
Entity createBoss(RenderSystem* renderer, vec2 pos);
// Artifact
Entity createArtifact(RenderSystem* renderer, vec2 pos);
// Item (consumable)
Entity createConsumable(RenderSystem* renderer, vec2 pos);
// Item (equipable)
Entity createEquipable(RenderSystem* renderer, vec2 pos);
// Chest
Entity createChest(RenderSystem* renderer, vec2 pos);
// Door
Entity createDoor(RenderSystem* renderer, vec2 pos);
// Sign
Entity createSign(RenderSystem* renderer, vec2 pos, std::vector<std::pair<std::string, int>>& messages);
// Stair
Entity createStair(RenderSystem* renderer, vec2 pos);
// Wall
Entity createWall(RenderSystem* renderer, vec2 pos);
// Menu start
Entity createMenuStart(RenderSystem* renderer, vec2 pos);
// Menu quit
Entity createMenuQuit(RenderSystem* renderer, vec2 pos);
// Menu title
Entity createMenuTitle(RenderSystem* renderer, vec2 pos);
// Actions bar
Entity createActionsBar(RenderSystem* renderer, vec2 pos);
// Attack button
Entity createAttackButton(RenderSystem* renderer, vec2 pos);
// Move button
Entity createMoveButton(RenderSystem* renderer, vec2 pos);
// Guard button
Entity createGuardButton(RenderSystem* renderer, vec2 pos, BUTTON_ACTION_ID action, TEXTURE_ASSET_ID texture);
// End turn button
// Item button
Entity createItemButton(RenderSystem* renderer, vec2 pos);
// Back button
Entity createBackButton(RenderSystem* renderer, vec2 pos);
// Cancel button
Entity createCancelButton(RenderSystem* renderer, vec2 pos);
// Pause button
Entity createPauseButton(RenderSystem* renderer, vec2 pos);
// Collection (book) button
Entity createCollectionButton(RenderSystem* renderer, vec2 pos);
// Attack mode text
Entity createAttackModeText(RenderSystem* renderer, vec2 pos);
// Move mode text
Entity createMoveModeText(RenderSystem* renderer, vec2 pos);
// Stylized pointer
Entity createPointer(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture);
// HP Bar 
Entity createHPBar(RenderSystem* renderer, vec2 position);
// MP Bar 
Entity createMPBar(RenderSystem* renderer, vec2 position);
// EP Bar 
Entity createEPBar(RenderSystem* renderer, vec2 position);
// HP Fill 
Entity createHPFill(RenderSystem* renderer, vec2 position);
// MP Fill 
Entity createMPFill(RenderSystem* renderer, vec2 position);
// Ep Fill 
Entity createEPFill(RenderSystem* renderer, vec2 position);
// Fog for fog of war
Entity createFog(vec2 pos, float resolution, float radius, vec2 screenResolution);
// Ep range
Entity createEpRange(vec2 pos, float resolution, float radius, vec2 screenResolution);
// Camera
Entity createCamera(vec2 pos);
// Tiles
SpawnData createTiles(RenderSystem* renderer, const std::string& filepath);
// Text
Entity createText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor);
// Dialog text
Entity createDialogText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor);
// Animated campfire
Entity createCampfire(RenderSystem* renderer, vec2 pos);
// Animated explosion
Entity createExplosion(RenderSystem* renderer, vec2 pos);
// Collection menu
Entity createCollectionMenu(RenderSystem* renderer, vec2 pos);
// Artifact card
Entity createArtifactIcon(RenderSystem* renderer, vec2 pos, ARTIFACT artifact);
// Description Dialog (artifact version) - todo: add more definitions with other items (attacks etc.)
Entity createDescriptionDialog(RenderSystem* renderer, vec2 pos, ARTIFACT artifact);

// cutscene
Entity createCutScene(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID textureID);
