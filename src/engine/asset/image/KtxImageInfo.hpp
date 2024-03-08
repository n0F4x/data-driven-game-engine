#pragma once

#include <memory>

#include <ktx.h>

namespace engine::asset {

class KtxImageInfo {
public:
    [[nodiscard]] auto operator->() noexcept -> ktxTexture*;

private:
    struct Deleter {
        void operator()(ktxTexture*) noexcept;
    };
    std::unique_ptr<ktxTexture, Deleter> m_ktxTexture;
};

}   // namespace engine::asset
