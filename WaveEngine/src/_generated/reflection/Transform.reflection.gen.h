#pragma once

#include "Core/Meta/MetaType.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

namespace MetaType {

using namespace ecs;

class TransformReflection{
public:
    // fields
    static const char* getFieldName_translation(){ return "translation";}
    static const char* getFieldTypeName_translation(){ return "Vec3";}
    static void set_translation(void* instance, void* value){ static_cast<TransformComponent*>(instance)->translation = *static_cast<Vec3*>(value);}
    static void* get_translation(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->translation));}

    static const char* getFieldName_rotation(){ return "rotation";}
    static const char* getFieldTypeName_rotation(){ return "Vec3";}
    static void set_rotation(void* instance, void* value){ static_cast<TransformComponent*>(instance)->rotation = *static_cast<Vec3*>(value);}
    static void* get_rotation(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->rotation));}

    static const char* getFieldName_scale() { return "scale"; }
    static const char* getFieldTypeName_scale() { return "Vec3"; }
    static void set_scale(void* instance, void* value) { static_cast<TransformComponent*>(instance)->scale = *static_cast<Vec3*>(value); }
    static void* get_scale(void* instance) { return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->scale)); }
};

namespace ReflectionRegister {
void TransformReflectionRegister(){
    FieldFunctionTuple* field_function_tuple_translation = new FieldFunctionTuple(
        &TransformReflection::set_translation,
        &TransformReflection::get_translation,
        &TransformReflection::getFieldName_translation,
        &TransformReflection::getFieldTypeName_translation);
    GlobalMetaInfoManager::registerToFieldMap(traits::className<ecs::TransformComponent>(), field_function_tuple_translation);

    FieldFunctionTuple* field_function_tuple_rotation = new FieldFunctionTuple(
        &TransformReflection::set_rotation,
        &TransformReflection::get_rotation,
        &TransformReflection::getFieldName_rotation,
        &TransformReflection::getFieldTypeName_rotation);
    GlobalMetaInfoManager::registerToFieldMap(traits::className<ecs::TransformComponent>(), field_function_tuple_rotation);

    FieldFunctionTuple* field_function_tuple_scale = new FieldFunctionTuple(
        &TransformReflection::set_scale,
        &TransformReflection::get_scale,
        &TransformReflection::getFieldName_scale,
        &TransformReflection::getFieldTypeName_scale);
    GlobalMetaInfoManager::registerToFieldMap(traits::className<ecs::TransformComponent>(), field_function_tuple_scale);
}
}

}

