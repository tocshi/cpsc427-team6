#pragma once

#include <array>
#include <utility>
#include <map>
#include <iostream>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("dungeonbg.png"),
			textures_path("char/shou.png"),
			textures_path("enemy/slime.png"),
			textures_path("enemy/plant_shooter/plant_shooter.png"),
			textures_path("enemy/plant_shooter/projectile.png"),
			textures_path("enemy/caveling.png"),
			textures_path("boss.png"),
			textures_path("artifact.png"),
			textures_path("consumable.png"),
			textures_path("item/equipment.png"),
			textures_path("chest_item_closed.png"),
			textures_path("chest_item_open.png"),
			textures_path("chest_artifact_closed.png"),
			textures_path("chest_artifact_open.png"),
			textures_path("door.png"),
			textures_path("sign.png"),
			textures_path("sign_glowing.png"),
			textures_path("stair.png"),
			textures_path("start.png"),
			textures_path("quit.png"),
			textures_path("title.png"),
			textures_path("wall.png"),
			textures_path("statbars/hpbar.png"),
			textures_path("statbars/mpbar.png"),
			textures_path("statbars/epbar.png"),
			textures_path("statbars/hpfill.png"),
			textures_path("statbars/mpfill.png"),
			textures_path("statbars/epfill.png"),
			textures_path("move.png"),
			textures_path("attack.png"),
			textures_path("actions_bar.png"),
			textures_path("guard.png"),
			textures_path("item.png"),
			textures_path("end_turn.png"),
			textures_path("back_arrow.png"),
			textures_path("cancel.png"),
			textures_path("attack_mode.png"),
			textures_path("movement_mode.png"),
			textures_path("pause.png"),
			textures_path("book.png"),
			textures_path("roguelikeDungeon_transparent.png"),
			textures_path("campfire.png"),
			textures_path("explosion.png"),
			textures_path("menu/normal_mode_pointer.png"),
			textures_path("menu/attack_mode_sword.png"),
			textures_path("menu/move_mode_arrow.png"),
			textures_path("menu/close_cross.png"),
			textures_path("menu/collection_panel.png"),
			textures_path("menu/description_dialog.png"),
			textures_path("menu/artifact_placeholder.png"),
			textures_path("menu/scroll_arrow.png"),
			textures_path("cutscene1.png"), // add for cutscenes 
			textures_path("cutscene2.png"),
			textures_path("cutscene3.png"),
			textures_path("turn_ui.png"),
			textures_path("switch_default.png"),
			textures_path("switch_active.png"),
    		textures_path("keys/1_key.png"),
			textures_path("keys/2_key.png"),
			textures_path("keys/3_key.png"),
			textures_path("keys/4_key.png"),
			textures_path("keys/5_key.png"),
			textures_path("parallax/cave_0000_front_p.png"),
			textures_path("parallax/cave_0001_mid_p.png"),
			textures_path("parallax/cave_0002_back_p.png"),
			textures_path("parallax/cave_0003_color.png"),
			textures_path("attacks/normal_attack.png"),
			textures_path("attacks/roundslash.png"),
			textures_path("attacks/sapping_strike.png"),
			textures_path("attacks/piercing_thrust.png"),
			textures_path("attacks/parrying_stance.png"),
			textures_path("attacks/disengage.png"),
			textures_path("attacks/terminus_veritas.png"),
			textures_path("use.png"),
			textures_path("prepare.png"),
			textures_path("item/potion_red.png"),
			textures_path("item/potion_blue.png"),
			textures_path("item/potion_yellow.png"),
			textures_path("artifacts/arcane_specs.png"), // artifact sprites
			textures_path("artifacts/art_conserve.png"),
			textures_path("artifacts/blade_polish.png"),
			textures_path("artifacts/blood_ruby.png"),
			textures_path("artifacts/burrbag.png"),
			textures_path("artifacts/chimerarm.png"),
			textures_path("artifacts/discarded_fang.png"),
			textures_path("artifacts/fletching.png"),
			textures_path("artifacts/fungifier.png"),
			textures_path("artifacts/funnel.png"),
			textures_path("artifacts/goliath_belt.png"),
			textures_path("artifacts/guide_healthy.png"),
			textures_path("artifacts/hoplon.png"),
			textures_path("artifacts/lively_bulb.png"),
			textures_path("artifacts/lucky_chip.png"),
			textures_path("artifacts/messenger_cap.png"),
			textures_path("artifacts/rubber_mallet.png"),
			textures_path("artifacts/scout_shoes.png"),
			textures_path("artifacts/smoke_powder.png"),
			textures_path("artifacts/thick_tome.png"),
			textures_path("artifacts/thunder_twig.png"),
			textures_path("artifacts/warm_cloak.png"),
			textures_path("artifacts/windbag.png"),
			textures_path("mouse_icons.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("line"),
		shader_path("textured"),
		shader_path("wind"),
		shader_path("text"),
		shader_path("fog"),
		shader_path("ep_range")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	template <class T>
	void dynamicBindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();

	int initFreeType();

	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

	int findTextureId(const std::string& filename);

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection, Camera& camera);
	void drawText(Entity entity, const mat3& projection, Camera& camera);
	void drawToScreen();
	void updateTileMapCoords(TileUV& tileUV);
	void updateAnimTexCoords(AnimationData& anim);
	void updateSpritesheetTexCoords(Spritesheet& spritesheet);
	bool isOnScreen(Motion& motion, Camera& camera, int window_width, int window_height);

	TileUV prev_tileUV = TileUV();
	AnimationData prev_animdata = AnimationData();
	Spritesheet prev_spritesheet = Spritesheet();

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
