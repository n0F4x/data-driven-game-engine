#include <iostream>

#include <entt/entt.hpp>

#include "engine/App.hpp"

using namespace entt::literals;


void exited() noexcept {
	std::cout << "Exited\n";
}

int main() {
	App::create()
		.add_name("My game engine")
		.add_state(State::create<"MyState"_hs>()
			.on_enter([] { std::cout << "Entered\n"; })
			.on_exit(exited))
		.add_stage(Stage::create()
			.add_system([](Controller& controller) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "Stage 1\n";
	controller.quit(); }))
		.add_stage(Stage::create()
			.add_system([](Controller& controller) { std::cout << "Stage 2\n"; controller.quit(); }))
		.build().run();
}
