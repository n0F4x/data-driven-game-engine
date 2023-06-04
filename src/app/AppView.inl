namespace app {

/////////////////////////////////////////
///-----------------------------------///
///  App::View   IMPLEMENTATION  ///
///-----------------------------------///
/////////////////////////////////////////
template <class AppType>
AppView<AppType>::AppView(App& t_app) : m_app{ t_app } {}

template <class AppType>
auto AppView<AppType>::name() -> std::string_view {
    return m_app.m_name;
}

template <class AppType>
auto AppView<AppType>::renderer() -> App::Renderer& {
    return m_app.m_renderer;
}

}   // namespace app
