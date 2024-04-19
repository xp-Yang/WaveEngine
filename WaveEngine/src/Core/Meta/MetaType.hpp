#ifndef MetaType_hpp
#define MetaType_hpp

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>

namespace MetaType {

namespace Register {

typedef std::function<void* (void*)>           GetFunction;
typedef std::function<void(void*, void*)>      SetFunction;
struct ClassInfo;
struct FieldInfo;
struct MethodInfo;
struct BaseInfo;

inline std::unordered_map<std::string, ClassInfo> global_class_infos; // TODO 为什么static有问题

template <class ClassType>
inline void registerClass()
{
    global_class_infos.insert(std::make_pair(traits::className<ClassType>(), ClassInfo{ traits::className<ClassType>(), {} }));
}

template <class ClassType, class FieldType>
inline void registerField(std::string_view field_name)
{
    registerField<ClassType, FieldType>(field_name, nullptr, nullptr);
}

template <class ClassType, class FieldType>
inline void registerField(std::string_view field_name, GetFunction getter, SetFunction setter)
{
    std::string class_name = traits::className<ClassType>();
    if (global_class_infos.find(class_name) != global_class_infos.end())
    {
        std::string field_type_name_ = traits::className<FieldType>();
        std::string field_name_ = std::string(field_name);
        global_class_infos[class_name].class_name = class_name;
        global_class_infos[class_name].field_infos.emplace_back(field_type_name_, field_name_, getter, setter);
    }
}

inline void unregisterAll() {};

struct ClassInfo{
    std::string class_name;
    std::vector<FieldInfo> field_infos;
    //std::vector<MethodInfo> method_infos;
    //std::vector<BaseInfo> base_infos;
};

struct FieldInfo {
    FieldInfo() = default;
    FieldInfo(const std::string& type_name_, const std::string& name_, GetFunction getter_, SetFunction setter_)
        : field_type_name(type_name_)
        , field_name(name_)
        , getter(getter_)
        , setter(setter_)
    {}

    std::string field_type_name;
    std::string field_name;
    GetFunction getter;
    SetFunction setter;
};

struct MethodInfo {};

struct BaseInfo {};

}


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


class MetaObject {
public:
    template<class T>
    static MetaObject MetaObjectOf()
    {
        MetaObject meta_obj;
        std::string class_name = traits::className<T>();
        auto it = Register::global_class_infos.find(class_name);
        auto test = Register::global_class_infos[class_name];
        if (it != Register::global_class_infos.end())
        {
            meta_obj.m_class_info = it->second;
        }
        return meta_obj;
    }

public:
    MetaObject(const MetaObject& rhs);
    MetaObject& operator=(const MetaObject& rhs);

    const std::string& className() const;

    int fieldCount() const;
    Register::FieldInfo field(int index) const;
    Register::FieldInfo field(const std::string& name) const;

private:
    MetaObject() = default;

    Register::ClassInfo m_class_info;
};

template<class T>
class ReflectionInstance {
public:
    ReflectionInstance(T* obj) : m_meta(MetaObject::MetaObjectOf<T>()), m_instance(obj) {}

    const MetaObject& getMetaObject() { return m_meta; }
    T* getInstance() { return m_instance; }

    const std::string& className() const {
        return m_meta.className();
    }

    int fieldCount() const {
        return m_meta.fieldCount();
    }

    void* getField(int index) {
        // todo: should check validation
        return static_cast<void*>(m_meta.field(index).getter(m_instance));
    }

    void* getField(const std::string& name) {
        // todo: should check validation
        return static_cast<void*>(m_meta.field(name).getter(m_instance));
    }

    void setField(const std::string& name, void* value) {
        // todo: should check validation
        m_meta.field(name).setter(m_instance, value);
    }

private:
    T* m_instance;
    MetaObject m_meta;
};

}

#endif // !MetaType_hpp
