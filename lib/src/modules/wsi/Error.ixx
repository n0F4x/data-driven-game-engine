module;

#include <stdexcept>

export module ddge.modules.wsi.Error;

namespace ddge::wsi {

export class Error : public std::runtime_error {
public:
    explicit Error(const char* description) : std::runtime_error{ description } {}
};

}   // namespace ddge::wsi
