﻿using System.Runtime.CompilerServices;

namespace Iron
{
    public class RigidBody : Component
    {
        public float Mass
        {
            get => GetMass_Internal(Entity.ID);
            set => SetMass_Internal(Entity.ID, value);
        }

        public RigidBodyType RigidBodyType
        {
            get => GetRigidBodyType_Internal(Entity.ID);
            set => SetRigidBodyType_Internal(Entity.ID, value);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetMass_Internal(ulong entityID);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMass_Internal(ulong entityID, float mass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern RigidBodyType GetRigidBodyType_Internal(ulong entityID);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetRigidBodyType_Internal(ulong entityID, RigidBodyType type);
    }
}