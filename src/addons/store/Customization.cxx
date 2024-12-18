module addons.store.Customization;

auto plugins::store::PluginInvocation::operator()(core::store::Store& store) -> void
{
    m_invocation(m_plugin_ref, store);
}
