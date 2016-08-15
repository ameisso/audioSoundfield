#pragma once

#include "ofMain.h"
#include "SoundPoint.h"
#include "ofxXmlSettings.h"
#include "ofxOSC.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
        void loadSettings();
		void update();
		void draw();
    void handleOSC();

		void keyPressed(int key);
		void keyReleased(int key);
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);
        void mouseMoved(int x, int y);
    
    SoundListener listener;
    ofxXmlSettings settings;
    ofxOscReceiver oscReceiver;
    
    vector<SoundPoint> soundPoints;
    vector<AmbiantSoundPoint> ambiantPoints;
};
