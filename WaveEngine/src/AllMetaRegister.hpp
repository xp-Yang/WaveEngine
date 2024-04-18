#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Core/Meta/MetaType.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

#include "_generated/reflection/Transform.reflection.gen.h"

namespace MetaType {

inline void allMetaRegister()
{
	ReflectionRegister::TransformReflectionRegister();

}

}

#endif