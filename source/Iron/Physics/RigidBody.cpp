#include <box2d/box2d.h>

#include "RigidBody.h"
#include "PhysicsCore.h"
#include "BoxCollider.h"
#include "PhysicsInfo.h"

RigidBody::RigidBody(Entity* parentEntity) : Component(parentEntity)
{
    info = new RigidBodyInfo();

    // TODO: set dynamic type on init
}

RigidBody::~RigidBody()
{
    RemovePhysicsBody();
}

void RigidBody::SetDynamic()
{
    b2BodyDef groundBodyDef;
    glm::vec3 position = ParentEntity->Transform->GetPosition();
    groundBodyDef.type = b2_dynamicBody;
    groundBodyDef.position.Set(position.x, position.y);
    groundBodyDef.userData = b2BodyUserData();
    groundBodyDef.userData.pointer = (uintptr_t)ParentEntity;

    info->Body = PhysicsCore::GetWorld()->CreateBody(&groundBodyDef);
    _mass = info->Body->GetMass();
    _type = RigidBodyTypes::Dynamic;

    SetAutoFixture();
}

void RigidBody::SetStatic()
{
    b2BodyDef groundBodyDef;
    glm::vec3 position = ParentEntity->Transform->GetPosition();
    groundBodyDef.type = b2_staticBody;
    groundBodyDef.position.Set(position.x, position.y);
    groundBodyDef.userData = b2BodyUserData();
    groundBodyDef.userData.pointer = (uintptr_t)ParentEntity;

    info->Body = PhysicsCore::GetWorld()->CreateBody(&groundBodyDef);
    _mass = info->Body->GetMass();
    _type = RigidBodyTypes::Static;

    SetAutoFixture();
}

void RigidBody::SetKinematic()
{
    // TODO: not implemented
}

float RigidBody::GetMass() const
{
    return _mass;
}

void RigidBody::SetMass(float mass)
{
    _mass = mass;
    b2MassData massData;
    massData.mass = _mass;
    info->Body->SetMassData(&massData);
}

RigidBodyTypes::RigidBodyType RigidBody::GetType() const
{
    return _type;
}

void RigidBody::SetType(RigidBodyTypes::RigidBodyType type)
{
    if (type == _type)
        return;

    _type = type;
    switch (type)
    {
        case RigidBodyTypes::None:
            RemovePhysicsBody();
            break;
        case RigidBodyTypes::Dynamic:
            SetDynamic();
            break;
        case RigidBodyTypes::Static:
            SetStatic();
            break;
        case RigidBodyTypes::Kinematic:
            SetKinematic();
            break;
    }
}

void RigidBody::SetAutoFixture()
{
    auto bc = ParentEntity->GetComponent<BoxCollider>();
    if (bc != nullptr)
    {
        b2FixtureDef fixtureDef;
        fixtureDef.shape = bc->info->GroundBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
        info->Body->CreateFixture(&fixtureDef);
    }
}

void RigidBody::RemovePhysicsBody()
{
    if (info == nullptr)
        return;

    PhysicsCore::GetWorld()->DestroyBody(info->Body);
    delete info;
}

void RigidBody::UpdatePhysicsTransformation()
{
    if (inGetTransformation)
        return;

    auto position = ParentEntity->Transform->GetPosition();
    b2Vec2 b2Position;

    b2Position.x = position.x;
    b2Position.y = position.y;
    info->Body->SetTransform(b2Position, ParentEntity->Transform->GetRotation().z);
}

void RigidBody::GetPhysicsTransformation()
{
    inGetTransformation = true;
    // TODO: check body type
    float z = ParentEntity->Transform->GetPosition().z;
    auto position = info->Body->GetPosition();
    float rotation = info->Body->GetAngle();
    ParentEntity->Transform->SetPosition(glm::vec3(position.x, position.y, z));
    ParentEntity->Transform->SetRotation(glm::vec3(0, 0, rotation));
    inGetTransformation = false;
}
