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

using FieldFunctionTuple = std::tuple<SetFuncion/*fieldValueSetter*/, GetFuncion/*fieldValueGetter*/, GetNameFuncion/*className*/, GetNameFuncion/*fieldName*/, GetNameFuncion/*fieldTypeName*/>;
//using ClassFunctionTuple = std::tuple<GetBaseClassReflectionInstanceListFunc, ConstructorWithJson, WriteJsonByName>;


static std::multimap<std::string, FieldFunctionTuple*> global_field_multi_map;

struct GlobalMetaInfoManager {
public:
    //static void registerToClassMap(const char* name, ClassFunctionTuple* value);
    static void registerToFieldMap(std::string_view name, FieldFunctionTuple* value);
    static void unregisterAll();

    //static std::map<std::string, ClassFunctionTuple*>       class_multi_map;
};


class MetaField;
class MetaObject {
public:
    // TODO
    template<class T>
    static MetaObject MetaObjectOf(T* obj)
    {
        return MetaObject("Transform");
    }

public:
	MetaObject(const std::string& class_name);
    MetaObject& operator=(const MetaObject& rhs);

    const std::string& className() const;

    int fieldCount() const;
    MetaField field(int index) const;
    MetaField field(const std::string& name) const;

    bool isValid() const { return m_is_valid; }

private:
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

}

#endif // !MetaType_hpp
