//
// Created by Ned on 8/04/2026.
//

#include "CircleBuffer.h"

template<typename T>
CircleBuffer<T>::CircleBuffer(const size_t size) : bufferSize(size)
{
    buffer = new T[bufferSize];
}

template<typename T>
void CircleBuffer<T>::insert(T element)
{
    buffer(endPointer++) = element;
    if (endPointer >= bufferSize)
    {
        endPointer = 0;
        startPointer++;
    }
}

template<typename T>
T CircleBuffer<T>::at(size_t index)
{
    // get the fixed memory index rather than the relative circular buffer index
    // does this comment make sense? its 3am! who gaf!!
    size_t fixedIndex = (startPointer + index) % bufferSize;
    return buffer(fixedIndex);
}
