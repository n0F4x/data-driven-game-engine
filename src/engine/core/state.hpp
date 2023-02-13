#pragma once

#include <concepts>

namespace engine {

template <class StateType>
concept StateConcept = requires(StateType t) {
                           {
                               t.id()
                           } -> std::convertible_to<std::size_t>;
                           t.enter();
                           t.exit();
                       } && std::destructible<StateType>;

template <class DerivedType>
class StateInterface {
public:
    StateInterface()
        requires(StateConcept<DerivedType>)
    = default;
};

}   // namespace engine
