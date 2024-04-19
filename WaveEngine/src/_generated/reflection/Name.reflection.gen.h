#pragma once

#include "Core/Meta/MetaType.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

namespace MetaType {
namespace Register{

using namespace ecs;

namespace generate {
class NameReflection{
public:
    // fields
    static void* get_name(void* instance){ return static_cast<void*>(&(static_cast<NameComponent*>(instance)->name));}
    static void set_name(void* instance, void* value){ static_cast<NameComponent*>(instance)->name = *static_cast<std::string*>(value);}
};
}

void NameReflectionRegister(){
    registerClass<ecs::NameComponent>();
    registerField<ecs::NameComponent, std::string>(
        "name",
        &generate::NameReflection::get_name,
        &generate::NameReflection::set_name
    );
}

}}

