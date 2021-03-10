#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "../EntityComponentSystem/Entity.h"

class Physics
{
public:
    static void Init();
    static void Terminate();
    static void Simulate(float deltaTime);

    static std::vector<EntityID> PointCast(glm::vec2 center);
    static std::vector<EntityID> AABBCast(glm::vec2 center, glm::vec2 size);
};