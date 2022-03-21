// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection, Camera& camera)
{
	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	if (camera.active && render_request.used_layer < RENDER_LAYER_ID::UI) {
		transform.translate(-camera.position);
	}
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		// update texture coordinates if necessary
		if (render_request.used_geometry == GEOMETRY_BUFFER_ID::TILEMAP && registry.tileUVs.has(entity)) {
			TileUV& tileUV = registry.tileUVs.get(entity);
			if (tileUV.layer != prev_tileUV.layer || tileUV.tileID != prev_tileUV.tileID) {
				updateTileMapCoords(tileUV);
				prev_tileUV = tileUV;
			}
		}
		else if (render_request.used_geometry == GEOMETRY_BUFFER_ID::ANIMATION && registry.animations.has(entity)) {
			AnimationData& anim = registry.animations.get(entity);
			if (anim.spritesheet_texture != prev_animdata.spritesheet_texture || anim.current_frame != prev_animdata.current_frame) {
				updateAnimTexCoords(anim);
					prev_animdata = anim;
			}
		}
		else if (render_request.used_geometry == GEOMETRY_BUFFER_ID::SPRITESHEET && registry.spritesheets.has(entity)) {
			Spritesheet& spritesheet = registry.spritesheets.get(entity);
			if (spritesheet.texture != prev_spritesheet.texture || spritesheet.index != prev_spritesheet.index) {
				updateSpritesheetTexCoords(spritesheet);
				prev_spritesheet = spritesheet;
			}
		}

		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::LINE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::FOG) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		GLint distance_uloc = glGetUniformLocation(program, "distance");
		GLint resolution_uloc = glGetUniformLocation(program, "resolution");
		GLint screen_resolution_uloc = glGetUniformLocation(program, "screen_resolution");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

		// set distance
		Fog fog = registry.fog.get(entity);
		float dist = (fog.radius / fog.resolution);

		glUniform1f(distance_uloc, dist);
		gl_has_errors();

		// set fog resolution
		glUniform1f(resolution_uloc, fog.resolution);
		gl_has_errors();

		// set sreen resolution
		glUniform2f(screen_resolution_uloc, fog.screen_resolution.x, fog.screen_resolution.y);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::EP) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		GLint distance_uloc = glGetUniformLocation(program, "distance");
		GLint resolution_uloc = glGetUniformLocation(program, "resolution");
		GLint screen_resolution_uloc = glGetUniformLocation(program, "screen_resolution");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

		// set distance
		EpRange ep = registry.epRange.get(entity);
		float dist = (ep.radius / ep.resolution);

		glUniform1f(distance_uloc, dist);
		gl_has_errors();

		// set fog resolution
		glUniform1f(resolution_uloc, ep.resolution);
		gl_has_errors();

		// set sreen resolution
		glUniform2f(screen_resolution_uloc, ep.screen_resolution.x, ep.screen_resolution.y);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawText(Entity entity, const mat3 &projection, Camera& camera)
{
	Text &text = registry.texts.get(entity);

	Transform transform;
	// move text relative to world if text is damageText
	if (registry.damageText.has(entity)) {
		Motion& motion = registry.motions.get(entity);
		transform.translate(-camera.position);
		transform.translate(motion.position);
		transform.scale(motion.scale);
	}

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// // Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);

	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
	gl_has_errors();
	assert(in_texcoord_loc >= 0);

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							sizeof(TexturedVertex), (void *)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(
		in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
		(void *)sizeof(
			vec3)); // note the stride to skip the preceeding vertex position

    // activate corresponding render state
    glActiveTexture(GL_TEXTURE0);
    // glBindVertexArray(VAO);

	std::string msg = text.message;
	float x = text.position[0];
	float y = text.position[1];
	float scale = text.scale;

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = text.textColor;
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = msg.begin(); c != msg.end(); c++)
    {
		// Setting uniform values to the currently bound program
		GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
		glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
		gl_has_errors();

        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - ch.Bearing.y * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // // update VBO for each character
		std::vector<TexturedVertex> textured_vertices(4);
		textured_vertices[0].position = { xpos, ypos + h, 0.f };
		textured_vertices[1].position = { xpos + w, ypos + h, 0.f };
		textured_vertices[2].position = { xpos + w, ypos, 0.f };
		textured_vertices[3].position = { xpos, ypos, 0.f };
		textured_vertices[0].texcoord = { 0.f, 1.f };
		textured_vertices[1].texcoord = { 1.f, 1.f };
		textured_vertices[2].texcoord = { 1.f, 0.f };
		textured_vertices[3].texcoord = { 0.f, 0.f };
		const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
		// update content of VBO memory
		dynamicBindVBOandIBO(GEOMETRY_BUFFER_ID::TEXTQUAD, textured_vertices, textured_indices);
		gl_has_errors();
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // render quad
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
        // // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
    // glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the wind texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WIND]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint wind_program = effects[(GLuint)EFFECT_ASSET_ID::WIND];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(wind_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(wind_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(wind_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 0 , 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix(w, h);
	// Draw all textured meshes that have a position and size component
	registry.renderRequests.sort(
		[](Entity a, Entity b) {
			if (!registry.renderRequests.has(a)) {
				return false;
			}
			if (!registry.renderRequests.has(b)) {
				return true;
			}
			return registry.renderRequests.get(a).used_layer < registry.renderRequests.get(b).used_layer;
		});
	// Get the active camera (assumes only one is active)
	Camera camera;
	for (Entity entity : registry.cameras.entities) {
		Camera camera_to_check = registry.cameras.get(entity);
		if (camera_to_check.active) {
			camera = camera_to_check;
			break;
		}
	}
	for (Entity entity : registry.renderRequests.entities)
	{
		if (registry.texts.has(entity)) {
			drawText(entity, projection_2D, camera);
		}
		else if (!registry.motions.has(entity) || registry.hidden.has(entity))
			continue;
		else if (registry.renderRequests.get(entity).used_layer < RENDER_LAYER_ID::UI &&
			!isOnScreen(registry.motions.get(entity), camera, w, h))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		else
			drawTexturedMesh(entity, projection_2D, camera);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix(int width, int height)
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) width;
	float bottom = (float) height;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

int RenderSystem::findTextureId(const std::string& filename) {
	auto it = std::find(texture_paths.begin(), texture_paths.end(), textures_path(filename));
	// If element was found
	if (it != texture_paths.end())
	{
		return it - texture_paths.begin();
	}
	return -1;
}

void RenderSystem::updateTileMapCoords(TileUV& tileUV) {
	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { tileUV.uv_start.x, tileUV.uv_end.y };
	textured_vertices[1].texcoord = { tileUV.uv_end.x, tileUV.uv_end.y };
	textured_vertices[2].texcoord = { tileUV.uv_end.x, tileUV.uv_start.y };
	textured_vertices[3].texcoord = { tileUV.uv_start.x, tileUV.uv_start.y };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::TILEMAP, textured_vertices, textured_indices);
}

bool RenderSystem::isOnScreen(Motion& motion, Camera& camera, int window_width, int window_height) {
	return !(motion.position.x - camera.position.x + abs(motion.scale.x) / 2 < 0 ||
		motion.position.x - camera.position.x - abs(motion.scale.x) / 2 > window_width ||
		motion.position.y - camera.position.y + abs(motion.scale.y) / 2 < 0 ||
		motion.position.y - camera.position.y - abs(motion.scale.y) / 2 > window_height
		);
}

void RenderSystem::updateAnimTexCoords(AnimationData& anim) {
	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	int index = anim.frame_indices[anim.current_frame];
	float start_x = (anim.frame_size.x * (index % anim.spritesheet_columns)) / anim.spritesheet_width;
	float start_y = (anim.frame_size.y * (index / anim.spritesheet_columns)) / anim.spritesheet_height;
	float end_x = start_x + (anim.frame_size.x / anim.spritesheet_width);
	float end_y = start_y + (anim.frame_size.y / anim.spritesheet_height);

	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { start_x, end_y };
	textured_vertices[1].texcoord = { end_x, end_y };
	textured_vertices[2].texcoord = { end_x, start_y };
	textured_vertices[3].texcoord = { start_x, start_y };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::ANIMATION, textured_vertices, textured_indices);
}

void RenderSystem::updateSpritesheetTexCoords(Spritesheet& spritesheet) {
	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	if (spritesheet.width == 0 || spritesheet.height == 0)
		return;
	float start_x = (spritesheet.frame_size.x * (spritesheet.index % spritesheet.columns)) / spritesheet.width;
	float start_y = (spritesheet.frame_size.y * (spritesheet.index / spritesheet.columns)) / spritesheet.height;
	float end_x = start_x + (spritesheet.frame_size.x / spritesheet.width);
	float end_y = start_y + (spritesheet.frame_size.y / spritesheet.height);

	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { start_x, end_y };
	textured_vertices[1].texcoord = { end_x, end_y };
	textured_vertices[2].texcoord = { end_x, start_y };
	textured_vertices[3].texcoord = { start_x, start_y };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITESHEET, textured_vertices, textured_indices);
}