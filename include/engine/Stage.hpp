#pragma once

#include <vector>
#include <future>

#include <gsl/pointers>

#include <patterns/builder/helper.hpp>

class Controller;


class Stage {
public:
	using System = gsl::not_null<void(*)(Controller&)>;

private:
	class Builder;
	friend BuilderBase<Stage>;

public:
	[[nodiscard]] constexpr static auto create() noexcept;

	void run(Controller& controller) const {
		std::vector<std::future<void>> futures;
		futures.resize(systems.size());

		for (auto& system : systems)
			futures.push_back(std::async(std::launch::async, system, std::ref(controller)));
	}

	[[nodiscard]] constexpr auto has_system() const noexcept {
		return !std::empty(systems);
	}

private:
	[[nodiscard]] constexpr Stage() noexcept = default;

	std::vector<System> systems;
};


class Stage::Builder : public BuilderBase<Stage> {
public:
	using BuilderBase<Stage>::BuilderBase;

	[[nodiscard]] constexpr auto add_system(System&& system) {
		draft().systems.push_back(std::move(system));

		return std::move(*this);
	}
};

constexpr auto Stage::create() noexcept {
	return Builder{};
}
