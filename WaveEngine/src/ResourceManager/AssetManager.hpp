#ifndef AssetManager_hpp
#define AssetManager_hpp

#include "MeshData.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "ObjImporter.hpp"

// 资产类设计原则：
// 1. 方便序列化/反序列化
// 2. 认为是易变的

namespace Asset {

inline constexpr int maxPointLightCount = 8;

class AssetManager {

};

}

#endif // !AssetManager_hpp
