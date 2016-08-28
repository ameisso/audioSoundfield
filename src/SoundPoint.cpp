//
//  SoundPoint.cpp
//  soundfield
//
//  Created by Antoine on 14/08/2016.
//
//

#include "SoundPoint.h"

//SOUND OBJECT
void SoundObject::setup(int posX, int posY)
{
    position.x = posX;
    position.y = posY;
}

ofRectangle SoundObject::getBitmapStringBoundingBox(string text)
{
    vector<string> lines = ofSplitString(text, "\n");
    int maxLineLength = 0;
    for(int i = 0; i < (int)lines.size(); i++)
    {
        // tabs are not rendered
        const string & line(lines[i]);
        int currentLineLength = 0;
        for(int j = 0; j < (int)line.size(); j++)
        {
            if (line[j] == '\t')
            {
                currentLineLength += 8 - (currentLineLength % 8);
            }
            else
            {
                currentLineLength++;
            }
        }
        maxLineLength = MAX(maxLineLength, currentLineLength);
    }
    
    int padding = 4;
    int fontSize = 8;
    float leading = 1.7;
    int height = lines.size() * fontSize * leading - 1;
    int width = maxLineLength * fontSize;
    return ofRectangle(0,0,width, height);
}

//SOUND POINT


void SoundPoint::setup(int posX, int posY, int aMaxDistance, float aLoopRate, string aSoundPath)
{
    SoundObject::setup(posX, posY);
    maxDistance = aMaxDistance;
    loopRate = aLoopRate;
    soundPath = aSoundPath;
    soundPlayer.load(soundPath);
    if(loopRate == 0)
    {
        soundPlayer.setLoop(true);
    }
    else
    {
        soundPlayer.setLoop(false);
    }
    soundPlayer.setVolume(0);
    startPlaying();
}

void SoundPoint::update(SoundListener listener)
{
    if( soundPlayer.isPlaying() )
    {
        float distance =  position.distance( listener.position );
        float volume = ofMap(distance, maxDistance, 0, 0, 1);
        soundPlayer.setVolume(volume);
        
        ofVec2f directionVector(0,-1);
        directionVector.rotate(listener.orientation);
        ofVec2f sourceVector = ofVec2f(position.x-listener.position.x,position.y-listener.position.y);
        float listenerAngle = directionVector.angle(sourceVector);
        float pan;
        if( listenerAngle > 0)//RIGHT
        {
            if (listenerAngle <= 90) //BOTTOM
            {
                pan = ofMap(listenerAngle, 0, 90, 0, 1);
            }
            else
            {
                pan = ofMap(listenerAngle, 90, 180, 1, 0);
            }
        }
        else //LEFT
        {
            if (abs(listenerAngle) <= 90) //BOTTOM
            {
                pan = ofMap(listenerAngle, 0, -90, 0, -1);
            }
            else
            {
                pan = ofMap(listenerAngle, -90, -180, -1, 0);
            }
        }
        soundPlayer.setPan(pan);
    }
    else
    {
        if( isPlaying )
        {
            isPlaying = false;
            float duration = ofGetElapsedTimeMillis() - lastPlayStartTime;
            loopOffset = ofRandom(duration*0.1); // 10% of global time.
            lastPlayEndTime = ofGetElapsedTimeMillis();
        }
        if( ofGetElapsedTimeMillis()-lastPlayEndTime > loopRate * 1000.0 + loopOffset && !isPlaying )
        {
            startPlaying();
        }
    }
}

void SoundPoint::setVolume(float volume)
{
    soundPlayer.setVolume(volume);
}

void SoundPoint::draw()
{
    ofFill();
    if( soundPlayer.isPlaying() )
    {
        ofSetColor(150,100,0);
    }
    else
    {
        ofSetColor(80);
    }
    ofDrawCircle(position.x,position.y,10);
    
    ofNoFill();
    ofDrawCircle(position.x,position.y,maxDistance);
    
    if( ! soundPlayer.isPlaying() )
    {
        ofSetColor(150);
    }
    
    ofDrawBitmapString(soundPath, position.x-getBitmapStringBoundingBox(soundPath).getWidth()/2, position.y-20);
    if(  soundPlayer.isPlaying() )
    {
        string volumeString("vol : "+ofToString(int(soundPlayer.getVolume()*100)));
        ofDrawBitmapString(volumeString, position.x-getBitmapStringBoundingBox(volumeString).getWidth()/2, position.y+20);
        
        string panString("pan : "+ofToString(int(soundPlayer.getPan()*100)));
        ofDrawBitmapString(panString, position.x-getBitmapStringBoundingBox(panString).getWidth()/2, position.y+30);
    }
    else
    {
        int nextPlay = abs((ofGetElapsedTimeMillis() - (lastPlayEndTime + loopOffset + loopRate*1000))/1000);
        string nextString("next : "+ofToString(nextPlay));
        ofDrawBitmapString(nextString, position.x-getBitmapStringBoundingBox(nextString).getWidth()/2, position.y+20);
    }
}

void SoundPoint::startPlaying()
{
    isPlaying = true;
    lastPlayStartTime = ofGetElapsedTimeMillis();
    soundPlayer.play();
}





//AMBIANT SOUND POINT
void AmbiantSoundPoint::setup(string aSoundPath,float aLoopRate, float volume)
{
    SoundPoint::setup(0, 0, 100,aLoopRate, aSoundPath);
    setVolume(volume);
}

void AmbiantSoundPoint::draw()
{
    
}








//SOUND LISTENER

void SoundListener::setup(int posX, int posY, float anOrientation)
{
    SoundObject::setup(posX, posY);
    orientation = anOrientation;
    walkSpeed = 10;
}

void SoundListener::update(ofVec2f direction)
{
    direction.normalize();
    position.x += direction.x*walkSpeed;
    position.y += direction.y*walkSpeed;
}

void SoundListener::draw()
{
    ofFill();
    int squareSize = 20;
    ofSetColor(200,000,100);
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofPushMatrix();
    ofTranslate(position.x,position.y);
    ofRotate(45);
    ofDrawRectangle(0,0, squareSize, squareSize);
    ofPopMatrix();
    
    ofVec2f directionVector(0,-30);
    directionVector.rotate(orientation);
    ofDrawLine(position, ofVec2f(position.x+directionVector.x,position.y+directionVector.y));
}
