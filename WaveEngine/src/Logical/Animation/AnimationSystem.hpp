#ifndef AnimationSystem_hpp
#define AnimationSystem_hpp

#include "Core/Math/Math.hpp"
#include "Animation.hpp"

class AnimationSystem
{
public:
    AnimationSystem() = default;
    AnimationSystem::AnimationSystem(Animation* Animation)
        : m_CurrentAnimation(Animation)
    {
        m_CurrentTime = 0.0;
        m_FinalBoneMatrices.assign(100, Mat4(1.0f));
    }

    void AnimationSystem::onUpdate(float delta_time = 1.0f/60.0f)
    {
        m_DeltaTime = delta_time;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * delta_time;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), Mat4(1.0f));
        }
    }

    void AnimationSystem::PlayAnimation(Animation* pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }

    void AnimationSystem::CalculateBoneTransform(const AssimpNodeData* node, Mat4 parentTransform)
    {
        std::string nodeName = node->name;
        Mat4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone)
        {
            Bone->onUpdate(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        Mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            Mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    const std::vector<Mat4>& GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

private:
    std::vector<Mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};

#endif // !AnimationSystem_hpp