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
        size_t bufferSize;
        // pointers to front and back of our buffer
        size_t startPointer = 0;
        size_t endPointer = 0;
    public:
        CircleBuffer(size_t size);
        void insert(T element);
        T at(size_t index);

};


#endif //GAMEBOY2_CIRCLEBUFFER_H