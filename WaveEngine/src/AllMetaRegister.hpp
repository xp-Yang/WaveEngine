#ifndef AllMetaRegister_hpp
#define AllMetaRegister_hpp

#include "Core/Meta/MetaType.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"

#include "_generated/reflection/Transform.reflection.gen.h"
#include "_generated/reflection/Name.reflection.gen.h"

namespace MetaType {
namespace Register{

inline void allMetaRegister()
{
	NameReflectionRegister();
	TransformReflectionRegister();

}

}
}

#endif