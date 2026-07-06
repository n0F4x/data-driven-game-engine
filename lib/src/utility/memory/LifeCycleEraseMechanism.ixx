module;

#include <cstddef>
#include <functional>
#include <memory_resource>
#include <type_traits>

#include "utility/contract_macros.hpp"

export module ddge.utility.memory.LifeCycleEraseMechanism;

import ddge.utility.containers.SmallBuffer;
import ddge.utility.contracts;
import ddge.utility.meta.concepts.nothrow_movable;
import ddge.utility.meta.reflection.hash;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.ScopeFail;

namespace ddge::util {

export template <typename T, std::size_t size_T, std::size_t alignment_T>
concept uses_small_buffer_optimization_c = fits_in_small_buffer_c<T, size_T, alignment_T>
                                        && meta::nothrow_movable_c<T>;

export template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
class LifeCycleEraseMechanism;

namespace internal {

template <std::size_t size_T, std::size_t alignment_T>
using Storage = SmallBuffer<std::max(size_T, sizeof(void*)), alignment_T>;

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
struct EmbeddedVTable {
    using DropFunc = auto(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage<size_T, alignment_T>&      storage
    ) -> void;
    using SwapFunc = auto(
        std::pmr::polymorphic_allocator<>& lhs_allocator,
        Storage<size_T, alignment_T>&      lhs_storage,
        std::pmr::polymorphic_allocator<>& rhs_allocator,
        Storage<size_T, alignment_T>&      rhs_storage,
        const LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>&
            rhs_erase_mechanism
    ) -> void;


    std::reference_wrapper<DropFunc> drop;
    std::reference_wrapper<SwapFunc> swap;
};

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
struct IndirectVTable;

}   // namespace internal

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
class LifeCycleEraseMechanism {
public:
    template <typename T>
    constexpr explicit LifeCycleEraseMechanism(std::type_identity<T>) noexcept;

    [[nodiscard]]
    constexpr auto copy_construct(
        std::pmr::polymorphic_allocator<>&            allocator,
        const internal::Storage<size_T, alignment_T>& source_storage
    ) const -> internal::Storage<size_T, alignment_T>
        requires(!is_move_only_T);
    [[nodiscard]]
    constexpr auto move_construct(
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const noexcept -> internal::Storage<size_T, alignment_T>;
    [[nodiscard]]
    constexpr auto move_construct(
        std::pmr::polymorphic_allocator<>&       destination_allocator,
        const std::pmr::polymorphic_allocator<>& source_allocator,
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const -> internal::Storage<size_T, alignment_T>;
    constexpr auto move_construct_at(
        internal::Storage<size_T, alignment_T>&  destination_storage,
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const noexcept -> void;
    constexpr auto move_construct_at(
        std::pmr::polymorphic_allocator<>&       destination_allocator,
        internal::Storage<size_T, alignment_T>&  destination_storage,
        const std::pmr::polymorphic_allocator<>& source_allocator,
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const -> void;
    constexpr auto drop(
        std::pmr::polymorphic_allocator<>&      allocator,
        internal::Storage<size_T, alignment_T>& storage
    ) const -> void;

    template <typename T, typename... Args_T>
    [[nodiscard]]
    constexpr static auto construct(
        std::allocator_arg_t,
        std::pmr::polymorphic_allocator<>& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    ) -> internal::Storage<size_T, alignment_T>;

    constexpr auto copy_assign(
        std::pmr::polymorphic_allocator<>&            destination_allocator,
        internal::Storage<size_T, alignment_T>&       destination_storage,
        const LifeCycleEraseMechanism&                destination_erase_mechanism,
        const internal::Storage<size_T, alignment_T>& source_storage
    ) const -> void
        requires(!is_move_only_T);

    [[nodiscard]]
    constexpr auto address_of(internal::Storage<size_T, alignment_T>& storage) const
        -> void*;
    [[nodiscard]]
    constexpr auto address_of(const internal::Storage<size_T, alignment_T>& storage) const
        -> const void*;

    [[nodiscard]]
    constexpr auto type_hash() const -> uint64_t;
    [[nodiscard]]
    constexpr auto type_name() const -> std::string_view;
    [[nodiscard]]
    constexpr auto uses_small_buffer() const noexcept -> bool;

    constexpr auto swap(
        std::pmr::polymorphic_allocator<>&      lhs_allocator,
        internal::Storage<size_T, alignment_T>& lhs_storage,
        std::pmr::polymorphic_allocator<>&      rhs_allocator,
        internal::Storage<size_T, alignment_T>& rhs_storage,
        const LifeCycleEraseMechanism&          rhs_erase_mechanism
    ) const -> void;
    constexpr auto reset(
        std::pmr::polymorphic_allocator<>&      allocator,
        internal::Storage<size_T, alignment_T>& storage
    ) const -> void;

private:
    internal::EmbeddedVTable<is_move_only_T, size_T, alignment_T> m_embedded_vtable;
    std::reference_wrapper<
        const internal::IndirectVTable<is_move_only_T, size_T, alignment_T>>
        m_indirect_vtable;
};

}   // namespace ddge::util

namespace ddge::util {

namespace internal {

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
struct IndirectVTable {
    using CopyConstructAtFunc = auto(
        std::pmr::polymorphic_allocator<>&  allocator,
        Storage<size_T, alignment_T>&       destination_storage,
        const Storage<size_T, alignment_T>& source_storage
    ) -> void;
    using MoveConstructAtFunc = auto(
        Storage<size_T, alignment_T>&  destination_storage,
        Storage<size_T, alignment_T>&& source_storage
    ) -> void;
    using MoveConstructAtUsingAllocatorFunc = auto(
        std::pmr::polymorphic_allocator<>&       destination_allocator,
        Storage<size_T, alignment_T>&            destination_storage,
        const std::pmr::polymorphic_allocator<>& source_allocator,
        Storage<size_T, alignment_T>&&           source_storage
    ) -> void;
    using CopyAssignFunc = auto(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage<size_T, alignment_T>&      destination_storage,
        const LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>&
                                            destination_erase_mechanism,
        const Storage<size_T, alignment_T>& source_storage
    ) -> void;
    using AddressOfFunc       = auto(Storage<size_T, alignment_T>&) -> void*;
    using ConstAddressOfFunc  = auto(const Storage<size_T, alignment_T>&) -> const void*;
    using TypeHashFunc        = auto() -> uint64_t;
    using TypeNameFunc        = auto() -> std::string_view;
    using UsesSmallBufferFunc = auto() -> bool;
    using ReleaseFunc         = auto(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage<size_T, alignment_T>&      storage
    ) -> void;


    std::add_pointer_t<CopyConstructAtFunc>     copy_construct_at;
    std::reference_wrapper<MoveConstructAtFunc> move_construct_at;
    std::reference_wrapper<MoveConstructAtUsingAllocatorFunc>
                                                move_construct_at_using_allocator;
    std::add_pointer_t<CopyAssignFunc>          copy_assign;
    std::reference_wrapper<AddressOfFunc>       address_of;
    std::reference_wrapper<ConstAddressOfFunc>  const_address_of;
    std::reference_wrapper<TypeHashFunc>        type_hash;
    std::reference_wrapper<TypeNameFunc>        type_name;
    std::reference_wrapper<UsesSmallBufferFunc> uses_small_buffer;
    std::reference_wrapper<ReleaseFunc>         reset;
};

template <typename T, bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
struct Operations {
    template <typename... Args_T>
    constexpr static auto create_at(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage<size_T, alignment_T>&      storage,
        Args_T&&... args
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            allocator.construct(
                storage.template launder<T>(), std::forward<Args_T>(args)...
            );
        }
        else {
            *storage.template launder<T*>() =
                allocator.new_object<T>(std::forward<Args_T>(args)...);
        }
    }

    constexpr static auto copy_construct_at(
        std::pmr::polymorphic_allocator<>&  allocator,
        Storage<size_T, alignment_T>&       destination_storage,
        const Storage<size_T, alignment_T>& source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            if constexpr (std::is_trivially_copy_constructible_v<T>) {
                destination_storage = source_storage;
            }
            else {
                create_at(
                    allocator, destination_storage, *source_storage.template launder<T>()
                );
            }
        }
        else {
            if (*source_storage.template launder<T*>() == nullptr) {
                *destination_storage.template launder<T*>() = nullptr;
                return;
            }

            create_at(
                allocator, destination_storage, **source_storage.template launder<T*>()
            );
        }
    }

    constexpr static auto move_construct_at(
        Storage<size_T, alignment_T>&  destination_storage,
        Storage<size_T, alignment_T>&& source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            std::construct_at(
                destination_storage.template launder<T>(),
                std::move(*source_storage.template launder<T>())
            );
        }
        else {
            *destination_storage.template launder<T*>() =
                std::exchange(*source_storage.template launder<T*>(), nullptr);
        }
    }

    constexpr static auto move_construct_at_using_allocator(
        std::pmr::polymorphic_allocator<>&       destination_allocator,
        Storage<size_T, alignment_T>&            destination_storage,
        const std::pmr::polymorphic_allocator<>& source_allocator,
        Storage<size_T, alignment_T>&&           source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            if constexpr (std::is_trivially_move_constructible_v<T>) {
                destination_storage = source_storage;
            }
            else {
                destination_allocator.construct(
                    destination_storage.template launder<T>(),
                    std::move(*source_storage.template launder<T>())
                );
            }
        }
        else {
            if constexpr (is_move_only_T) {
                PRECOND(destination_allocator == source_allocator);
                *destination_storage.template launder<T*>() =
                    std::exchange(*source_storage.template launder<T*>(), nullptr);
            }
            else {
                if (destination_allocator == source_allocator) {
                    *destination_storage.template launder<T*>() =
                        std::exchange(*source_storage.template launder<T*>(), nullptr);
                }
                else {
                    copy_construct_at(
                        destination_allocator, destination_storage, source_storage
                    );
                }
            }
        }
    }

    constexpr static auto drop(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage<size_T, alignment_T>&      storage
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            std::destroy_at(storage.template launder<T>());
        }
        else {
            if (*storage.template launder<T*>() != nullptr) {
                allocator.delete_object(*storage.template launder<T*>());
            }
        }
    }

    constexpr static auto copy_assign(
        std::pmr::polymorphic_allocator<>& destination_allocator,
        Storage<size_T, alignment_T>&      destination_storage,
        const LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>&
                                            destination_erase_mechanism,
        const Storage<size_T, alignment_T>& source_storage
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            if (destination_erase_mechanism.type_hash() == type_hash()) {
                if constexpr (std::is_copy_assignable_v<T>) {
                    *destination_storage.template launder<T>() =
                        *source_storage.template launder<T>();
                }
                else {
                    /*
                     * Lambdas are not necessarily assignable ;(
                     */
                    std::destroy_at(destination_storage.template launder<T>());
                    destination_allocator.construct(
                        destination_storage.template launder<T>(),
                        *source_storage.template launder<T>()
                    );
                }
            }
            else {
                T new_object{ *source_storage.template launder<T>() };

                destination_erase_mechanism.drop(
                    destination_allocator, destination_storage
                );

                destination_allocator.construct(
                    destination_storage.template launder<T>(), new_object
                );
            }
        }
        else {
            if (destination_erase_mechanism.type_hash() == type_hash()
                && *destination_storage.template launder<T*>() != nullptr
                && *source_storage.template launder<T*>() != nullptr)
            {
                if constexpr (std::is_copy_assignable_v<T>) {
                    **destination_storage.template launder<T*>() =
                        **source_storage.template launder<T*>();
                }
                else {
                    /*
                     * Lambdas are not necessarily assignable ;(
                     */
                    std::destroy_at(*destination_storage.template launder<T*>());
                    destination_allocator.construct(
                        *destination_storage.template launder<T*>(),
                        **source_storage.template launder<T*>()
                    );
                }
                return;
            }

            T* const new_object = *source_storage.template launder<T*>() == nullptr
                                    ? nullptr
                                    : destination_allocator.new_object<T>(
                                          **source_storage.template launder<T*>()
                                      );
            const ScopeFail new_object_guard{
                [&] noexcept -> void {
                    if (new_object != nullptr) {
                        destination_allocator.delete_object(new_object);
                    }
                },
            };

            destination_erase_mechanism.drop(destination_allocator, destination_storage);

            *destination_storage.template launder<T*>() = new_object;
        }
    }

    [[nodiscard]]
    constexpr static auto address_of(Storage<size_T, alignment_T>& storage) -> void*
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            return static_cast<void*>(storage.template launder<T>());
        }
        else {
            return static_cast<void*>(*storage.template launder<T*>());
        }
    }

    [[nodiscard]]
    constexpr static auto const_address_of(const Storage<size_T, alignment_T>& storage)
        -> const void*
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            return static_cast<const void*>(storage.template launder<T>());
        }
        else {
            return static_cast<const void*>(*storage.template launder<T*>());
        }
    }

    [[nodiscard]]
    constexpr static auto type_hash() noexcept -> uint64_t
    {
        return meta::hash<T>();
    }

    [[nodiscard]]
    constexpr static auto type_name() noexcept -> std::string_view
    {
        return meta::name_of<T>();
    }

    [[nodiscard]]
    constexpr static auto uses_small_buffer() noexcept -> bool
    {
        return uses_small_buffer_optimization_c<T, size_T, alignment_T>;
    }

    constexpr static auto swap(
        std::pmr::polymorphic_allocator<>& lhs_allocator,
        Storage<size_T, alignment_T>&      lhs_storage,
        std::pmr::polymorphic_allocator<>& rhs_allocator,
        Storage<size_T, alignment_T>&      rhs_storage,
        const LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>&
            rhs_erase_mechanism
    ) -> void
    {
        if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            if (type_hash() == rhs_erase_mechanism.type_hash()) {
                if constexpr (std::is_swappable_v<T>) {
                    std::swap(
                        *lhs_storage.template launder<T>(),
                        *rhs_storage.template launder<T>()
                    );
                }
                else {
                    /*
                     * Lambdas are not necessarily swappable ;(
                     */
                    T tmp{ std::move(*lhs_storage.template launder<T>()) };
                    lhs_allocator.destroy(lhs_storage.template launder<T>());
                    lhs_allocator.construct(
                        lhs_storage.template launder<T>(),
                        std::move(*rhs_storage.template launder<T>())
                    );
                    rhs_allocator.destroy(rhs_storage.template launder<T>());
                    rhs_allocator.construct(
                        rhs_storage.template launder<T>(), std::move(tmp)
                    );
                }
            }
            else {
                PRECOND(
                    rhs_erase_mechanism.uses_small_buffer()
                    || lhs_allocator == rhs_allocator
                );
                Storage<size_T, alignment_T> tmp;
                rhs_erase_mechanism.move_construct_at(
                    lhs_allocator, tmp, rhs_allocator, std::move(rhs_storage)
                );
                rhs_erase_mechanism.drop(rhs_allocator, rhs_storage);
                const ScopeFail tmp_guard{ [&] noexcept -> void {
                    rhs_erase_mechanism.drop(lhs_allocator, tmp);
                } };

                std::construct_at(
                    rhs_storage.template launder<T>(),
                    std::move(*lhs_storage.template launder<T>())
                );
                std::destroy_at(lhs_storage.template launder<T>());

                rhs_erase_mechanism.move_construct_at(lhs_storage, std::move(tmp));
                rhs_erase_mechanism.drop(lhs_allocator, tmp);
            }
        }
        else {
            PRECOND(lhs_allocator == rhs_allocator);
            if (type_hash() == rhs_erase_mechanism.type_hash()) {
                std::swap(
                    *lhs_storage.template launder<T*>(),
                    *rhs_storage.template launder<T*>()
                );
            }
            else {
                T* tmp{ *lhs_storage.template launder<T*>() };

                rhs_erase_mechanism.move_construct_at(
                    lhs_allocator, lhs_storage, rhs_allocator, std::move(rhs_storage)
                );
                rhs_erase_mechanism.drop(rhs_allocator, rhs_storage);

                *rhs_storage.template launder<T*>() = tmp;
            }
        }
    }

    constexpr static auto reset(
        std::pmr::polymorphic_allocator<>& allocator,
        Storage<size_T, alignment_T>&      storage
    ) -> void
    {
        /*
         * Small buffer reset is no-op. It will destroy at drop.
         */

        if constexpr (!uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
            if (*storage.template launder<T*>() != nullptr) {
                allocator.delete_object(*storage.template launder<T*>());
                *storage.template launder<T*>() = nullptr;
            }
        }
    }

    [[nodiscard]]
    constexpr static auto make_embedded_vtable() noexcept
        -> EmbeddedVTable<is_move_only_T, size_T, alignment_T>
    {
        return EmbeddedVTable<is_move_only_T, size_T, alignment_T>{
            .drop = drop,
            .swap = swap,
        };
    }

    [[nodiscard]]
    constexpr static auto copy_construct_at_function_address()
    {
        if constexpr (is_move_only_T) {
            return nullptr;
        }
        else {
            return copy_construct_at;
        }
    }

    [[nodiscard]]
    constexpr static auto copy_assign_function_address()
    {
        if constexpr (is_move_only_T) {
            return nullptr;
        }
        else {
            return copy_assign;
        }
    }

    constexpr static IndirectVTable<is_move_only_T, size_T, alignment_T> indirect_vtable{
        .copy_construct_at                 = copy_construct_at_function_address(),
        .move_construct_at                 = move_construct_at,
        .move_construct_at_using_allocator = move_construct_at_using_allocator,
        .copy_assign                       = copy_assign_function_address(),
        .address_of                        = address_of,
        .const_address_of                  = const_address_of,
        .type_hash                         = type_hash,
        .type_name                         = type_name,
        .uses_small_buffer                 = uses_small_buffer,
        .reset                             = reset,
    };
};

}   // namespace internal

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename T>
constexpr LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::
    LifeCycleEraseMechanism(std::type_identity<T>) noexcept
    : m_embedded_vtable{
          internal::Operations<T, is_move_only_T, size_T, alignment_T>::
              make_embedded_vtable(),
      },
      m_indirect_vtable{
          internal::Operations<T, is_move_only_T, size_T, alignment_T>::indirect_vtable
      }
{}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::copy_construct(
        std::pmr::polymorphic_allocator<>&            allocator,
        const internal::Storage<size_T, alignment_T>& source_storage
    ) const -> internal::Storage<size_T, alignment_T>
    requires(!is_move_only_T)
{
    internal::Storage<size_T, alignment_T> result;
    m_indirect_vtable.get().copy_construct_at(allocator, result, source_storage);
    return result;
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::move_construct(
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const noexcept -> internal::Storage<size_T, alignment_T>
{
    internal::Storage<size_T, alignment_T> result;
    move_construct_at(result, std::move(source_storage));
    return result;
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::move_construct(
        std::pmr::polymorphic_allocator<>&       destination_allocator,
        const std::pmr::polymorphic_allocator<>& source_allocator,
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const -> internal::Storage<size_T, alignment_T>
{
    internal::Storage<size_T, alignment_T> result;
    move_construct_at(
        destination_allocator, result, source_allocator, std::move(source_storage)
    );
    return result;
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::move_construct_at(
        internal::Storage<size_T, alignment_T>&  destination_storage,
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const noexcept -> void
{
    m_indirect_vtable.get().move_construct_at(
        destination_storage, std::move(source_storage)
    );
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::move_construct_at(
        std::pmr::polymorphic_allocator<>&       destination_allocator,
        internal::Storage<size_T, alignment_T>&  destination_storage,
        const std::pmr::polymorphic_allocator<>& source_allocator,
        internal::Storage<size_T, alignment_T>&& source_storage
    ) const -> void
{
    m_indirect_vtable.get().move_construct_at_using_allocator(
        destination_allocator,
        destination_storage,
        source_allocator,
        std::move(source_storage)
    );
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::drop(
    std::pmr::polymorphic_allocator<>&      allocator,
    internal::Storage<size_T, alignment_T>& storage
) const -> void
{
    m_embedded_vtable.drop(allocator, storage);
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
template <typename T, typename... Args_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::construct(
    std::allocator_arg_t,
    std::pmr::polymorphic_allocator<>& allocator,
    std::in_place_type_t<T>,
    Args_T&&... args
) -> internal::Storage<size_T, alignment_T>
{
    internal::Storage<size_T, alignment_T> result;
    internal::Operations<T, is_move_only_T, size_T, alignment_T>::create_at(
        allocator, result, std::forward<Args_T>(args)...
    );
    return result;
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::copy_assign(
    std::pmr::polymorphic_allocator<>&            destination_allocator,
    internal::Storage<size_T, alignment_T>&       destination_storage,
    const LifeCycleEraseMechanism&                destination_erase_mechanism,
    const internal::Storage<size_T, alignment_T>& source_storage
) const -> void
    requires(!is_move_only_T)
{
    return m_indirect_vtable.get().copy_assign(
        destination_allocator,
        destination_storage,
        destination_erase_mechanism,
        source_storage
    );
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::address_of(
    internal::Storage<size_T, alignment_T>& storage
) const -> void*
{
    return m_indirect_vtable.get().address_of(storage);
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::address_of(
    const internal::Storage<size_T, alignment_T>& storage
) const -> const void*
{
    return m_indirect_vtable.get().const_address_of(storage);
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::type_hash() const
    -> uint64_t
{
    return m_indirect_vtable.get().type_hash();
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::type_name() const
    -> std::string_view
{
    return m_indirect_vtable.get().type_name();
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::
    uses_small_buffer() const noexcept -> bool
{
    return m_indirect_vtable.get().uses_small_buffer();
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::swap(
    std::pmr::polymorphic_allocator<>&      lhs_allocator,
    internal::Storage<size_T, alignment_T>& lhs_storage,
    std::pmr::polymorphic_allocator<>&      rhs_allocator,
    internal::Storage<size_T, alignment_T>& rhs_storage,
    const LifeCycleEraseMechanism&          rhs_erase_mechanism
) const -> void
{
    m_embedded_vtable.swap(
        lhs_allocator, lhs_storage, rhs_allocator, rhs_storage, rhs_erase_mechanism
    );
}

template <bool is_move_only_T, std::size_t size_T, std::size_t alignment_T>
constexpr auto LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T>::reset(
    std::pmr::polymorphic_allocator<>&      allocator,
    internal::Storage<size_T, alignment_T>& storage
) const -> void
{
    m_indirect_vtable.get().reset(allocator, storage);
}

}   // namespace ddge::util
