#pragma once

#include "Core/Meta/MetaType.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

namespace MetaType {
namespace Register{

using namespace ecs;

namespace generate {
class TransformReflection{
public:
    // fields
    static void set_translation(void* instance, void* value){ static_cast<TransformComponent*>(instance)->translation = *static_cast<Vec3*>(value);}
    static void* get_translation(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->translation));}

    static void set_rotation(void* instance, void* value){ static_cast<TransformComponent*>(instance)->rotation = *static_cast<Vec3*>(value);}
    static void* get_rotation(void* instance){ return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->rotation));}

    static void set_scale(void* instance, void* value) { static_cast<TransformComponent*>(instance)->scale = *static_cast<Vec3*>(value); }
    static void* get_scale(void* instance) { return static_cast<void*>(&(static_cast<TransformComponent*>(instance)->scale)); }
};
}

void TransformReflectionRegister(){
    registerClass<ecs::TransformComponent>();
    registerField<ecs::TransformComponent, Vec3>(
        "translation",
        &generate::TransformReflection::get_translation,
        &generate::TransformReflection::set_translation
    );
    registerField<ecs::TransformComponent, Vec3>(
        "rotation",
        &generate::TransformReflection::get_rotation,
        &generate::TransformReflection::set_rotation
    );
    registerField<ecs::TransformComponent, Vec3>(
        "scale",
        &generate::TransformReflection::get_scale,
        &generate::TransformReflection::set_scale
    );
}

}}

