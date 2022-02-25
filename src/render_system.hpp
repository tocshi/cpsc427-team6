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
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::CHICKEN, mesh_path("chicken.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("dungeonbg.png"),
			textures_path("char/shou.png"),
			textures_path("enemy/slime.png"),
			textures_path("boss.png"),
			textures_path("artifact.png"),
			textures_path("consumable.png"),
			textures_path("equipable.png"),
			textures_path("chest.png"),
			textures_path("door.png"),
			textures_path("sign.png"),
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
			textures_path("roguelikeDungeon_transparent.png"),
			textures_path("campfire.png"),
			textures_path("explosion.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("egg"),
		shader_path("chicken"),
		shader_path("textured"),
		shader_path("wind"),
		shader_path("text"),
		shader_path("fog") };

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
	void drawText(Entity entity, const mat3& projection);
	void drawToScreen();
	void updateTileMapCoords(TileUV& tileUV);
	void updateAnimTexCoords(AnimationData& anim);
	bool isOnScreen(Motion& motion, Camera& camera, int window_width, int window_height);

	TileUV prev_tileUV = TileUV();
	AnimationData prev_animdata = AnimationData();

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
