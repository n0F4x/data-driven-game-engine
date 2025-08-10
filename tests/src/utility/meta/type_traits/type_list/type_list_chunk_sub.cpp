#include <type_traits>

import ddge.utility.meta.type_traits.type_list.type_list_chunk_sub;

namespace {

template <typename...>
struct TypeList {};

template <typename...>
struct SubTypeList {};

}   // namespace

static_assert(std::is_same_v<
              ddge::util::meta::type_list_chunk_sub_t<
                  TypeList<
                      int8_t,
                      TypeList<int16_t>,
                      TypeList<int32_t, int64_t>,
                      SubTypeList<uint8_t>,
                      SubTypeList<uint16_t, uint32_t>>,
                  SubTypeList>,
              TypeList<
                  int8_t,
                  TypeList<int16_t>,
                  TypeList<int32_t, int64_t>,
                  SubTypeList<uint8_t>,
                  SubTypeList<uint16_t>,
                  SubTypeList<uint32_t>>>);
