#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
     size = MAX_SIZE;
}

void RingBuffer::setup(int aSize)
{
    size = aSize;
    currentIndex = 0;
    for( int i = 0 ; i < size ; i++ )
    {
        setVec2fValue(ofVec2f(0));
    }
}

void RingBuffer::setIntValue(int value)
{
    buffer[currentIndex] = ofVec2f(value);
    currentIndex = (currentIndex + 1) % size;
}

void RingBuffer::setVec2fValue(ofVec2f value)
{
    buffer[currentIndex] = value;
    currentIndex = (currentIndex + 1) % size;
}

int RingBuffer::getSize()
{
    return size;
}

ofVec2f RingBuffer::operator[] ( int pos )
{
    auto p = abs(currentIndex - pos) % getSize() ;
    return buffer[p] ;
}
