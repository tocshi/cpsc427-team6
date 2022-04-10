#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "tilemap.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float PLAYER_BB_WIDTH = 64.f * ui_scale;
const float PLAYER_BB_HEIGHT = 64.f * ui_scale;
const float ENEMY_BB_WIDTH = 64.f * ui_scale;
const float ENEMY_BB_HEIGHT = 64.f * ui_scale;
const float PLANT_PROJECTILE_BB_WIDTH = 16.f * ui_scale;
const float PLANT_PROJECTILE_BB_HEIGHT = 16.f * ui_scale;
const float BOSS_BB_WIDTH = 0.6f * 150.f * ui_scale;
const float BOSS_BB_HEIGHT = 0.6f * 150.f * ui_scale;
const float PICKUP_BB_WIDTH = 64.f * ui_scale;
const float PICKUP_BB_HEIGHT = 64.f * ui_scale;
const float CHEST_BB_WIDTH = 64.f * ui_scale;
const float CHEST_BB_HEIGHT = 64.f * ui_scale;
const float DOOR_BB_WIDTH = 0.6f * 150.f * ui_scale;
const float DOOR_BB_HEIGHT = 0.6f * 150.f * ui_scale;
const float SIGN_BB_WIDTH = 64.f * ui_scale;
const float SIGN_BB_HEIGHT = 64.f * ui_scale;
const float TEXTBOX_BB_WIDTH = 1200.f * ui_scale;
const float TEXTBOX_BB_HEIGHT = 300.f * ui_scale;
const float STAIR_BB_WIDTH = 0.6f * 150.f * ui_scale;
const float STAIR_BB_HEIGHT = 0.6f * 150.f * ui_scale;
const float WALL_BB_WIDTH = 100.f * ui_scale;
const float WALL_BB_HEIGHT = 100.f * ui_scale;
const float START_BB_WIDTH = 380.f * ui_scale;
const float START_BB_HEIGHT = 98.f * ui_scale;
const float QUIT_BB_WIDTH = 380.f * ui_scale;
const float QUIT_BB_HEIGHT = 98.f * ui_scale;
const float TITLE_BB_WIDTH = 0.6f * 870.f * ui_scale;
const float TITLE_BB_HEIGHT = 0.6f * 300.f * ui_scale;
const float STAT_BB_WIDTH = 0.6f * 500.f * ui_scale;
const float STAT_BB_HEIGHT = 0.6f * 105.f * ui_scale;
const float ACTIONS_BAR_BB_HEIGHT = 200.f * ui_scale;
const float ACTIONS_BAR_BB_WIDTH = 1800.f * ui_scale;
const float ACTIONS_BUTTON_BB_HEIGHT = 125.f * ui_scale;
const float ACTIONS_BUTTON_BB_WIDTH = 125.f * ui_scale;
const float ATTACK_DIALOG_BUTTON_BB_HEIGHT = 49.f * 1 * ui_scale;
const float ATTACK_DIALOG_BUTTON_BB_WIDTH = 190.f * 1 * ui_scale;
const float PAUSE_BUTTON_BB_HEIGHT = 50.f * ui_scale;
const float PAUSE_BUTTON_BB_WIDTH = 50.f * ui_scale;
const float COLLECTION_MENU_BB_WIDTH = 800.f * ui_scale;
const float COLLECTION_MENU_BB_HEIGHT = 700.f * ui_scale;
const float ARTIFACT_CARD_BB_WIDTH = 600.f * ui_scale;
const float ARTIFACT_CARD_BB_HEIGHT = 150.f * ui_scale;
const float ARTIFACT_IMAGE_BB_WIDTH = 64.f * ui_scale;
const float ARTIFACT_IMAGE_BB_HEIGHT = 64.f * ui_scale;
const float DESCRIPTION_DIALOG_BB_WIDTH = 400.f * ui_scale;
const float DESCRIPTION_DIALOG_BB_HEIGHT = 400.f * ui_scale;
const float KEY_ICON_BB_WIDTH = 50.f * ui_scale;
const float KEY_ICON_BB_HEIGHT = 50.f * ui_scale;
const float MODE_BB_WIDTH = 125.f * ui_scale;
const float MODE_BB_HEIGHT = 125.f * ui_scale;
const float POINTER_BB_WIDTH = 64.f * ui_scale;
const float POINTER_BB_HEIGHT = 64.f * ui_scale;
const float SWITCH_BB_WIDTH = 64.f * ui_scale;
const float SWITCH_BB_HEIGHT = 64.f * ui_scale;
const float ENEMY_HP_BAR_OFFSET = -48.f;

// want to make fog small so it can be rendered a lot
const float FOG_BB_WIDTH = 0.6f * 100.f * ui_scale;
const float FOG_BB_HEIGHT = 0.6f * 100.f * ui_scale;

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// the background
Entity createBackground(RenderSystem* renderer, vec2 position);
// the game background
Entity createGameBackground(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture_id, RENDER_LAYER_ID render_id);
// Player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
// Enemy (split into different enemies for future)
Entity createEnemy(RenderSystem* renderer, vec2 pos);
// Plant Shooter
Entity createPlantShooter(RenderSystem* renderer, vec2 pos);
// Projectiles
Entity createProjectile(RenderSystem* renderer, Entity owner, vec2 pos, vec2 scale, float dir, float multiplier, TEXTURE_ASSET_ID texture);
// Traps
Entity createTrap(RenderSystem* renderer, Entity owner, vec2 pos, vec2 scale, float multiplier, int turns, int triggers, TEXTURE_ASSET_ID texture);
// Caveling
Entity createCaveling(RenderSystem* renderer, vec2 pos);
// King Slime
Entity createKingSlime(RenderSystem* renderer, vec2 pos);
// Equipment
Equipment createEquipment(EQUIPMENT type, int tier);
Entity createEquipmentEntity(RenderSystem* renderer, vec2 pos, Equipment equipment);
// Artifact
Entity createArtifact(RenderSystem* renderer, vec2 pos, ARTIFACT type);
// Item (consumable)
Entity createConsumable(RenderSystem* renderer, vec2 pos);
// Chest
Entity createChest(RenderSystem* renderer, vec2 pos, bool isArtifact);
// Door
Entity createDoor(RenderSystem* renderer, vec2 pos, bool boss_door=false);
// Sign
Entity createSign(RenderSystem* renderer, vec2 pos, std::vector<std::pair<std::string, int>>& messages);
// Sign using textboxes
Entity createSign2(RenderSystem* renderer, vec2 pos, std::vector<std::vector<std::string>>& messages);
// Textbox
Entity createTextbox(RenderSystem* renderer, vec2 pos, std::vector<std::vector<std::string>>& messages);
// Stair
Entity createStair(RenderSystem* renderer, vec2 pos);
// Wall
Entity createWall(RenderSystem* renderer, vec2 pos, vec2 scale);
// Menu start
Entity createMenuStart(RenderSystem* renderer, vec2 pos);
// Menu quit
Entity createMenuQuit(RenderSystem* renderer, vec2 pos);
// Menu title
Entity createMenuTitle(RenderSystem* renderer, vec2 pos);
// Hotkey icon
Entity createKeyIcon(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture);
// Actions bar
Entity createActionsBar(RenderSystem* renderer, vec2 pos);
// Attack button
Entity createAttackButton(RenderSystem* renderer, vec2 pos);
// Move button
Entity createMoveButton(RenderSystem* renderer, vec2 pos);
// Guard button
Entity createGuardButton(RenderSystem* renderer, vec2 pos, BUTTON_ACTION_ID action, TEXTURE_ASSET_ID texture);
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
// Attack indicators
Entity createAttackIndicator(RenderSystem* renderer, vec2 position, float x_scale, float y_scale, bool isCircle);
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
// Attack range
Entity createAttackRange(vec2 pos, float resolution, float radius, vec2 screenResolution);
// Camera
Entity createCamera(vec2 pos);
// Tiles
SpawnData createTiles(RenderSystem* renderer, const std::string& filepath);
// Text
Entity createText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor);
// Damage text
Entity createDamageText(RenderSystem* renderer, vec2 pos, std::string text_input , bool is_heal);
// Text (w/ motion)
Entity createMotionText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor);
// Dialog text
Entity createDialogText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor);
// Animated campfire
Entity createCampfire(RenderSystem* renderer, vec2 pos);
// Animated explosion
Entity createExplosion(RenderSystem* renderer, vec2 pos);
// Collection menu
Entity createCollectionMenu(RenderSystem* renderer, vec2 pos, Entity player);
// Artifact card
Entity createArtifactIcon(RenderSystem* renderer, vec2 pos, ARTIFACT artifact);
// Description Dialog (artifact version) - todo: add more definitions with other items (attacks etc.)
Entity createDescriptionDialog(RenderSystem* renderer, vec2 pos, ARTIFACT artifact);
// Attack card
Entity createAttackCard(RenderSystem* renderer, vec2 pos, ATTACK attack);
// Attack type dialog
Entity createAttackDialog(RenderSystem* renderer, vec2 pos, ATTACK attack, bool prepared);
// cutscene
Entity createCutScene(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID textureID);
// Turn UI
Entity createTurnUI(RenderSystem* renderer, vec2 pos);
// Icon
Entity createIcon(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture_id);
// switch
Entity createSwitch(RenderSystem* renderer, vec2 pos);
// generic button
Entity createAttackDialogButton(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID button_texture, BUTTON_ACTION_ID button_action);
// potion
Entity createConsumable(RenderSystem* renderer, vec2 pos, CONSUMABLE type);
// left mouse click animation
Entity createMouseAnimation(RenderSystem* renderer, vec2 pos);
// left mouse click animation
Entity createMouseAnimationUI(RenderSystem* renderer, vec2 pos);
// item card
Entity createItemCard(RenderSystem* renderer, vec2 pos, EQUIPMENT type, Equipment item);
// item menu
Entity createItemMenu(RenderSystem* renderer, vec2 top_card_pos, Inventory inv);
// stats text
Entity createStatsText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor);
// Generate equipment texture for item menu
Entity createItemEquipmentTexture(RenderSystem* renderer, vec2 pos, Equipment equipment);
// objective counter UI
Entity createObjectiveCounter(RenderSystem* renderer, vec2 pos);
// attack animation
Entity createAttackAnimation(RenderSystem* renderer, vec2 pos, ATTACK attack);
// Terminus Veritas effect
Entity createBigSlash(RenderSystem* renderer, vec2 pos, float angle, float scale);
// menu continue
Entity createMenuContinue(RenderSystem* renderer, vec2 pos);
// Save and Quit Button
Entity createSaveQuit(RenderSystem* renderer, vec2 pos);
// Enemy HP bar's backing (black line)
Entity createEnemyHPBacking(vec2 position, Entity parent);
// Enemy HP bar's fill (red line)
Entity createEnemyHPFill(vec2 position, Entity parent);
// shadow for enemies
Entity createShadow(vec2 pos, Entity caster);