#include "roi.h"


ROIManager::ROIManager()
{
    lut_cache = PixelLUTCache::getInstance();
}

ROIManager::~ROIManager()
{

}

void ROIManager::addPoints(std::set<int> pixelIndexes, int nside)
{
    long pixelsPerFace = nside2npix(nside)/12;
    PixelLUT *lut = lut_cache->getLut(nside);

    set<int>::iterator it;
    for(it=pixelIndexes.begin(); it!=pixelIndexes.end(); it++)
    {
        /* use LUT */
        long texturepos = lut->at(*it);
        int face = texturepos / pixelsPerFace;
        long pos = texturepos % pixelsPerFace;

        ROI* roi;

        if(faceROI.contains(face))
        {
            roi = faceROI[face];
        }
        else
        {
            roi = new ROI(face, nside);
            faceROI.insert(face, roi);
        }

        roi->addPoint(pos);
    }

    pixelIndexes.clear();
}

ROI* ROIManager::getFaceROI(int face)
{
    if(hasROI(face))
        return faceROI[face];

    return NULL;
}

bool ROIManager::hasROI(int face)
{
    return faceROI.contains(face);
}

void ROIManager::clear()
{
    QMap<int, ROI*>::iterator it;
    for(it=faceROI.begin(); it!=faceROI.end(); it++)
    {
        delete it.value();
    }
    faceROI.clear();
}


ROI::ROI(int faceNumber, int nside)
{
    this->faceNumber = faceNumber;
    this->nside = nside;

    created = false;
    valuesLoaded = false;

    /* create empty array mask */
    int tot = nside*nside;
    mask = new unsigned char[tot];
    std::fill_n(mask, tot, 255);
}

ROI::~ROI()
{
    if(mask!=NULL)
    {
        delete[] mask;
    }

    glDeleteTextures(1, &textureId);
}

void ROI::addPoint(long pos)
{
    mask[pos] = 100;
    pixelSetChanged = true;
}

void ROI::build()
{

}


void ROI::createBuffer()
{
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, nside, nside, 0, GL_ALPHA, GL_UNSIGNED_BYTE, mask);
    glBindTexture(GL_TEXTURE_2D, NULL);

    created = true;
}

void ROI::updateBuffer()
{
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nside, nside, GL_ALPHA, GL_UNSIGNED_BYTE, mask);
    glBindTexture(GL_TEXTURE_2D, NULL);
}

void ROI::draw()
{
    if(!created)
        createBuffer();
    else if(pixelSetChanged)
        updateBuffer();

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, textureId);

    pixelSetChanged = false;
}


void ROI::unbind()
{
    glBindTexture(GL_TEXTURE_2D, NULL);
}
