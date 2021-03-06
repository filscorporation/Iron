#pragma once

#include "../EntityComponentSystem/SparseSet.h"
#include "../Scene/Scene.h"
#include "UIEvent.h"
#include "UIInteractable.h"

class Scene;
class UISystem;

class UILayer
{
public:
    explicit UILayer(Scene* scene);
    ~UILayer();

    void LoadDefaultResources();
    void Update();
    void Draw();
    void PollEvent(UIEvent& uiEvent);

    bool IsPointerOverUI() const;
    static UILayer* Current();
    uint32_t GetLayerThickness() const;
    uint32_t GetCurrentHierarchyIndex() const;
    void IncreaseCurrentHierarchyIndex(uint32_t thickness);
    void SetSortingOrderDirty();
    bool NeedRebuildSortingOrder() const;

    void AddToUpdateQueue(EntityID entityID, UpdateIntaractable callback);
    void RemoveFromUpdateQueue(EntityID entityID);

    EntityID CreateUIElement();
    EntityID CreateUIElement(const char* name, EntityID parent);
    EntityID CreateUIImage();
    EntityID CreateUIImage(Sprite* sprite, const char* name, EntityID parent);
    EntityID CreateUIButton();
    EntityID CreateUIButton(Sprite* sprite, const char* name, EntityID parent);
    EntityID CreateUIText();
    EntityID CreateUIText(const char* text, const char* name, EntityID parent);
    EntityID CreateUIInputField();
    EntityID CreateUIInputField(const char* name, EntityID parent);

    int PixelsPerUnit = 32;

    struct UIResourcesStorage
    {
        Sprite* DefaultInputFieldSprite = nullptr;
        Sprite* DefaultPixelSprite = nullptr;
    };
    UIResourcesStorage UIResources;

private:
    Scene* _scene;
    bool _isPointerOverUI = false;

    uint32_t _layerThickness = 0;
    uint32_t _currentHierarchyIndex = 0;
    bool _rebuildSortingOrder = true;

    struct InteractableCallback
    {
        InteractableCallback(UpdateIntaractable callback, EntityID owner)
        {
            Callback = callback;
            Owner = owner;
        }
        UpdateIntaractable Callback;
        EntityID Owner;
    };
    SparseDataSet<InteractableCallback> _updateQueue;

    UISystem* uiSystem;
};
