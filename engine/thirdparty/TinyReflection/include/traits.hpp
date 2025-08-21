#ifndef Traits_hpp
#define Traits_hpp

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <type_traits>
#include <utility>

namespace Meta {

namespace traits {

//template<typename T>
//constexpr auto rawSignature() noexcept {
//#  if defined(__clang__)
//    return std::string_view{ __PRETTY_FUNCTION__ };
//#  elif defined(__GNUC__)
//    return std::string_view{ __PRETTY_FUNCTION__ };
//#  elif defined(_MSC_VER)
//    return std::string_view{ __FUNCSIG__ };
//#  endif
//}
//
//template<typename T>
//constexpr std::string typeName2(T&& obj) noexcept {
//    std::string_view sig = rawSignature<T>();
//    std::string_view sig2 = rawSignature<T&&>();
//    return typeName<T>();
//}
//
//template<typename T>
//constexpr std::string typeName2() noexcept {
//    constexpr std::string_view mark_str = "rawSignature";
//    std::string_view sig = rawSignature<T>();
//#if defined(__clang__)
//    sig = sig.substr(37);
//    std::string name = std::string(sig.substr(0, sig.size() - 1));
//#elif defined(__GNUC__)
//    sig = sig.substr(52);
//    std::string name = std::string(sig.substr(0, sig.size() - 1));
//#elif defined(_MSC_VER)
//    int start_index = sig.find(mark_str) + mark_str.size() + 1;
//    sig = sig.substr(start_index);
//    std::string name = std::string(sig.substr(0, sig.size() - 16));
//#endif
//    //const std::vector<std::string> class_keys = {
//    //"struct",
//    //"class",
//    //"enum",
//    //"union",
//    //};
//    //for (const auto& class_key : class_keys) {
//    //    auto pos = name.find(class_key);
//    //    if (pos != std::string::npos) {
//    //        name = name.substr(pos + class_key.size() + 1);
//    //    }
//    //}
//    return name;
//}

// typeid����ȷ�Ƶ���̬�����ͣ�Ҫ���麯����
template<typename T>
constexpr std::string typeName(T&& obj) noexcept {
    return typeid(std::forward<T>(obj)).name();
}

template<typename T>
constexpr std::string typeName() noexcept {
    return typeid(T).name();
}


// ���������������
template <typename T, typename = void>
struct is_container : std::false_type {};

template <typename T>
struct is_container<T, std::void_t<
    typename T::value_type,        // ������ value_type
    typename T::iterator,          // �����е���������
    typename T::const_iterator,    // �����г�������������
    decltype(std::declval<T>().begin()),  // ������ begin() ����
    decltype(std::declval<T>().end()),    // ������ end() ����
    decltype(std::declval<T>().size())    // Ӧ���� size() ����
    >> : std::true_type{};

// ����Ƿ��ǹ�������
template <typename T, typename = void>
struct is_associative_container : std::false_type {};

template <typename T>
struct is_associative_container<T, std::void_t<
    typename T::key_type,
    typename T::key_compare,
    decltype(std::declval<T>().find(std::declval<typename T::key_type>()))
    >> : std::true_type {};

// ����Ƿ�����������
template <typename T>
struct is_sequence_container : std::conjunction<
    is_container<T>,
    std::negation<is_associative_container<T>>> {};

}

}

#endif // !Meta_hpp
