#pragma once

#include "ofMain.h"
#include "SoundPoint.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
        void loadSettings();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);
        void mouseMoved(int x, int y);
    
    SoundListener listener;
    ofxXmlSettings settings;
    
    vector<SoundPoint> soundPoints;
    vector<AmbiantSoundPoint> ambiantPoints;
};
