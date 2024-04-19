#ifndef MetaType_hpp
#define MetaType_hpp

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>

namespace MetaType {

typedef std::function<void(void*, void*)>      SetFuncion;
typedef std::function<void* (void*)>           GetFuncion;
typedef std::function<const char* ()>          GetNameFuncion;

using FieldFunctionTuple = std::tuple<SetFuncion/*fieldValueSetter*/, GetFuncion/*fieldValueGetter*/, GetNameFuncion/*fieldName*/, GetNameFuncion/*fieldTypeName*/>;
//using ClassFunctionTuple = std::tuple<GetBaseClassReflectionInstanceListFunc, ConstructorWithJson, WriteJsonByName>;


static std::multimap<std::string, FieldFunctionTuple*> global_field_multi_map;
//static std::map<std::string, ClassFunctionTuple*>       class_map;

struct GlobalMetaInfoManager {
public:
    //static void registerToClassMap(const char* name, ClassFunctionTuple* value);
    static void registerToFieldMap(std::string_view name, FieldFunctionTuple* value);
    static void unregisterAll();
};


class MetaField;
class MetaObject {
public:
    template<class T>
    static MetaObject MetaObjectOf(T* obj)
    {
        return MetaObject(traits::className<T>());
    }

public:
    MetaObject& operator=(const MetaObject& rhs);

    const std::string& className() const;

    int fieldCount() const;
    MetaField field(int index) const;
    MetaField field(const std::string& name) const;

    bool isValid() const { return m_is_valid; }

private:
    MetaObject(const std::string& class_name);

    bool m_is_valid{ false };
    std::vector<MetaField, std::allocator<MetaField>> m_fields;
    std::string m_class_name;
};

class MetaField
{
public:
    MetaField& operator=(const MetaField& rhs);

    void* get(void* instance) const;
    void  set(void* instance, void* value);

    const std::string& name() const;
    const std::string& typeName() const;

private:
    MetaField(FieldFunctionTuple* functions);

private:
    friend class MetaObject;

    FieldFunctionTuple* m_functions;
    std::string m_field_name;
    std::string m_field_type_name;
};


namespace traits {
// to get the class name info from the template T
template<typename T>
constexpr auto rawClassNameInfo() noexcept {
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
    constexpr auto sig = rawClassNameInfo<T>();
#if defined(__clang__)
    std::string_view prefix_removed = sig.substr(47);
    std::string_view suffix_removed = sig.substr(0, prefix_removed.size() - 1);
    return suffix_removed;
#elif defined(__GNUC__)
    std::string_view prefix_removed = sig.substr(62);
    std::string_view suffix_removed = sig.substr(0, prefix_removed.size() - 1);
    return suffix_removed;
#elif defined(_MSC_VER)
    std::string_view prefix_removed = sig.substr(48);
    std::string_view suffix_removed = prefix_removed.substr(0, prefix_removed.size() - 16);
    return suffix_removed;
#endif
}

template<typename T>
constexpr std::string className() noexcept {
    static std::vector<std::string> class_keys = {
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
}

}

#endif // !MetaType_hpp
