#pragma once

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <iostream>

#include "Component.h"
#include "Transformation.h"

class Entity
{
public:
    Entity();
    ~Entity();

    uint64_t ID;

    const char* Name = "";

    void Destroy();
    void Destroy(float time);
    bool IsDestroyed();

    void OnUpdate();
    void OnFixedUpdate();
    void OnLateUpdate();

    Transformation* Transform;

    std::vector<Component*> Components()
    {
        // TODO: rework
        std::vector<Component*> values;
        values.reserve(components.size());

        for(auto pair : components)
        {
            values.push_back(pair.second);
        }

        return values;
    }

    template<class T>
    T* AddComponent()
    {
        if (HasComponent<T>())
            return (T*)components[std::type_index(typeid(T))];

        if (!std::is_base_of<Component, T>::value)
            // TODO: Error
            return NULL;

        T* newComponentT = new T();
        auto newComponent = (Component*)newComponentT;
        components[std::type_index(typeid(T))] = newComponent;
        newComponent->ParentEntity = this;

        newComponent->OnCreate();

        return newComponentT;
    }

    template<class T>
    bool HasComponent()
    {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    template<class T>
    T* GetComponent()
    {
        if (HasComponent<T>())
            return (T*)components[std::type_index(typeid(T))];

        return NULL;
    }

    template<class T>
    bool RemoveComponent()
    {
        auto component = GetComponent<T>();
        if (component != nullptr)
        {
            components.erase(std::type_index(typeid(T)));
            delete component;
            return true;
        }

        return false;
    }
private:
    std::unordered_map<std::type_index, Component*> components;

    bool isDestroyed = false;
    bool destroyDelayed = false;
    float destroyTimer;
};