

class App;

class Controller final {
public:
	[[nodiscard]] explicit Controller(App& app) noexcept : app{ app } {}

	auto quit() noexcept {
		app.running = false;
	}

	auto transition_to(State::Id to) noexcept {
		if (auto iter{ app.states.find(to) }; iter != app.states.end()) {
			app.currentState->exited();

			app.prevState = app.currentState;
			app.currentState = &iter->second;

			app.currentState->entered();
		}
	}
	auto transition_to_prev() noexcept {
		app.currentState->exited();

		auto& temp = app.prevState;
		app.prevState = app.currentState;
		app.currentState = temp;

		app.currentState->entered();
	}

private:
	App& app;
};
