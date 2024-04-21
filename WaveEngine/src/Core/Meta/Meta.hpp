#ifndef Meta_hpp
#define Meta_hpp

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>

namespace Meta {

namespace Register {

template <class ClassType> struct ClassInfo;
template <class ClassType> struct FieldInfo;
template <class ClassType> struct MethodInfo;

template <class ClassType>
inline void registerClass()
{
    MetaObject<ClassType>::registerClass();
}

template <class ClassType, class FieldType>
inline void registerField(FieldType ClassType::*var_ptr, std::string_view field_name)
{
    MetaObject<ClassType>::registerField<FieldType>(var_ptr, field_name);
}

template <class ClassType, class MethodReturnType, typename ... MethodArgs>
inline void registerMethod(MethodReturnType (ClassType::* method_ptr)(MethodArgs...) const, std::string_view method_name)
{
    MetaObject<ClassType>::registerMethod<MethodReturnType, MethodArgs...>(method_ptr, method_name);
}

inline void unregisterAll() {};

template <class ClassType>
struct ClassInfo{
    std::string class_name;
    std::vector<FieldInfo<ClassType>> field_infos;
    std::vector<MethodInfo<ClassType>> method_infos;
    //std::vector<BaseInfo> base_infos;
};

template <class ClassType>
struct FieldInfo {
    void* ClassType::* var;
    std::string field_type_name;
    std::string field_name;
};

template <class ClassType>
struct MethodInfo {
    void (ClassType::* func)();
    std::string return_type_name;
    std::string method_name;
    std::vector<std::string> arg_types;
    std::string signature;
};

//struct BaseInfo {};

}


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

template<class ClassType>
class MetaObject {
public:
    MetaObject() = default;
    MetaObject(const MetaObject& rhs) { m_class_info = rhs.m_class_info; }
    MetaObject& operator=(const MetaObject& rhs) {
        if (this == &rhs)
            return *this;
        m_class_info = rhs.m_class_info;
        return *this;
    }

    const std::string& className() const { return m_class_info.class_name; }

    int fieldCount() const { return m_class_info.field_infos.size(); }
    Register::FieldInfo<ClassType> field(int index) const {
        if (0 <= index && index < m_class_info.field_infos.size())
            return m_class_info.field_infos[index];
        return {};
    }
    Register::FieldInfo<ClassType> field(const std::string& name) const {
        const auto it = std::find_if(m_class_info.field_infos.begin(), m_class_info.field_infos.end(), [&name](const auto& field_info) {
            return field_info.field_name == name;
            });
        if (it != m_class_info.field_infos.end())
            return *it;
        return {};
    }

    int methodCount() const { return m_class_info.method_infos.size(); }
    Register::MethodInfo<ClassType> method(int index) const {
        if (0 <= index && index < m_class_info.method_infos.size())
            return m_class_info.method_infos[index];
        return {};
    }
    Register::MethodInfo<ClassType> method(const std::string& name) const {
        const auto it = std::find_if(m_class_info.method_infos.begin(), m_class_info.method_infos.end(), [&name](const auto& method_info) {
            return method_info.method_name == name;
            });
        if (it != m_class_info.method_infos.end())
            return *it;
        return {};
    }

private:
    template<class T> friend void Register::registerClass();
    template<class T, class I> friend void Register::registerField(I T::* var_ptr, std::string_view field_name);
    template<class T, class I, typename ... Args> friend void Register::registerMethod(I(T::* method_ptr)(Args...) const, std::string_view method_name);

    static void registerClass() {
        m_class_info.class_name = traits::className<ClassType>();
    }

    template<class FieldType>
    static void registerField(FieldType ClassType::* var_ptr, std::string_view field_name) {
        if (!m_class_info.class_name.empty())
        {
            std::string field_type_name_ = traits::className<FieldType>();
            std::string field_name_ = std::string(field_name);
            Register::FieldInfo<ClassType> filed_info = { reinterpret_cast<void* ClassType::*>(var_ptr), field_type_name_, field_name_ };
            m_class_info.field_infos.emplace_back(filed_info);
        }
    }

    template <typename Arg>
    static std::string getArgTypeName() {
        return traits::className<Arg>();
    }

    template <class MethodReturnType, typename ... MethodArgs>
    static void registerMethod(MethodReturnType(ClassType::* method_ptr)(MethodArgs...) const, std::string_view method_name)
    {
        std::string return_type_name = traits::className<MethodReturnType>();
        std::string method_name_ = std::string(method_name);
        auto arg_types = std::initializer_list<std::string>{ getArgTypeName<MethodArgs>() ... };
        std::string method_signature = return_type_name + " " + method_name_ + "(";
        if (arg_types.size() == 0)
            method_signature += std::string(")");
        else {
            for (auto it = arg_types.begin(); it != arg_types.end(); it++) {
                method_signature += *it;
                method_signature += ((it + 1) == arg_types.end()) ? std::string(")") : std::string(", ");
            }
        }
        Register::MethodInfo<ClassType> method_info = { reinterpret_cast<void (ClassType::*)()>(method_ptr), return_type_name, method_name_, arg_types, method_signature };
        m_class_info.method_infos.emplace_back(method_info);
    }

private:
    static inline Register::ClassInfo<ClassType> m_class_info; //
};

template<class ClassType>
class ReflectionInstance {
public:
    ReflectionInstance(ClassType* obj) : m_meta(MetaObject<ClassType>()), m_instance(obj) {}

    const MetaObject<ClassType>& getMetaObject() const { return m_meta; }

    ClassType* getInstance() { return m_instance; }

    const std::string& className() const {
        return m_meta.className();
    }

    int fieldCount() const {
        return m_meta.fieldCount();
    }
    Register::FieldInfo<ClassType> field(int index) const {
        return m_meta.field(index);
    }
    Register::FieldInfo<ClassType> field(const std::string& name) const {
        return m_meta.field(name);
    }
    void* getFieldValue(int index) {
        auto& field = m_meta.field(index);
        return &(m_instance->*(field.var));
    }
    template<class FieldType>
    FieldType* getFieldValue(int index) {
        auto& field = m_meta.field(index);
        return getFieldValue<FieldType>(field.field_name);
    }
    template<class FieldType>
    FieldType* getFieldValue(const std::string& name) {
        auto& field = m_meta.field(name);
        if (field.field_type_name != traits::className<FieldType>())
            return nullptr;
        return &(m_instance->*reinterpret_cast<FieldType ClassType::*>(field.var));
    }
    template<class FieldType>
    void setFieldValue(const std::string& name, FieldType* value) {
        *getFieldValue<FieldType>(name) = *value;
    }

    int methodCount() const {
        return m_meta.methodCount();
    }
    Register::MethodInfo<ClassType> method(int index) const {
        return m_meta.method(index);
    }
    Register::MethodInfo<ClassType> method(const std::string& name) const {
        return m_meta.method(name);
    }
    template<class ReturnType, typename ... Args>
    ReturnType invokeMethod(int index, Args&&... args) {
        auto& method = m_meta.method(index);
        return invokeMethod<ReturnType>(method.method_name, std::forward<Args>(args)...);
    }
    template<class ReturnType, typename ... Args>
    ReturnType invokeMethod(const std::string& name, Args&&... args) {
        auto& method = m_meta.method(name);
        if (method.return_type_name != traits::className<ReturnType>()) {
            assert(false);
            return {};
        }
        return (m_instance->*reinterpret_cast<ReturnType (ClassType::*)(Args ...)>(method.func))(std::forward<Args>(args)...);
    }

private:
    ClassType* m_instance;
    MetaObject<ClassType> m_meta;
};

}

#endif // !Meta_hpp
