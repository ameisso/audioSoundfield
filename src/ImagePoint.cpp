#include "ImagePoint.h"

void ImageObject::setup(int posX, int posY)
{
    position.x = posX;
    position.y = posY;
}

void ImageObject::setPosition(ofVec2f aPosition)
{
    position = aPosition;
}

ofVec2f ImageObject::getPosition()
{
    return position;
}


//IMAGE POINT
void ImagePoint::setup(int posX, int posY, string aPath, bool isTrigerable)
{
    path = aPath;
    ImageObject::setup(posX, posY);
    trigerable = isTrigerable;
#warning test if it ends in .gif
    if(true)
    {
        type = IMAGE_TYPE_GIF;
        gifDecoder.decode(path);
        gifFile = gifDecoder.getFile();
        currentGifFrameIndex = 0;
    }
    else
    {
        type = IMAGE_TYPE_PNG;
    }
}

void ImagePoint::update(ofVec2f listenerPosition)
{
    if(type == IMAGE_TYPE_GIF)
    {
        float duration = currentGifFrame->getDuration();
        if( ofGetElapsedTimeMillis()-lastFrameTime > duration*1000 )
        {
            currentGifFrame = gifFile.getFrameAt(currentGifFrameIndex);
            lastFrameTime = ofGetElapsedTimeMillis();
            currentGifFrameIndex++;
            if( currentGifFrameIndex == gifFile.getNumFrames() )
            {
                currentGifFrameIndex = 0;
            }
        }
    }
}

void ImagePoint::draw()
{
    ofSetColor(255);
    currentGifFrame->draw(getPosition().x , getPosition().y);
}
