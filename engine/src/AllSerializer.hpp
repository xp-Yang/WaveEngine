#ifndef AllSerializer_hpp
#define AllSerializer_hpp

#include "Base/Meta/Serializer.hpp"

namespace Meta {

namespace Serialization {

//// Vec2
//template<>
//inline Json Serializer::write(const Vec2* instance) {
//    Json::object json_obj;
//    json_obj.insert_or_assign("x", Serializer::write(&(instance->x)));
//    json_obj.insert_or_assign("y", Serializer::write(&(instance->y)));
//    return Json(json_obj);
//}
//template<>
//inline Vec2& Serializer::read(const Json& json, Vec2* instance) {
//    assert(json.is_object());
//    Serializer::read(json["x"], &(instance->x));
//    Serializer::read(json["x"], &(instance->x));
//    Serializer::read(json["y"], &(instance->y));
//    return *instance;
//}
//
//// Vec3
//template<>
//inline Json Serializer::write(const Vec3* instance) {
//    Json::object json_obj;
//    json_obj.insert_or_assign("x", Serializer::write(&(instance->x)));
//    json_obj.insert_or_assign("y", Serializer::write(&(instance->y)));
//    json_obj.insert_or_assign("z", Serializer::write(&(instance->z)));
//    return Json(json_obj);
//}
//template<>
//inline Vec3& Serializer::read(const Json& json, Vec3* instance) {
//    assert(json.is_object());
//    Serializer::read(json["x"], &(instance->x));
//    Serializer::read(json["y"], &(instance->y));
//    Serializer::read(json["z"], &(instance->z));
//    return *instance;
//}
//
//// Name
//template<>
//inline Json Serializer::write(const NameComponent* instance) {
//    Json::object json_obj;
//    json_obj.insert_or_assign("name", Serializer::write(&(instance->name)));
//    return Json(json_obj);
//}
//template<>
//inline NameComponent& Serializer::read(const Json& json, NameComponent* instance) {
//    assert(json.is_object());
//    Serializer::read(json["name"], &(instance->name));
//    return *instance;
//}
//
//// Transform
//template<>
//inline Json Serializer::write(const TransformComponent* instance) {
//    Json::object json_obj;
//    json_obj.insert_or_assign("translation", Serializer::write(&(instance->translation)));
//    json_obj.insert_or_assign("rotation", Serializer::write(&(instance->rotation)));
//    json_obj.insert_or_assign("scale", Serializer::write(&(instance->scale)));
//    return Json(json_obj);
//}
//template<>
//inline TransformComponent& Serializer::read(const Json& json, TransformComponent* instance) {
//    assert(json.is_object());
//    Serializer::read(json["translation"], &(instance->translation));
//    Serializer::read(json["rotation"], &(instance->rotation));
//    Serializer::read(json["scale"], &(instance->scale));
//    return *instance;
//}


}

}

#endif