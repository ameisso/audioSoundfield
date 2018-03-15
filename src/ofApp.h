#pragma once

#include "ofMain.h"
#include "SoundPoint.h"
#include "ofxXmlSettings.h"
#include "ofxOSC.h"
#include "ImagePoint.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void exit();
    void loadSettings();
    void update();
    void keepListenerInside();
    void draw();
    void handleOSC();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseMoved(int x, int y);
    
private:
    
    SoundListener listener;
    ofxXmlSettings settings;
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    
    ofFbo mapFbo;
    
    vector<SoundPoint> soundPoints;
    vector<AmbiantSoundPoint> ambiantPoints;
    vector<ImagePoint> imagePoints;
    
    bool isMovingWithMouse;
    
    void updatePosition(int x, int y);
    bool fileExists(string name);
    
    bool cursorImageLoaded = false;
    bool showListenerPosition = true;
    bool showSounds = false;
    bool showMapMode = false;
    float viewPortScale;
    
    ofVec2f mouseDelta; 
};
