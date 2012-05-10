#ifndef ROI_H
#define ROI_H

#include <QSet>
#include <QVector>
#include <QDebug>
#include <QGLBuffer>
#include "vertice.h"
#include "facevertices.h"
#include "configs.h"
#include "pixellutcache.h"
#include <set>
#include <QMap>

using namespace std;

class ROI
{
    public:
        ROI(int faceNumber, int nside);
        ~ROI();
        void draw();
        void build();
        void unbind();
        void addPoint(long pos);
        void removePoint(long pos);

    private:
        int nside;
        int faceNumber;

        GLuint textureId;
        bool created;
        bool valuesLoaded;
        bool pixelSetChanged;
        unsigned char* mask;

        void createBuffer();
        void updateBuffer();
};

class ROIManager
{
    public:
        ROIManager();
        ~ROIManager();
        void addPoints(std::set<int> pixelIndexes, int nside);
        void removePoints(std::set<int> pixelIndexes, int nside);
        ROI* getFaceROI(int face);
        bool hasROI(int face);
        void clear();
    private:
        QMap<int, ROI*> faceROI;
        PixelLUTCache* lut_cache;
};

#endif // ROI_H
