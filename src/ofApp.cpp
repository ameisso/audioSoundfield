#include "ofApp.h"

void ofApp::setup()
{
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofSetCircleResolution(100);
    
    loadSettings();
    ofHideCursor();
}

void ofApp::exit()
{
    ofShowCursor();
}

void ofApp::loadSettings()
{
    soundPoints.clear();
    ambiantPoints.clear();
    imagePoints.clear();
    
    if(settings.loadFile("settings.xml"))
    {
        settings.pushTag("soundPoints");
        int numberOfSavedPoints = settings.getNumTags("soundPoint");
        for(int i = 0; i < numberOfSavedPoints; i++)
        {
            settings.pushTag("soundPoint", i);
            SoundPoint soundPoint;
            soundPoint.setup(settings.getAttribute("params", "x", 0.0), settings.getAttribute("params", "y", 0.0), settings.getAttribute("params", "maxDistance", 0.0), settings.getAttribute("params", "loopRate", 0.0) , settings.getAttribute("params", "name", ""),  settings.getAttribute("params", "triggerable", 0));
            soundPoints.push_back(soundPoint);
            settings.popTag();
        }
        settings.popTag();
        
        settings.pushTag("ambiantSounds");
        for(int i = 0; i < settings.getNumTags("ambiantSound"); i++)
        {
            settings.pushTag("ambiantSound", i);
            AmbiantSoundPoint ambiantSoundPoint;
            ambiantSoundPoint.setup(settings.getAttribute("params", "name", ""), settings.getAttribute("params", "loopRate", 0.0), settings.getAttribute("params", "volume", 0.0));
            ambiantPoints.push_back(ambiantSoundPoint);
            settings.popTag();
        }
        settings.popTag();
        settings.pushTag("imagePoints");
        for(int i = 0; i < settings.getNumTags("imagePoint"); i++)
        {
            settings.pushTag("imagePoint", i);
            ImagePoint point;
            point.setup(settings.getAttribute("params", "x", 0.0), settings.getAttribute("params", "y", 0.0),settings.getAttribute("params", "name", ""), settings.getAttribute("params", "triggerable", 0),settings.getAttribute("params", "scale", 1.0));
            imagePoints.push_back(point);
            settings.popTag();
        }
        settings.popTag();
        settings.pushTag("OSC");
        oscReceiver.setup(-1);
        oscSender.setup("localhost", 1);
        oscReceiver.setup(settings.getValue("listenPort", 8000));
        oscSender.setup(settings.getValue("targetIP", "localhost"), settings.getValue("targetPort", 1234));
        settings.popTag();
        
        settings.pushTag("PARAMS");
        listener.setup(350, 500, 0, settings.getValue("listenerSmoothness", 0));
        viewPortScale = settings.getValue("viewPortScale", 1.0);
        mapFbo.allocate(settings.getAttribute("mapsize","x", 200),settings.getAttribute("mapsize","y", 200),GL_RGB);
        settings.popTag();
        settings.pushTag("IMAGE");
        string cursorName = "images/"+settings.getValue("cursorName", "");
        ofImage listenerImage;
        if( listenerImage.load(cursorName) );
        {
            listener.setImage(listenerImage, settings.getValue("cursorScale", 1.0));
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
    handleOSC();
    for(vector<SoundPoint>::iterator it = soundPoints.begin(); it != soundPoints.end(); ++it)
    {
        (it)->update(listener);
    }
    
    for(vector<AmbiantSoundPoint>::iterator it = ambiantPoints.begin(); it != ambiantPoints.end(); ++it)
    {
        (it)->update(listener);
    }
    for(vector<ImagePoint>::iterator it = imagePoints.begin(); it != imagePoints.end(); ++it)
    {
        (it)->update(listener.getPosition());
    }
    keepListenerInside();
    listener.update();
}

void ofApp::keepListenerInside()
{
    if( listener.getPosition().x > mapFbo.getWidth()-listener.getImageSize().x/2 )
    {
        listener.setPosition(ofVec2f(listener.getImageSize().x/2,listener.getPosition().y));
    }
    else if( listener.getPosition().x < listener.getImageSize().x/2  )
    {
        listener.setPosition(ofVec2f(mapFbo.getWidth()-listener.getImageSize().x/2,listener.getPosition().y));
    }
    else if( listener.getPosition().y >  mapFbo.getHeight()-listener.getImageSize().y/2 )
    {
        listener.setPosition(ofVec2f(listener.getPosition().x,listener.getImageSize().y/2));
    }
    else if( listener.getPosition().y < listener.getImageSize().y/2  )
    {
        listener.setPosition(ofVec2f(listener.getPosition().x,mapFbo.getHeight()-listener.getImageSize().y/2));
    }
}

void ofApp::draw()
{
    ofSetColor(255);
    
    mapFbo.begin();
    ofBackground(50);
    ofSetColor(255);
    ofSetRectMode(OF_RECTMODE_CENTER);
    for(vector<ImagePoint>::iterator it = imagePoints.begin(); it != imagePoints.end(); ++it)
    {
        it->draw();
    }
    ofSetColor(255,255,0);
    if( showSounds )
    {
        for(vector<SoundPoint>::iterator it = soundPoints.begin(); it != soundPoints.end(); ++it)
        {
            it->draw();
        }
    }
    listener.drawImage();
    mapFbo.end();
    ofVec2f drawSize = ofVec2f(mapFbo.getWidth()*viewPortScale,mapFbo.getHeight()*viewPortScale);
    if( showMapMode )
    {
        mapFbo.draw(0 , 0, ofGetWidth(),ofGetHeight());// for map mode
    }
    else
    {
        mapFbo.getTexture().drawSubsection(0,0,ofGetWidth(),ofGetHeight(),listener.getPosition().x-drawSize.x/2,listener.getPosition().y-drawSize.y/2,drawSize.x,drawSize.y);
    }
    
    if(showListenerPosition)
    {
        ofSetColor(200);
        
        ofDrawRectangle(0, 0, 200, 25);
        ofSetColor(0);
        ofDrawBitmapString("listener -> "+ofToString(listener.getPosition().x)+" "+ofToString(listener.getPosition().y), 15,15);
        int offset = 25;
        for(vector<AmbiantSoundPoint>::iterator it = ambiantPoints.begin(); it != ambiantPoints.end(); ++it)
        {
            if(   it->getIsPlaying() )
            {
                offset += 25;
                ofSetColor(100);
                ofDrawRectangle(0, offset-25, 500, 25);
                ofSetColor(0);
                ofDrawBitmapString("ambiant -> "+it->getPath(), 15,offset-10);
            }
        }
    }
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
    else if(key =='i')
    {
        showListenerPosition = ! showListenerPosition;
    }
    else if(key =='p')
    {
        showSounds = ! showSounds;
    }
    else if(key =='m')
    {
        showMapMode = ! showMapMode;
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
    mouseDelta = ofVec2f(x,y);
    if( showMapMode )
    {
        int mappedX = ofMap(x,0,ofGetWidth(),0,mapFbo.getWidth());
        int mappedY = ofMap(y,0,ofGetHeight(),0,mapFbo.getHeight());
        listener.setPosition(ofVec2f(mappedX,mappedY));
    }
}

void ofApp::mouseDragged(int x, int y , int button)
{
    if( ! showMapMode  )
    {
        ofVec2f delta = ofVec2f(mouseDelta.x-x,mouseDelta.y-y);
        updatePosition(listener.getPosition().x+delta.x,listener.getPosition().y+delta.y);
    }
    mouseDelta = ofVec2f(x,y);
}

void ofApp::mouseMoved(int x, int y)
{
    if( isMovingWithMouse )
    {
        updatePosition(x,y);
    }
}

void ofApp::updatePosition(int x, int y)
{
    listener.setPosition( ofVec2f(x,y) );
    ofxOscMessage m;
    m.addIntArg(listener.getPosition().x);
    m.addIntArg(listener.getPosition().y);
    m.setAddress("/listener/position");
    oscSender.sendMessage(m);
}

bool ofApp::fileExists(string name)
{
    bool fileExists = false;
    ofDirectory dir("");
    dir.listDir();
    for(int i = 0; i < dir.size(); i++)
    {
        if(dir.getName(i) == name)
        {
            fileExists = true;
            break;
        }
    }
    return fileExists;
}

