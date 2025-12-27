module;

#include <stdexcept>

export module ddge.modules.app.PluginSetupFailedError;

namespace ddge::app {

export class PluginSetupFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace ddge::app
