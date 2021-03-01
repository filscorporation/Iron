#pragma once

#include <cstdint>
#include "ScriptingCore.h"

namespace InternalCalls
{
    // Core
    void Application_Quit();
    int Application_GetState();
    MonoString* Application_RuntimePath();
    MonoString* Application_DataPath();
    bool Input_IsKeyPressed(int code);

    // Resources Manager
    uint64_t ResourcesManager_LoadImage(MonoString* path);
    void Sprite_SetAsSpriteSheet(uint64_t spriteID, int tileWidth, int tileHeight);
    int Sprite_GetWidth(uint64_t spriteID);
    int Sprite_GetHeight(uint64_t spriteID);
    uint64_t Animation_FromSpriteSheet(uint64_t spriteID, float length);
    bool Animation_GetLoop(uint64_t animationID);
    void Animation_SetLoop(uint64_t animationID, bool loop);

    // Time
    float Time_GetDeltaTime();
    float Time_GetTimeScale();
    void Time_SetTimeScale(float timeScale);

    // Screen
    int Screen_GetWidth();
    void Screen_SetWidth(int width);
    int Screen_GetHeight();
    void Screen_SetHeight(int height);
    bool Screen_GetFullscreen();
    void Screen_SetFullscreen(bool fullscreen);
    glm::vec4 Screen_GetColor();
    void Screen_SetColor(glm::vec4 color);

    // Log
    void Log_LogInfo(MonoString* message);
    void Log_LogWarning(MonoString* message);
    void Log_LogError(MonoString* message);
}