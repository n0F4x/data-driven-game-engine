namespace engine::renderer {

auto SceneManager::emplace_scene(size_t t_id, auto&&... t_args) -> Scene&
{
    for (size_t i{}; i < m_ids.size(); i++) {
        if (m_ids[i] == t_id) {
            return m_scenes[i];
        }
    }

    return m_scenes.emplace_back(std::forward<decltype(t_args)>(t_args)...).second;
}

}   // namespace engine::renderer
