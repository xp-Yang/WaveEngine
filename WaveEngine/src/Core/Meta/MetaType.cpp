#include "MetaType.hpp"

namespace MetaType {

MetaObject::MetaObject(const MetaObject& rhs)
{
    m_class_info = rhs.m_class_info;
}

MetaObject& MetaObject::operator=(const MetaObject& rhs)
{
    if (this == &rhs)
        return *this;

    m_class_info = rhs.m_class_info;
    return *this;
}

const std::string& MetaObject::className() const
{
    return m_class_info.class_name;
}

int MetaObject::fieldCount() const
{
    return m_class_info.field_infos.size();
}

Register::FieldInfo MetaObject::field(int index) const
{
    if (0 <= index && index < m_class_info.field_infos.size())
        return m_class_info.field_infos[index];

    return {};
}

Register::FieldInfo MetaObject::field(const std::string& name) const
{
    const auto it = std::find_if(m_class_info.field_infos.begin(), m_class_info.field_infos.end(), [&name](const auto& field_info) {
        return field_info.field_name == name;
        });
    if (it != m_class_info.field_infos.end())
        return *it;

    return {};
}

}
