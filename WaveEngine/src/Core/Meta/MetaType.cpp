#include "MetaType.hpp"

namespace MetaType {

static constexpr char* UNKNOWN_TYPE = "UnknownType";
static constexpr char* UNKNOWN = "Unknown";

void GlobalMetaInfoManager::registerToFieldMap(std::string_view name, FieldFunctionTuple* value)
{
    global_field_multi_map.insert(std::make_pair(name, value));
}

void GlobalMetaInfoManager::unregisterAll()
{
    for (const auto& it : global_field_multi_map)
        delete it.second;
    global_field_multi_map.clear();

    //for (const auto& it : m_class_map)
    //    delete it.second;
    //m_class_map.clear();
}

MetaObject::MetaObject(const std::string& class_name)
    : m_is_valid(false)
    , m_class_name(class_name)
{
    m_fields.clear();
    auto fileds_iter = global_field_multi_map.equal_range(class_name);
    while (fileds_iter.first != fileds_iter.second)
    {
        MetaField f_field(fileds_iter.first->second);
        m_fields.emplace_back(f_field);
        m_is_valid = true;
        ++fileds_iter.first;
    }

}

MetaObject& MetaObject::operator=(const MetaObject& rhs)
{
    if (this == &rhs)
        return *this;

    m_is_valid = rhs.m_is_valid;
    m_class_name = rhs.m_class_name;
    m_fields.clear();
    m_fields = rhs.m_fields;
    return *this;
}

const std::string& MetaObject::className() const
{
    return m_class_name;
}

int MetaObject::fieldCount() const
{
    return m_fields.size();
}

MetaField MetaObject::field(int index) const
{
    if (0 <= index && index < m_fields.size())
        return m_fields[index];
    return MetaField(nullptr);
}

MetaField MetaObject::field(const std::string& name) const
{
    const auto it = std::find_if(m_fields.begin(), m_fields.end(), [&name](const auto& field) {
        return field.name() == name;
        });
    if (it != m_fields.end())
        return *it;
    return MetaField(nullptr);
}

MetaField& MetaField::operator=(const MetaField& rhs)
{
    if (this == &rhs)
        return *this;

    m_functions = rhs.m_functions;
    m_field_name = rhs.m_field_name;
    m_field_type_name = rhs.m_field_type_name;
    return *this;
}

void* MetaField::get(void* instance) const
{
    // todo: should check validation
    return static_cast<void*>((std::get<1>(*m_functions))(instance));
}

void MetaField::set(void* instance, void* value)
{
    // todo: should check validation
    (std::get<0>(*m_functions))(instance, value);
}

const std::string& MetaField::name() const
{
    return m_field_name;
}

const std::string& MetaField::typeName() const
{
    return m_field_type_name;
}

MetaField::MetaField(FieldFunctionTuple* functions)
    : m_functions(functions)
{
    m_field_type_name = UNKNOWN_TYPE;
    m_field_name = UNKNOWN;
    if (m_functions == nullptr)
        return;

    m_field_type_name = (std::get<3>(*m_functions))();
    m_field_name = (std::get<2>(*m_functions))();
}

}
