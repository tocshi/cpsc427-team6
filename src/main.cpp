
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "turn_order_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;
	TurnOrderSystem turnOrder;

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window);
	world.init(&renderer);

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// simple turn-based implementation
		while (turnOrder.is_player_turn()) {
			// Calculating elapsed times in milliseconds from the previous iteration
			auto now = Clock::now();
			float elapsed_ms =
				(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
			t = now;
			physics.step(elapsed_ms);
		}

		world.step(0);
		ai.step(0);
		world.handle_collisions();

		renderer.draw();

		// TODO A2: you can implement the debug freeze here but other places are possible too.
	}

	return EXIT_SUCCESS;
}
