module addons.store.Customization;

auto addons::store::PluginInvocation::operator()(core::store::Store& store) -> void
{
    m_invocation(m_plugin_ref, store);
}
