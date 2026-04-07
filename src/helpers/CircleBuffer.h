//
// Created by Ned on 8/04/2026.
//

#ifndef GAMEBOY2_CIRCLEBUFFER_H
#define GAMEBOY2_CIRCLEBUFFER_H

// simple class to implement a circle buffer
template<typename T>
class CircleBuffer
{
    private:
        T* buffer;
        // total internal size
        size_t bufferSize;
        // length of filled buffer
        size_t bufferLength = 0;
        // pointers to front and back of our buffer
        size_t startPointer = 0;
        size_t endPointer = 0;
    public:
        CircleBuffer(const size_t size) : bufferSize(size)
        {
            buffer = new T[bufferSize];
        }
        void insert(T element)
        {
            if (endPointer == bufferSize)
            {
                endPointer = 0;
                startPointer++;
            }
            buffer[endPointer++] = element;
            if (bufferLength < bufferSize) bufferLength++;
        }
        T at(size_t index)
        {
            // get the fixed memory index rather than the relative circular buffer index
            // does this comment make sense? its 3am! who gaf!!
            size_t fixedIndex = (startPointer + index) % bufferLength;
            return buffer[fixedIndex];
        }
        size_t length() const { return bufferLength; }

};

#endif //GAMEBOY2_CIRCLEBUFFER_H