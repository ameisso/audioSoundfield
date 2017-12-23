#include "ofApp.h"

void ofApp::setup()
{
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
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
            p.setup(settings.getValue("posX", 0),settings.getValue("posY", 0),settings.getValue("maxDistance", 0), settings.getValue("loopRate", 0),settings.getValue("fileName", ""));
            soundPoints.push_back(p);
            settings.popTag();
        }
        settings.popTag();
        
        settings.pushTag("ambiantSounds");
        for(int i = 0; i < settings.getNumTags("ambiantSound"); i++)
        {
            settings.pushTag("ambiantSound", i);
            AmbiantSoundPoint a;
            a.setup(settings.getValue("fileName", ""),settings.getValue("loopRate", 1.0),settings.getValue("volume", 1.0));
            ambiantPoints.push_back(a);
            settings.popTag();
        }
        settings.popTag();
        settings.pushTag("OSC");
        oscReceiver.setup(-1);
        oscSender.setup("localhost", 1);
        oscReceiver.setup(settings.getValue("listenPort", 8000));
        oscSender.setup(settings.getValue("targetIP", "localhost"), settings.getValue("targetPort", 1234));
        settings.popTag();
        
        settings.pushTag("SVG");
        backgroundPlan.load(settings.getValue("fileName", ""));
        svgScale = settings.getValue("scale", 1.0);
        svgOffset = ofPoint(settings.getValue("offsetX", 0),settings.getValue("offsetY", 0));
        
        settings.popTag();
    }
    else
    {
        ofLogError("Position file did not load!");
    }
}

void ofApp::update()
{
    handleOSC();
    for(vector<SoundPoint>::iterator it = soundPoints.begin(); it != soundPoints.end(); ++it)
    {
        (it)->update(listener);
    }
    listener.update();
}

void ofApp::draw()
{
    ofBackground(50);
    ofPushMatrix();
    ofTranslate(svgOffset.x,svgOffset.y);
    ofScale(svgScale, svgScale);
    
    
    backgroundPlan.draw();
    ofPopMatrix();
    ofSetColor(255,255,0);
    for(vector<SoundPoint>::iterator it = soundPoints.begin(); it != soundPoints.end(); ++it)
    {
        it->draw();
    }
    listener.draw();
}


void ofApp::handleOSC()
{
    while(oscReceiver.hasWaitingMessages())
    {
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/ping")
        {
            ofLog() << "ping";
        }
        else if(m.getAddress() == "/listener/position")
        {
            if (m.getNumArgs() == 2)
            {
                if (m.getArgType(0) == OFXOSC_TYPE_INT32 && m.getArgType(1) == OFXOSC_TYPE_INT32)
                {
                    listener.setPosition( ofVec2f(m.getArgAsInt32(0), m.getArgAsInt32(1)) );
                }
                else if ((m.getArgType(0) == OFXOSC_TYPE_FLOAT && m.getArgType(1) == OFXOSC_TYPE_FLOAT))
                {
                    listener.setPosition( ofVec2f(m.getArgAsFloat(0)*ofGetWidth(), (1-m.getArgAsFloat(1))*ofGetHeight()) );
                }
            }
        }
        else
        {
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            msg_string += "UNEXPECTED MESSAGE: ";
            msg_string += m.getAddress();
            msg_string += ": ";
            for(int i = 0; i < m.getNumArgs(); i++)
            {
                // get the argument type
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32)
                {
                    msg_string += ofToString(m.getArgAsInt32(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT)
                {
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING)
                {
                    msg_string += m.getArgAsString(i);
                }
                else
                {
                    msg_string += "unknown";
                }
            }
            // add to the list of strings to display
            ofLog() << msg_string;
        }
    }
}

void ofApp::keyPressed(int key)
{
    ofVec2f direction(0,0);
    if(key == OF_KEY_DOWN)
    {
        direction = ofVec2f(0,1);
    }
    if(key == OF_KEY_UP)
    {
        direction += ofVec2f(0,-1);
    }
    if(key == OF_KEY_LEFT)
    {
        direction += ofVec2f(-1,0);
    }
    if(key == OF_KEY_RIGHT)
    {
        direction += ofVec2f(1,0);
    }
    listener.setWalkspeed( listener.getWalkSpeed() + direction );
    
    if(key =='r')
    {
        loadSettings();
    }
    else if(key =='f')
    {
        ofToggleFullscreen();
    }
    else if(key =='d')
    {
        isMovingWithMouse = true;
    }
    
}

void ofApp::keyReleased(int key)
{
    if( key == OF_KEY_DOWN || key == OF_KEY_UP || key == OF_KEY_LEFT || key == OF_KEY_RIGHT )
    {
        listener.setWalkspeed(ofVec2f(0));
    }
    else if(key =='d')
    {
        isMovingWithMouse = false;
    }
}


void ofApp::mousePressed(int x, int y , int button)
{
    listener.setPosition( ofVec2f(x,y) );
}

void ofApp::mouseDragged(int x, int y , int button)
{
    listener.setPosition( ofVec2f(x,y) );
    ofxOscMessage m;
    m.addIntArg(x);
    m.addIntArg(y);
    m.setAddress("/listener/position");
    oscSender.sendMessage(m);
}

void ofApp::mouseMoved(int x, int y)
{
    if( isMovingWithMouse )
    {
        listener.setPosition( ofVec2f(x,y) );
        ofxOscMessage m;
        m.addIntArg(x);
        m.addIntArg(y);
        m.setAddress("/listener/position");
        oscSender.sendMessage(m);
    }
}
