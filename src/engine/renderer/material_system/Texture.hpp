#pragma once

namespace engine::renderer {

class Texture {
public:
    [[nodiscard]] static auto load() -> Texture;
};

}   // namespace engine::renderer
