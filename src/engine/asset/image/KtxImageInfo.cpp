#include "KtxImageInfo.hpp"

namespace engine::asset {

auto KtxImageInfo::operator->() noexcept -> ktxTexture*
{
    return m_ktxTexture.get();
}

void KtxImageInfo::Deleter::operator()(ktxTexture* ktxTexture) noexcept
{
    ktxTexture_Destroy(ktxTexture);
}

}
