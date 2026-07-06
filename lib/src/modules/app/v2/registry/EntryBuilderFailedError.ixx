module;

#include <stdexcept>

export module ddge.modules.app.v2.registry.EntryBuilderFailedError;

namespace ddge::app::v2 {

export class EntryBuilderFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace ddge::app::v2
