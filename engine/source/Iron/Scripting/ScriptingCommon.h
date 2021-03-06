#pragma once

namespace ScriptEventTypes
{
    enum ScriptEventType
    {
        OnUpdate            = 1 << 0,
        OnCreate            = 1 << 1,
        OnDestroy           = 1 << 2,
        OnFixedUpdate       = 1 << 3,
        OnLateUpdate        = 1 << 4,
        OnEnabled           = 1 << 5,
        OnDisabled          = 1 << 6,
        OnCollisionEnter    = 1 << 7,
        OnCollisionStay     = 1 << 8,
        OnCollisionExit     = 1 << 9,
        OnMouseOver         = 1 << 10,
        OnMouseEnter        = 1 << 11,
        OnMouseExit         = 1 << 12,
        OnMousePressed      = 1 << 13,
        OnMouseJustPressed  = 1 << 14,
        OnMouseJustReleased = 1 << 15,
    };

    inline ScriptEventType operator|(ScriptEventType a, ScriptEventType b)
    {
        return static_cast<ScriptEventType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline ScriptEventType operator&(ScriptEventType a, ScriptEventType b)
    {
        return static_cast<ScriptEventType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
}

struct ScriptTypeInfo
{
    ScriptEventTypes::ScriptEventType Mask;
    // Here will be placed some info about fields for serialization
};

using ScriptPointer = intptr_t;
