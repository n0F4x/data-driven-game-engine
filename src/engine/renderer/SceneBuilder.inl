#include "engine/renderer/model/ImageLoader.hpp"
#include "engine/renderer/model/ModelLoader.hpp"

namespace engine::renderer {

auto SceneBuilder::load_model(auto&&... t_args) -> tl::optional<entt::id_type>
{
    if (entt::id_type hash{ ModelLoader::hash(std::forward<decltype(t_args)>(t_args)...) };
        m_resource_manager.get().find_model(hash))
    {
        m_cached_models.insert(hash);
        return hash;
    }

    return ModelLoader{ m_resource_manager.get() }
        .load(std::forward<decltype(t_args)>(t_args)...)
        .transform([&](ModelInfo&& t_model_info) {
            entt::id_type hash{ t_model_info.hash };
            m_models.push_back(std::move(t_model_info));
            return hash;
        });
}

}   // namespace engine::renderer
