#pragma once

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <iostream>

#include "Component.h"
#include "Transformation.h"
#include "../Core/Log.h"

class Object
{
public:
    Object();
    ~Object();

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
        newComponent->ParentObject = this;

        Log::LogInfo((std::string)"Component " + typeid(T).name() + " added");

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
        if (HasComponent<T>())
        {
            components.erase(std::type_index(typeid(T)));
            return true;
        }

        return false;
    }
private:
    std::unordered_map<std::type_index, Component*> components;
};