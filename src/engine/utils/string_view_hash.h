#pragma once

#include <string_view>

namespace engine::utils {

// 透明哈希函数，支持 std::string_view 查找
struct StringViewHash
{
    using is_transparent = void;

    size_t operator()(std::string_view sv) const noexcept
    {
        std::hash<std::string_view> stringViewHasher;
        return stringViewHasher(sv);
    }
};

} // namespace engine::utils
