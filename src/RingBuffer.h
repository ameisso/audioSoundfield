#ifndef RingBuffer_h
#define RingBuffer_h

#include <ofMain.h>
#define MAX_SIZE 500
class RingBuffer{
public:
    
    RingBuffer();
    void setup(int aSize);
    void setIntValue( int value );
    void setVec2fValue( ofVec2f value );
    int getSize();
    ofVec2f operator[] ( int pos );
    
private:
    ofVec2f buffer[MAX_SIZE];
    int size = 100;
    int currentIndex = 0;
    
};


#endif
