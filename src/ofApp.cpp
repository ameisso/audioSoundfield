#include "ofApp.h"

void ofApp::setup()
{
    ofSetFrameRate(30);
    ofSetCircleResolution(100);
    
    listener.setup(350, 500, 0);
    loadSettings();
}

void ofApp::loadSettings()
{
    soundPoints.clear();
    ambiantPoints.clear();
    
    if(settings.loadFile("settings.xml"))
    {
        settings.pushTag("soundPoints");
        int numberOfSavedPoints = settings.getNumTags("soundPoint");
        for(int i = 0; i < numberOfSavedPoints; i++)
        {
            settings.pushTag("soundPoint", i);
            
            SoundPoint p;
            p.setup(settings.getValue("posX", 0),settings.getValue("posY", 0),settings.getValue("maxDistance", 0),settings.getValue("fileName", ""));
            soundPoints.push_back(p);
            settings.popTag();
        }
        settings.popTag();
        
        settings.pushTag("ambiantSounds");
        for(int i = 0; i < settings.getNumTags("ambiantSound"); i++)
        {
            settings.pushTag("ambiantSound", i);
            AmbiantSoundPoint a;
            a.setup(settings.getValue("fileName", ""),settings.getValue("volume", 1.0));
            ambiantPoints.push_back(a);
            settings.popTag();
        }
        settings.popTag();
    }
    else
    {
        ofLogError("Position file did not load!");
    }
}

void ofApp::update()
{
    for(vector<SoundPoint>::iterator it = soundPoints.begin(); it != soundPoints.end(); ++it)
    {
        (it)->update(listener);
    }
}

void ofApp::draw()
{
    ofBackground(50);
    ofSetColor(255,255,0);
    for(vector<SoundPoint>::iterator it = soundPoints.begin(); it != soundPoints.end(); ++it)
    {
        it->draw();
    }
    listener.draw();
}

void ofApp::keyPressed(int key)
{
    ofVec2f direction(0,0);
        if(key == OF_KEY_DOWN)
        {
            direction = ofVec2f(0,1);
        }
        else  if(key == OF_KEY_UP)
        {
            direction = ofVec2f(0,-1);
        }
        else  if(key == OF_KEY_LEFT)
        {
            direction = ofVec2f(-1,0);
        }
        else  if(key == OF_KEY_RIGHT)
        {
            direction = ofVec2f(1,0);
        }
    listener.update(direction);
    
    if(key =='r')
    {
        loadSettings();
    }
}

void ofApp::keyReleased(int key){
    
}


void ofApp::mousePressed(int x, int y , int button)
{
    listener.position.x = x;
    listener.position.y = y;
}

void ofApp::mouseDragged(int x, int y , int button)
{
    listener.position.x = x;
    listener.position.y = y;
}

void ofApp::mouseMoved(int x, int y)
{

}