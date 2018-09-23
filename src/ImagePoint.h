#ifndef ImagePoint_h
#define ImagePoint_h

#include "ofMain.h"
#include "ofxGifDecoder.h"
#include "ofxGifFile.h"

#define IMAGE_TYPE_GIF 0
#define IMAGE_TYPE_PNG 1



class ImageObject
{
    
public:
    virtual void setup(int posX, int posY);
    virtual void update(){};
    virtual  void draw(){};
    virtual void setPosition( ofVec2f aPosition );
    ofVec2f getPosition();
    
private:
    ofVec2f position;
};





class ImagePoint : public ImageObject
{
public:
    
    void setup(int posX, int posY, string aPath, float aScale);
    void update(ofVec2f listenerPosition);
    void draw();
    
private:
    int type;
    ofImage pngImage;
    ofxGifDecoder gifDecoder;
    ofxGifFile gifFile;
    ofxGifFrame *currentGifFrame;
    float gifFrameDuration = 0;
    
    string path;
    int currentGifFrameIndex;
    long lastFrameTime;
    float scale;
    bool imageLoaded = false;
};



#endif

