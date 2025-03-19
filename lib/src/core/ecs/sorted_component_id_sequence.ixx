export module core.ecs:sorted_component_id_sequence;

import utility.meta.type_traits.integer_sequence.integer_sequence_sort;
import utility.ValueSequence;

import :ComponentID;

template <ComponentID::Underlying... component_ids>
using sorted_component_id_sequence_t = util::meta::
    integer_sequence_sort_t<util::ValueSequence<ComponentID::Underlying, component_ids...>>;
