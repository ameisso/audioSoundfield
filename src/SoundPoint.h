//
//  SoundPoint.hpp
//  soundfield
//
//  Created by Antoine on 14/08/2016.
//
//
#pragma once
#include "ofMain.h"
#include "RingBuffer.h"

class SoundListener;

class SoundObject
{
    
public:
    virtual void setup(int posX, int posY);
    virtual void update(){};
    virtual  void draw(){};
    ofRectangle getBitmapStringBoundingBox(string text);
    virtual void setPosition( ofVec2f aPosition );
    ofVec2f getPosition();
    
private:
    ofVec2f position;
};







class SoundPoint : public SoundObject
{
public:
    
    void setup(int posX, int posY, int aMaxDistance, float aLoopRate, string aSoundPath, bool isTrigerable);
    void update(SoundListener listener);
    void setVolume(float volume);
    void draw();
    
private:
    
    void startPlaying();
    bool isPlaying;
    string soundPath;
    int maxDistance;
    float loopRate;
    float loopOffset;
    float lastPlayStartTime;
    float lastPlayEndTime;
    ofSoundPlayer soundPlayer;
    bool trigerable;
    bool listenerIsInside;
};







class AmbiantSoundPoint : public SoundPoint
{
public:
    void setup(string aSoundPath,float aLoopRate, float volume);
    void draw();
};







class SoundListener : public SoundObject
{
public:
    void setup(int posX, int posY, float anOrientation);
    void update();
    void draw();
    
    void setPosition( ofVec2f aPosition );
    float getOrientation();
    void setWalkspeed( ofVec2f aWalkSpeed );
    ofVec2f getWalkSpeed();
    
private:
    float orientation;
    ofVec2f walkSpeed;
    ofVec2f direction;
    
    RingBuffer lastPositions;
};




