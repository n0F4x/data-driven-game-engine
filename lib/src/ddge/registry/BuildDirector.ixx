module;

#include <concepts>
#include <format>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "ddge/util/contract_macros.hpp"

export module ddge.registry.BuildDirector;

import ddge.registry.BuildableEntryBase;
import ddge.registry.BuildableEntryBuilderBase;
import ddge.registry.configuration_entry_c;
import ddge.registry.entry_c;
import ddge.registry.entry_builder_c;
import ddge.registry.EntryBase;
import ddge.registry.EntryBuilderBase;
import ddge.registry.EntryBuilderContainer;
import ddge.registry.EntryInjectionContainer;
import ddge.registry.Registry;
import ddge.registry.represents_entry_builder_dependency_c;
import ddge.registry.represents_entry_dependency_c;
import ddge.util.containers.OptionalRef;
import ddge.util.contracts;
import ddge.util.meta.concepts.functional.function_pointer;
import ddge.util.meta.concepts.specialization_of;
import ddge.util.meta.reflection.hash;
import ddge.util.meta.reflection.name_of;
import ddge.util.meta.type_traits.functional.arguments_of;
import ddge.util.meta.type_traits.functional.result_of;
import ddge.util.TypeList;

namespace ddge::registry {

export template <typename>
class BuildDirector;

class BuildDirectorBase {
public:
    explicit BuildDirectorBase(
        EntryInjectionContainer& injection_container,
        EntryBuilderContainer&   builder_container,
        Registry&                registry
    );

protected:
    [[nodiscard]]
    auto empty() const noexcept -> bool;
    auto reset() noexcept -> void;

    template <auto injection_T>
    auto try_insert_injection() const -> bool;   // NOLINT(*-use-nodiscard)
    template <typename Builder_T>
    auto try_emplace_builder() const -> bool;    // NOLINT(*-use-nodiscard)

    template <typename EntryBuilder_T>
    auto build_builder() const -> void;
    template <typename Entry_T>
    auto build_entry() const -> void;

    template <typename Builder_T>
    auto resolve_build_dependencies() const -> void;
    template <typename Dependency_T>
    auto resolve_dependency() const -> void;

private:
    EntryInjectionContainer* m_injection_container{};
    EntryBuilderContainer*   m_builder_container{};
    Registry*                m_registry{};
};

template <typename EntryBuilder_T, typename FuncPtr_T>
struct IsEntryBuilderMakerFunctionPointer {
    constexpr static bool value{ false };
};

template <typename EntryBuilder_T, typename... Args_T, bool is_noexcept_T>
struct IsEntryBuilderMakerFunctionPointer<
    EntryBuilder_T,
    auto (*)(Args_T...) noexcept(is_noexcept_T)->EntryBuilder_T>   //
{
    constexpr static bool value{ (represents_entry_builder_dependency_c<Args_T> && ...) };
};

template <typename T, typename EntryBuilder_T>
concept entry_builder_maker_function_pointer_c =
    IsEntryBuilderMakerFunctionPointer<EntryBuilder_T, T>::value;

template <entry_builder_c EntryBuilder_T>
class BuildDirector<EntryBuilder_T> : public BuildDirectorBase {
public:
    using BuildDirectorBase::BuildDirectorBase;

    template <typename T>
        requires(!std::is_same_v<T, EntryBuilder_T>)
    explicit BuildDirector(const BuildDirector<T>&);

    template <entry_builder_maker_function_pointer_c<EntryBuilder_T> auto func_T>
        requires(func_T != nullptr)
    auto use_function() -> void;

    template <typename... Dependencies_T>
        requires(represents_entry_builder_dependency_c<Dependencies_T> && ...)
             && std::constructible_from<EntryBuilder_T, Dependencies_T...>
    auto use_dependencies() -> void;

private:
    template <typename... Dependencies_T>
    auto resolve_dependencies(auto (*)(Dependencies_T...)->EntryBuilder_T) const -> void;
};

template <typename T, typename Entry_T>
concept builds_entry_c =
    entry_builder_c<T>
    && std::same_as<util::meta::result_of_t<decltype(&T::build)>, Entry_T>;

template <typename Entry_T, typename FuncPtr_T>
struct IsEntryMakerFunctionPointer {
    constexpr static bool value{ false };
};

template <typename Entry_T, typename... Args_T, bool is_noexcept_T>
struct IsEntryMakerFunctionPointer<
    Entry_T,
    auto (*)(Args_T...) noexcept(is_noexcept_T)->Entry_T>   //
{
    constexpr static bool value{ (represents_entry_dependency_c<Args_T> && ...) };
};

template <typename T, typename Entry_T>
concept entry_maker_function_pointer_c = IsEntryMakerFunctionPointer<Entry_T, T>::value;

template <entry_c Entry_T>
class BuildDirector<Entry_T> : public BuildDirectorBase {
public:
    using BuildDirectorBase::BuildDirectorBase;

    template <typename T>
        requires(!std::is_same_v<T, Entry_T>)
    explicit BuildDirector(const BuildDirector<T>&);

    template <builds_entry_c<Entry_T> Builder_T>
    auto use_builder() -> void;

    template <entry_maker_function_pointer_c<Entry_T> auto func_T>
        requires(func_T != nullptr)
    auto use_function() -> void;

    template <typename... Dependencies_T>
        requires(represents_entry_dependency_c<Dependencies_T> && ...)
             && std::constructible_from<Entry_T, Dependencies_T...>
    auto use_dependencies() -> void;
};

}   // namespace ddge::registry

namespace ddge::registry {

BuildDirectorBase::BuildDirectorBase(
    EntryInjectionContainer& injection_container,
    EntryBuilderContainer&   builder_container,
    Registry&                registry
)
    : m_injection_container{ &injection_container },
      m_builder_container{ &builder_container },
      m_registry{ &registry }
{}

auto BuildDirectorBase::empty() const noexcept -> bool
{
    return m_injection_container == nullptr || m_builder_container == nullptr
        || m_registry == nullptr;
}

auto BuildDirectorBase::reset() noexcept -> void
{
    m_injection_container = nullptr;
    m_builder_container   = nullptr;
    m_registry            = nullptr;
}

template <typename T>
concept represents_optional_dependency_c =
    util::meta::specialization_of_c<T, util::OptionalRef>;

template <auto injection_T>
auto BuildDirectorBase::try_insert_injection() const -> bool
{
    return m_injection_container->try_insert<injection_T>();
}

template <typename Builder_T>
auto BuildDirectorBase::try_emplace_builder() const -> bool
{
    return m_builder_container->try_emplace<Builder_T>();
}

template <typename EntryBuilder_T>
auto BuildDirectorBase::build_builder() const -> void
{
    if constexpr (std::derived_from<EntryBuilder_T, internal::BuildableEntryBuilderBase>)
    {
        BuildDirector<EntryBuilder_T> build_director{
            *m_injection_container,
            *m_builder_container,
            *m_registry,
        };

        describe_build(build_director);
    }
    else {
        m_builder_container->try_emplace<EntryBuilder_T>();
    }
}

template <typename Entry_T>
auto BuildDirectorBase::build_entry() const -> void
{
    PRECOND(m_injection_container != nullptr);
    PRECOND(m_builder_container != nullptr);
    PRECOND(m_registry != nullptr);

    if constexpr (std::derived_from<Entry_T, internal::BuildableEntryBase>) {
        BuildDirector<Entry_T> build_director{
            // ReSharper disable CppDFANullDereference
            *m_injection_container,
            *m_builder_container,
            *m_registry,
            // ReSharper restore CppDFANullDereference
        };

        describe_build(build_director);
    }
    else {
        m_registry->try_emplace<Entry_T>();
    }
}

template <typename Builder_T>
auto BuildDirectorBase::resolve_build_dependencies() const -> void
{
    [&]<typename... Dependencies_T>(util::TypeList<Dependencies_T...>) -> void {
        (resolve_dependency<Dependencies_T>(), ...);
    }(util::meta::arguments_of_t<decltype(&Builder_T::build)>{});
}

template <typename Dependency_T>
auto BuildDirectorBase::resolve_dependency() const -> void
{
    if constexpr (!represents_optional_dependency_c<Dependency_T>) {
        using StrippedDependency = std::remove_cvref_t<Dependency_T>;

        if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>) {
            build_builder<StrippedDependency>();
        }
        else if constexpr (std::derived_from<StrippedDependency, EntryBase>) {
            build_entry<StrippedDependency>();
        }
        else {
            static_assert(false, "invalid dependency");
        }
    }
}

template <entry_builder_c EntryBuilder_T>
template <typename T>
    requires(!std::is_same_v<T, EntryBuilder_T>)
BuildDirector<EntryBuilder_T>::BuildDirector(const BuildDirector<T>& other)
    : BuildDirectorBase{ other }
{}

template <entry_builder_c EntryBuilder_T>
template <entry_builder_maker_function_pointer_c<EntryBuilder_T> auto func_T>
    requires(func_T != nullptr)
auto BuildDirector<EntryBuilder_T>::use_function() -> void
{
    PRECOND(
        !empty(),
        std::format("{} can only be used once", util::meta::name_of<BuildDirector>())
    );

    if (const bool success = try_insert_injection<func_T>(); !success) {
        return;
    }

    /*
     * Dependencies must be resolved after injection to avoid recursion
     *  in case of a cyclic dependency
     */
    resolve_dependencies(func_T);
    resolve_build_dependencies<EntryBuilder_T>();

    reset();
}

template <entry_builder_c EntryBuilder_T>
template <typename... Dependencies_T>
    requires(represents_entry_builder_dependency_c<Dependencies_T> && ...)
         && std::constructible_from<EntryBuilder_T, Dependencies_T...>
auto BuildDirector<EntryBuilder_T>::use_dependencies() -> void
{
    constexpr static auto function{
        +[] [[nodiscard]] (Dependencies_T... dependencies) -> EntryBuilder_T {
            return EntryBuilder_T(std::forward<Dependencies_T>(dependencies)...);   //
        }
    };

    use_function<function>();
}

template <entry_builder_c EntryBuilder_T>
template <typename... Dependencies_T>
auto BuildDirector<EntryBuilder_T>::resolve_dependencies(
    auto (*)(Dependencies_T...)->EntryBuilder_T
) const -> void
{
    (resolve_dependency<Dependencies_T>(), ...);
}

template <entry_c Entry_T>
template <typename T>
    requires(!std::is_same_v<T, Entry_T>)
BuildDirector<Entry_T>::BuildDirector(const BuildDirector<T>& other)
    : BuildDirectorBase{ other }
{}

template <entry_c Entry_T>
template <builds_entry_c<Entry_T> Builder_T>
auto BuildDirector<Entry_T>::use_builder() -> void
{
    PRECOND(
        !empty(),
        std::format("{} can only be used once", util::meta::name_of<BuildDirector>())
    );

    if constexpr (std::derived_from<Builder_T, internal::BuildableEntryBuilderBase>) {
        BuildDirector<Builder_T> build_director{ *this };

        describe_build(build_director);
    }
    else {
        if (const bool success = try_emplace_builder<Builder_T>(); !success) {
            return;
        }

        /*
         * Dependencies must be resolved after builder to avoid recursion
         *  in case of a cyclic dependency
         */
        resolve_build_dependencies<Builder_T>();
    }

    reset();
}

template <auto func_T>
struct DummyBuilder;

template <typename Entry_T, typename... Dependencies_T, auto (*func_T)(Dependencies_T...)->Entry_T>
struct DummyBuilder<func_T> : EntryBuilderBase {
    [[nodiscard]]
    static auto build(Dependencies_T... dependencies) -> Entry_T
    {
        return func_T(std::forward<Dependencies_T>(dependencies)...);
    }
};

template <entry_c Entry_T>
template <entry_maker_function_pointer_c<Entry_T> auto func_T>
    requires(func_T != nullptr)
auto BuildDirector<Entry_T>::use_function() -> void
{
    use_builder<DummyBuilder<func_T>>();
}

template <entry_c Entry_T>
template <typename... Dependencies_T>
    requires(represents_entry_dependency_c<Dependencies_T> && ...)
         && std::constructible_from<Entry_T, Dependencies_T...>
auto BuildDirector<Entry_T>::use_dependencies() -> void
{
    constexpr static auto function{
        +[] [[nodiscard]] (Dependencies_T... dependencies) -> Entry_T
        {
            return Entry_T(std::forward<Dependencies_T>(dependencies)...);   //
        }
    };

    use_function<function>();
}

}   // namespace ddge::registry
