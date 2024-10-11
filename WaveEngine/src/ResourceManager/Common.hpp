#ifndef AssetManager_hpp
#define AssetManager_hpp

#include "Structure/MeshData.hpp"
#include "Structure/Mesh.hpp"
#include "Structure/Material.hpp"
#include "Structure/Shader.hpp"
#include "Structure/Texture.hpp"

#include "AssetLoader.hpp"

// 资产类设计原则：一个资产类对应一个资产文件
// 1. 方便序列化/反序列化
// 2. 认为是易变的

namespace Asset {

inline constexpr int maxPointLightCount = 8;

class AssetManager {

};

}

#endif // !AssetManager_hpp
