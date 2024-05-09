#ifndef Meta_hpp
#define Meta_hpp

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <assert.h>
#include <json11/json11.hpp>

#include "traits.hpp"

namespace Meta {

namespace Register {

struct ClassInfo;
struct FieldInfo;
struct MethodInfo;

inline std::unordered_map<std::string, ClassInfo> global_class_info;

template <class ClassType>
inline void registerClass()
{
    std::string class_name = traits::className<ClassType>();
    if (global_class_info.find(class_name) != global_class_info.end()) {
        assert(false);
    }
    global_class_info.insert({ class_name, ClassInfo{class_name, {}, {}} });
}

template <class ClassType, class FieldType>
inline void registerField(FieldType ClassType::*var_ptr, std::string_view field_name)
{
    std::string class_name = traits::className<ClassType>();
    if (global_class_info.find(class_name) != global_class_info.end())
    {
        std::string field_type_name_ = traits::className<FieldType>();
        std::string field_name_ = std::string(field_name);
        size_t var_offset = reinterpret_cast<size_t>(&(reinterpret_cast<ClassType const volatile*>(nullptr)->*var_ptr));
        Register::FieldInfo filed_info = { var_offset, field_type_name_, field_name_ };
        global_class_info[class_name].field_infos.emplace_back(filed_info);
    }
}

template <typename Arg>
inline std::string getArgTypeName() {
    return traits::className<Arg>();
}

template <class ClassType, class MethodReturnType, typename ... MethodArgs>
inline void registerMethod(MethodReturnType (ClassType::* method_ptr)(MethodArgs...) const, std::string_view method_name)
{
    std::string class_name = traits::className<ClassType>();
    if (global_class_info.find(class_name) != global_class_info.end())
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
        Register::MethodInfo method_info = { reinterpret_cast<void (Register::Dumb::*)()>(method_ptr), return_type_name, method_name_, arg_types, method_signature };
        global_class_info[class_name].method_infos.emplace_back(method_info);
    }
}

template <class ClassType>
inline void registerSerializer(ClassType& (*read_func)(const Json&, ClassType*), Json(*write_func)(const ClassType*))
{}

inline void unregisterAll() {};

struct ClassInfo{
    std::string class_name;
    std::vector<FieldInfo> field_infos;
    std::vector<MethodInfo> method_infos;
    //std::vector<BaseInfo> base_infos;
};

struct FieldInfo {
    size_t var_offset;
    std::string field_type_name;
    std::string field_name;
    // TODO bool is_array;
};

struct Dumb {};
struct MethodInfo {
    void (Dumb::* func)();
    std::string return_type_name;
    std::string method_name;
    std::vector<std::string> arg_types;
    std::string signature;
};

//struct BaseInfo {};

}

class MetaObject {
public:
    MetaObject(std::string_view class_name) {
        std::string className = std::string(class_name);
        if (Register::global_class_info.find(className) != Register::global_class_info.end()) {
            m_class_info = Register::global_class_info.at(className);
        }
        else
            assert(false);
    }
    MetaObject(const MetaObject& rhs) = default;

    const std::string& className() const { return m_class_info.class_name; }

    int fieldCount() const { return m_class_info.field_infos.size(); }
    Register::FieldInfo field(int index) const {
        if (0 <= index && index < m_class_info.field_infos.size())
            return m_class_info.field_infos[index];
        return {};
    }
    Register::FieldInfo field(const std::string& name) const {
        const auto it = std::find_if(m_class_info.field_infos.begin(), m_class_info.field_infos.end(), [&name](const auto& field_info) {
            return field_info.field_name == name;
            });
        if (it != m_class_info.field_infos.end())
            return *it;
        return {};
    }

    int methodCount() const { return m_class_info.method_infos.size(); }
    Register::MethodInfo method(int index) const {
        if (0 <= index && index < m_class_info.method_infos.size())
            return m_class_info.method_infos[index];
        return {};
    }
    Register::MethodInfo method(const std::string& name) const {
        const auto it = std::find_if(m_class_info.method_infos.begin(), m_class_info.method_infos.end(), [&name](const auto& method_info) {
            return method_info.method_name == name;
            });
        if (it != m_class_info.method_infos.end())
            return *it;
        return {};
    }

private:
    MetaObject() = delete;

    Register::ClassInfo m_class_info;
};

template <class T>
inline MetaObject MetaObjectOf() { return MetaObject(traits::className<T>()); }

template <class T>
inline MetaObject MetaObjectOf(T* obj) { return MetaObject(traits::className<T>()); }


class WeakReflectionInstance {
public:
    const MetaObject& getMetaObject() const { return m_meta; }

    const std::string& className() const {
        return m_meta.className();
    }

    int fieldCount() const {
        return m_meta.fieldCount();
    }
    Register::FieldInfo field(int index) const {
        return m_meta.field(index);
    }
    Register::FieldInfo field(const std::string& name) const {
        return m_meta.field(name);
    }
    void* getFieldValue(int index) const {
        auto& field = m_meta.field(index);
        return (void*)((char*)m_instance + field.var_offset);
    }
    template<class FieldType>
    FieldType* getFieldValue(int index) const {
        auto& field = m_meta.field(index);
        return getFieldValue<FieldType>(field.field_name);
    }
    template<class FieldType>
    FieldType* getFieldValue(const std::string& name) const {
        auto& field = m_meta.field(name);
        if (field.field_type_name != traits::className<FieldType>())
            return nullptr;
        return (FieldType*)((char*)m_instance + field.var_offset);
    }
    template<class FieldType>
    void setFieldValue(const std::string& name, FieldType* value) const {
        *getFieldValue<FieldType>(name) = *value;
    }

    int methodCount() const {
        return m_meta.methodCount();
    }
    Register::MethodInfo method(int index) const {
        return m_meta.method(index);
    }
    Register::MethodInfo method(const std::string& name) const {
        return m_meta.method(name);
    }

protected:
    WeakReflectionInstance() = delete;
    WeakReflectionInstance(const WeakReflectionInstance& rhs) = default;
    WeakReflectionInstance(const MetaObject& meta, void* instance) : m_meta(meta), m_instance(instance) {}

    void* m_instance;
    MetaObject m_meta;
};

class DynamicReflectionInstance : public WeakReflectionInstance {
public:
    template <class T>
    DynamicReflectionInstance(const T* obj) : WeakReflectionInstance(MetaObjectOf<T>(), (void*)obj) {}
    DynamicReflectionInstance(std::string_view type_name, void* instance) : WeakReflectionInstance(MetaObject(type_name), instance) {}
    DynamicReflectionInstance(const DynamicReflectionInstance& rhs) : WeakReflectionInstance(rhs) {}

    void* getInstance() { return m_instance; }
};

template<class T>
class ReflectionInstance : public WeakReflectionInstance {
public:
    ReflectionInstance(T* obj) : WeakReflectionInstance(MetaObjectOf<T>(), (void*)obj) {}
    ReflectionInstance(const ReflectionInstance& rhs) : WeakReflectionInstance(rhs) {}

    T* getInstance() { return static_cast<T*>(m_instance); }

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
        return (static_cast<T*>(m_instance)->*reinterpret_cast<ReturnType(T::*)(Args ...)>(method.func))(std::forward<Args>(args)...);
    }
};

}

#endif // !Meta_hpp
