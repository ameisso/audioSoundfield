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
    if( path.find(".gif") != std::string::npos )
    {
        type = IMAGE_TYPE_GIF;
        gifDecoder.decode(path);
        gifFile = gifDecoder.getFile();
        currentGifFrameIndex = 0;
    }
    else
    {
        type = IMAGE_TYPE_PNG;
        pngImage.load(path);
    }
}

void ImagePoint::update(ofVec2f listenerPosition)
{
    if(type == IMAGE_TYPE_GIF)
    {
        if( ofGetElapsedTimeMillis()-lastFrameTime > gifFrameDuration )
        {
            currentGifFrame = gifFile.getFrameAt(currentGifFrameIndex);
            lastFrameTime = ofGetElapsedTimeMillis();
            currentGifFrameIndex++;
            gifFrameDuration = currentGifFrame->getDuration()*1000;
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
    if(type == IMAGE_TYPE_GIF)
    {
        currentGifFrame->draw(getPosition().x , getPosition().y);
    }
    else
    {
        pngImage.draw(getPosition().x , getPosition().y);
    }
}
