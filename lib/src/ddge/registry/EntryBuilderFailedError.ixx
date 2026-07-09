module;

#include <stdexcept>

export module ddge.registry.EntryBuilderFailedError;

namespace ddge::registry {

export class EntryBuilderFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace ddge::registry
