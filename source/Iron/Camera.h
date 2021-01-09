#pragma once

#include "Component.h"
#include <GLM/glm.hpp>

class Camera : public Component
{
public:
    float Width = 4.0f;
    float Height = 3.0f;
    float Depth = 10.0f;

    glm::mat4 GetView();
    glm::mat4 GetProjection();
};