#include "Camera.h"
#include "Screen.h"
#include "../Scene/Transformation.h"
#include "../Scene/SceneHelper.h"

float Camera::GetWidth() const
{
    return _width;
}

void Camera::SetWidth(float width)
{
    SetCameraDirty(true);
    _width = width;
    _height = _width * (float)Screen::GetHeight() / (float)Screen::GetWidth();
}

float Camera::GetHeight() const
{
    return _height;
}

void Camera::SetHeight(float height)
{
    SetCameraDirty(true);
    _height = height;
    _width = _height * (float)Screen::GetWidth() / (float)Screen::GetHeight();
}

float Camera::GetNearClippingPlane() const
{
    return _nearClippingPlane;
}

void Camera::SetNearClippingPlane(float distance)
{
    _nearClippingPlane = distance;
    SetCameraDirty(true);
}

float Camera::GetFarClippingPlane() const
{
    return _farClippingPlane;
}

void Camera::SetFarClippingPlane(float distance)
{
    _farClippingPlane = distance;
    SetCameraDirty(true);
}

CameraResizeModes::CameraResizeMode Camera::GetResizeMode()
{
    return _resizeMode;
}

void Camera::SetResizeMode(CameraResizeModes::CameraResizeMode resizeMode)
{
    if (_resizeMode == resizeMode)
        return;

    _resizeMode = resizeMode;
    UpdateSize();
}

void Camera::UpdateSize()
{
    switch (_resizeMode)
    {
        case CameraResizeModes::KeepHeight:
            _width = _height * (float)Screen::GetWidth() / (float)Screen::GetHeight();
            break;
        case CameraResizeModes::KeepWidth:
            _height = _width * (float)Screen::GetHeight() / (float)Screen::GetWidth();
            break;
        case CameraResizeModes::Stretch:
            // Do nothing
            return;
    }

    SetCameraDirty(true);
}

glm::mat4 Camera::GetViewProjection()
{
    if (IsCameraDirty() || GetComponentS<Transformation>(Owner).DidTransformationChange())
    {
        SetCameraDirty(false);
        glm::mat4 projection = glm::ortho(
                -_width / 2, _width / 2,
                -_height / 2, _height / 2,
                _nearClippingPlane, _farClippingPlane
        );
        glm::mat4 view = GetComponentS<Transformation>(Owner).GetTransformationMatrix();
        view = glm::inverse(view);

        viewProjection = projection * view;
    }

    return viewProjection;
}

void Camera::SetCameraDirty(bool dirty)
{
    dirtyCamera = dirty;
}

bool Camera::IsCameraDirty() const
{
    return dirtyCamera;
}

glm::vec2 Camera::ScreenToWorldPoint(glm::vec2 screenPoint)
{
    auto position = GetComponentS<Transformation>(Owner).GetPosition();
    return glm::vec2(
            _width * (screenPoint.x / float(Screen::GetWidth()) - 0.5) + position.x,
            _height * (screenPoint.y / float(Screen::GetHeight()) - 0.5) + position.y
    );
}

glm::vec2 Camera::WorldToScreenPoint(glm::vec2 worldPoint)
{
    auto position = GetComponentS<Transformation>(Owner).GetPosition();
    return glm::vec2(
            ((worldPoint.x - position.x) / _width + 0.5f) * float(Screen::GetWidth()),
            ((worldPoint.y - position.y) / _height + 0.5f) * float(Screen::GetHeight())
    );
}
