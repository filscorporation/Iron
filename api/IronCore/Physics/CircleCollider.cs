﻿using System.Runtime.CompilerServices;

namespace Iron
{
    public class CircleCollider : Collider
    {
        public float Radius
        {
            get => GetRadius_Internal(Entity.ID);
            set => SetRadius_Internal(Entity.ID, value);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetRadius_Internal(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetRadius_Internal(uint entityID, float radius);
    }
}