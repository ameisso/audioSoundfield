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
void ImagePoint::setup(int posX, int posY, string aPath, float aScale)
{
    path = "images/"+aPath;
    ImageObject::setup(posX, posY);
    scale = aScale;
    if( path.find(".gif") != std::string::npos )
    {
        type = IMAGE_TYPE_GIF;
#warning test decoding and throw error
        gifDecoder.decode(path);
        if( gifDecoder.decode(path) )
        {
            imageLoaded = true;
        }
        gifFile = gifDecoder.getFile();
        currentGifFrameIndex = 0;
    }
    else
    {
        type = IMAGE_TYPE_PNG;
        if( pngImage.load(path) )
        {
            imageLoaded = true;
        }
        pngImage.resize(pngImage.getWidth()*scale, pngImage.getHeight()*scale);
    }
}

void ImagePoint::update(ofVec2f listenerPosition)
{
    if( type == IMAGE_TYPE_GIF && imageLoaded )
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
    if( imageLoaded )
    {
        if(type == IMAGE_TYPE_GIF)
        {
            currentGifFrame->draw(getPosition().x , getPosition().y, currentGifFrame->getWidth()*scale, currentGifFrame->getHeight()*scale);
        }
        else
        {
            pngImage.draw(getPosition().x , getPosition().y);
        }
    }
}
