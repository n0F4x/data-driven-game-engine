module addons.store.Customization;

auto addons::store::InjectionInvocation::operator()(core::store::Store& store) -> void
{
    m_invocation(m_injection_ref, store);
}
