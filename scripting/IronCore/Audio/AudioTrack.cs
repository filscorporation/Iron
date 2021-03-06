﻿using System.Runtime.CompilerServices;

namespace Iron
{
    /// <summary>
    /// Loaded audio track
    /// </summary>
    public class AudioTrack : Resource
    {
        internal AudioTrack(uint id) : base(id) { }

        /// <summary>
        /// Track length in seconds
        /// </summary>
        public float Length => GetLength_Internal(ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetLength_Internal(uint audioTrackID);
    }
}