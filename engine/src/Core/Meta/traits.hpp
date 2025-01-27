#ifndef Traits_hpp
#define Traits_hpp

#include <vector>
#include <string>

namespace Meta {

namespace traits {

// to get the class name info from the template T
template<typename T>
constexpr auto rawSignature() noexcept {
#  if defined(__clang__)
    return std::string_view{ __PRETTY_FUNCTION__ };
#  elif defined(__GNUC__)
    return std::string_view{ __PRETTY_FUNCTION__ };
#  elif defined(_MSC_VER)
    return std::string_view{ __FUNCSIG__ };
#  endif
}

template<typename T>
constexpr auto rawClassName() noexcept {
    constexpr std::string_view mark_str = "rawSignature";
    constexpr std::string_view sig = rawSignature<T>();
#if defined(__clang__)
    std::string_view prefix_removed = sig.substr(37);
    std::string_view suffix_removed = sig.substr(0, prefix_removed.size() - 1);
    return suffix_removed;
#elif defined(__GNUC__)
    std::string_view prefix_removed = sig.substr(52);
    std::string_view suffix_removed = sig.substr(0, prefix_removed.size() - 1);
    return suffix_removed;
#elif defined(_MSC_VER)
    int start_index = sig.find(mark_str) + mark_str.size() + 1;
    std::string_view prefix_removed = sig.substr(start_index);
    std::string_view suffix_removed = prefix_removed.substr(0, prefix_removed.size() - 16);
    return suffix_removed;
#endif
}

template<typename T>
constexpr std::string className() noexcept {
    const std::vector<std::string> class_keys = {
        "struct ",
        "class ",
        "enum ",
        "union ",
    };
    std::string_view name = rawClassName<T>();
#if defined(_MSC_VER)
    for (const auto& class_key : class_keys) {
        auto pos = name.find(class_key);
        if (pos != std::string::npos) {
            name = name.substr(pos + class_key.size());
        }
    }
#endif
    return std::string(name);
}

inline bool type_is_fundamental(const std::string& name) noexcept {
    if (name == className<bool>() ||
        name == className<int8_t>() ||
        name == className<int16_t>() ||
        name == className<int32_t>() ||
        name == className<int64_t>() ||
        name == className<uint8_t>() ||
        name == className<uint16_t>() ||
        name == className<uint32_t>() ||
        name == className<uint64_t>() ||
        name == className<float>() ||
        name == className<double>() ||
        name == className<std::string>())
        return true;
    return false;
}

}

}

#endif // !Meta_hpp
