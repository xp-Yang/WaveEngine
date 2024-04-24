namespace Meta {
namespace Serialization {

// implementation of base types

template<>
inline Json Serializer::write(const char& instance)
{
    return Json(instance);
}
template<>
inline char& Serializer::read(const Json& json, char& instance)
{
    assert(json.is_number());
    return instance = json.number_value();
}

template<>
inline Json Serializer::write(const int& instance)
{
    return Json(instance);
}
template<>
inline int& Serializer::read(const Json& json, int& instance)
{
    assert(json.is_number());
    return instance = static_cast<int>(json.number_value());
}

template<>
inline Json Serializer::write(const unsigned int& instance)
{
    return Json(static_cast<int>(instance));
}
template<>
inline unsigned int& Serializer::read(const Json& json, unsigned int& instance)
{
    assert(json.is_number());
    return instance = static_cast<unsigned int>(json.number_value());
}

template<>
inline Json Serializer::write(const float& instance)
{
    return Json(instance);
}
template<>
inline float& Serializer::read(const Json& json, float& instance)
{
    assert(json.is_number());
    return instance = static_cast<float>(json.number_value());
}

template<>
inline Json Serializer::write(const double& instance)
{
    return Json(instance);
}
template<>
inline double& Serializer::read(const Json& json, double& instance)
{
    assert(json.is_number());
    return instance = static_cast<float>(json.number_value());
}

template<>
inline Json Serializer::write(const bool& instance)
{
    return Json(instance);
}
template<>
inline bool& Serializer::read(const Json& json, bool& instance)
{
    assert(json.is_bool());
    return instance = json.bool_value();
}

template<>
inline Json Serializer::write(const std::string& instance)
{
    return Json(instance);
}
template<>
inline std::string& Serializer::read(const Json& json, std::string& instance)
{
    assert(json.is_string());
    return instance = json.string_value();
}

}}