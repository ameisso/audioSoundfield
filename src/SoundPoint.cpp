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

void SoundObject::setPosition(ofVec2f aPosition)
{
    position = aPosition;
}

ofVec2f SoundObject::getPosition()
{
    return position;
}

//SOUND POINT
void SoundPoint::setup(int posX, int posY, int aMaxDistance, float aLoopRate, string aSoundPath, bool isTrigerable)
{
    SoundObject::setup(posX, posY);
    maxDistance = aMaxDistance;
    loopRate = aLoopRate;
    soundPath = "sounds/"+aSoundPath;
    trigerable = isTrigerable;
    if( ! soundPlayer.load(soundPath) )
    {
        ofLog()<<"error sound "<<soundPath<< " doesn't exist";
    }
    if(loopRate == 0 && ! trigerable)
    {
        soundPlayer.setLoop(true);
    }
    else
    {
        soundPlayer.setLoop(false);
    }
    soundPlayer.setVolume(0);
    lastPlayEndTime = ofGetElapsedTimeMillis();
}

void SoundPoint::update(SoundListener listener)
{
    float distance =  getPosition().distance( listener.getPosition() );
    if( soundPlayer.isPlaying() )
    {
        if( getPosition() != ofVec2f(0) )
        {
            float newVolume = ofMap(distance, maxDistance, 0, 0, 1);
            soundPlayer.setVolume(newVolume);
            ofVec2f directionVector(0,-1);
            directionVector.rotate(listener.getOrientation());
            ofVec2f sourceVector = ofVec2f(getPosition().x-listener.getPosition().x,getPosition().y-listener.getPosition().y);
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
            setVolume(volume);
        }
    }
    else
    {
        if(distance > maxDistance)
        {
            listenerIsInside = false;
        }
        if( isPlaying )
        {
            isPlaying = false;
            float duration = ofGetElapsedTimeMillis() - lastPlayStartTime;
            loopOffset = ofRandom(duration*0.1); // 10% of global time.
            lastPlayEndTime = ofGetElapsedTimeMillis();
        }
        if( ofGetElapsedTimeMillis()-lastPlayEndTime > loopRate * 1000.0 + loopOffset && !isPlaying && !trigerable )
        {
            startPlaying();
        }
        if( trigerable && distance < maxDistance && !isPlaying && !listenerIsInside  )
        {
            startPlaying();
            listenerIsInside = true;
        }
        if( ! trigerable && loopRate <= 0)
        {
            startPlaying();
        }
    }
}

void SoundPoint::setVolume(float aVolume)
{
    volume = aVolume;
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
    ofDrawCircle(getPosition().x,getPosition().y,10);
    
    ofNoFill();
    ofDrawCircle(getPosition().x,getPosition().y,maxDistance);
    
    if( ! soundPlayer.isPlaying() )
    {
        ofSetColor(150);
    }
    
    ofDrawBitmapString(soundPath, getPosition().x-getBitmapStringBoundingBox(soundPath).getWidth()/2, getPosition().y-20);
    if(  soundPlayer.isPlaying() )
    {
        string volumeString("vol : "+ofToString(int(soundPlayer.getVolume()*100)));
        //ofDrawBitmapString(volumeString, getPosition().x-getBitmapStringBoundingBox(volumeString).getWidth()/2, getPosition().y+20);
        
        string panString("pan : "+ofToString(int(soundPlayer.getPan()*100)));
        //ofDrawBitmapString(panString, getPosition().x-getBitmapStringBoundingBox(panString).getWidth()/2, getPosition().y+30);
    }
    else if( ! trigerable )
    {
        int nextPlay = abs((ofGetElapsedTimeMillis() - (lastPlayEndTime + loopOffset + loopRate*1000))/1000);
        string nextString("next : "+ofToString(nextPlay));
        ofDrawBitmapString(nextString, getPosition().x-getBitmapStringBoundingBox(nextString).getWidth()/2, getPosition().y+20);
    }
}

void SoundPoint::startPlaying()
{
    isPlaying = true;
    lastPlayStartTime = ofGetElapsedTimeMillis();
    soundPlayer.play();
}

bool SoundPoint::getIsPlaying()
{
    return soundPlayer.isPlaying();
}

string SoundPoint::getPath()
{
    return soundPath;
}


//AMBIANT SOUND POINT
void AmbiantSoundPoint::setup(string aSoundPath,float aLoopRate, float aVolume)
{
    SoundPoint::setup(0, 0, 100,aLoopRate, aSoundPath,false);
    setVolume(aVolume);
}

void AmbiantSoundPoint::draw()
{
    
}







//SOUND LISTENER
void SoundListener::setup(int posX, int posY, float anOrientation, int orientationSmoothness)
{
    SoundObject::setup(posX, posY);
    orientation = anOrientation;
    walkSpeed = ofVec2f(0);
    lastPositions.setup(orientationSmoothness);
}

void SoundListener::update()
{
    setPosition( getPosition() + walkSpeed );
}

void SoundListener::drawCursor()
{
    ofFill();
    for( int i = 1 ; i < lastPositions.getSize() ; i++ )
    {
        float weight = (lastPositions.getSize()-i)/float(lastPositions.getSize());
        ofSetColor(200,200,100,weight*255);
        ofDrawCircle(lastPositions[lastPositions.getSize()-i], 3);
    }
    ofSetColor(200,200,100);
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofPushMatrix();
    ofTranslate(getPosition().x,getPosition().y);
    ofRotateDeg(orientation);
    ofDrawTriangle(0,0,-5, 20, 5, 20);
    ofPopMatrix();
    ofVec2f orientationVector(0,-30);
    orientationVector.rotate(orientation);
    ofDrawLine(getPosition(), ofVec2f(getPosition().x+orientationVector.x,getPosition().y+orientationVector.y));
}

void SoundListener::drawImage()
{
    ofSetColor(255);
    if( gotImage )
    {
        ofPushMatrix();
        ofTranslate(getPosition().x,getPosition().y);
       // ofRotateDeg(sin(2*M_PI*0.7*ofGetElapsedTimeMillis()/1000.0));
        listenerImage.draw(0,0,listenerImage.getWidth(),listenerImage.getHeight());
        ofPopMatrix();
    }
}

void SoundListener::setPosition(ofVec2f aPosition)
{
    ofVec2f meanVec = ofVec2f(0);
    for( float i = 1 ; i < lastPositions.getSize() ; i++ )
    {
        float weight = (lastPositions.getSize()-i)/float(lastPositions.getSize());
        meanVec += (lastPositions[lastPositions.getSize()+i]-lastPositions[lastPositions.getSize()+i+1])*weight;
    }
    orientation = ofVec2f(0,1).angle(meanVec);
    SoundObject::setPosition(aPosition);
    lastPositions.setVec2fValue(aPosition);
}

float SoundListener::getOrientation()
{
    return orientation;
}

void SoundListener::setWalkspeed(ofVec2f aWalkspeed)
{
    walkSpeed = ofVec2f(fmin(aWalkspeed.x,LISTENER_MAX_SPEED), fmin(aWalkspeed.y,LISTENER_MAX_SPEED));
}

ofVec2f SoundListener::getWalkSpeed()
{
    return walkSpeed;
}

void SoundListener::setImage(ofImage anImage, float scale)
{
    listenerImage = anImage;
    listenerScale = scale;
    listenerImage.resize(listenerImage.getWidth()*scale, listenerImage.getHeight()*scale);
    gotImage = true;
}

ofVec2f SoundListener::getImageSize()
{
    return ofVec2f(listenerImage.getWidth(), listenerImage.getHeight());
}
