#include "healpixmap.h"

#include <QDebug>

HealpixMap::HealpixMap(QString path)
{
    int t, icol=0;;
    // TODO: resize faces vector to optimize performance
    // TODO: verify if file exists, etc..


    QTime timer1;
    timer1.restart();

    /* open fits file */
    QByteArray pathByteArray = path.toLocal8Bit();

    /* read healpix map */
//    /map = read_healpix_map(ba.data(), &nside, _coordsys, _ordering);


    fitsfile *fptr;
    int status=0, hducount, exttype, result, recordscount, more;

    //qDebug() << "opening fits " << path;

    /* open fits file */
    //qDebug() << pathByteArray.data();
    if(fits_open_file(&fptr, pathByteArray.data(), READONLY, &status)!=0)
    {
        qDebug("error opening fits file");
        qDebug() << " status " << status;
        return;
    }

    /* get the number of hdus in fits file */
    result = fits_get_num_hdus(fptr, &hducount, &status);
    qDebug() << "Hdus = " << hducount;
    if(result!=0)
    {
        qDebug("error on fits_get_num_hdus");
        qDebug() << " return " << result;
        qDebug() << " status " << status;
        fits_close_file(fptr, &status);
        return;
    }


    for(int i=1; i<=hducount; i++)
    {

        if(fits_movabs_hdu(fptr, i, &exttype, &status)!=0)
        {
            qDebug("error on fits_movabs_hdu");
            fits_close_file(fptr, &status);
            return;
        }

        //qDebug() << "Hdu " << i << " with type:";

        switch(exttype)
        {
            case BINARY_TBL:
                qDebug() << " binary table";
                fits_get_colnum(fptr, CASEINSEN, "TEMPERATURE", &t, &status);
                icol = t;
                break;
            case IMAGE_HDU:
                qDebug() << " image hdu"; break;
            case ASCII_TBL:
                qDebug() << " ascii table"; break;
        }

        /*
        if(fits_get_hdrspace(fptr, &recordscount, &more, &status)!=0)
        {
            qDebug("error on fits_get_hdrspace");
            fits_close_file(fptr, &status);
            return;
        }
        qDebug() << " total records = " << recordscount;

        if(true)
        {
            char record[FLEN_CARD];
            for(int j=1; j<=recordscount; j++)
            {
                if(fits_read_record(fptr, j, record, &status)!=0)
                {
                    qDebug("error on fits_read_record");
                    fits_close_file(fptr, &status);
                    return;
                }
                qDebug() << record;
            }
        }
        */
    }


    char value[24];
    char comment[80];

    /* Read Ordering */
    if(fits_read_keyword(fptr, "ORDERING", value, comment, &status)!=0)
    {
        qDebug("error on reading keyword");
        fits_close_file(fptr, &status);
        return;
    }
    /* trim string */
    this->ordering = readOrdering(value);
    qDebug() << "Ordering is " << value;

    /* Read Coordsys */
    if(fits_read_keyword(fptr, "COORDSYS", value, comment, &status)!=0)
    {
        qDebug("error on reading keyword");
        fits_close_file(fptr, &status);
        return;
    }
    /* trim string */
    this->coordsys = readCoordsys(value);
    qDebug() << "Coordsys is " << value;

    /* Read Nside */
    if(fits_read_keyword(fptr, "NSIDE", value, comment, &status)!=0)
    {
        qDebug("error on reading keyword");
        fits_close_file(fptr, &status);
        return;
    }
    QString nside_qs(value);
    this->nside = nside_qs.toInt();
    this->npixels = nside2npix(this->nside);
    qDebug() << "Nside is " << nside << " with " << npixels << " pixels";


    /* GET Temperature */
    float nul = -999.;
    temperature = new float[npixels];

    if (fits_read_col(fptr, TFLOAT, icol, 1, 1, npixels, &nul, temperature, &t, &status) != 0)
        qDebug("error reading temperature");


    /* close fits file */
    fits_close_file(fptr, &status);

    /* initialize variables */
    /*
    ordering = readOrdering(_ordering);
    coordsys = readCoordsys(_coordsys);


    qDebug() << "Healpixmap readed";
    qDebug() << " Nside = " << nside;
    qDebug() << " Npixels = " << npixels;
    qDebug() << " Ordering = " << _ordering << "(" << ordering << ")";
    qDebug() << " Coordsys = " << _coordsys << "(" << coordsys << ")";
    */


    buildTexture();

    int ms = timer1.elapsed();
    qDebug() << "Load map total = " << ms << " ms";
}


HealpixMap::Ordering HealpixMap::readOrdering(char *value)
{
    if(!strcmp(value, "RING"))
        return RING;
    else if(!strcmp(value, "NESTED"))
        return NESTED;

    // TODO: throw exception if invalid order
    else
        return RING;
}


HealpixMap::Coordsys HealpixMap::readCoordsys(char* value)
{
    if(!strcmp(value, "C"))
        return CELESTIAL;
    else if(!strcmp(value, "E"))
        return ECLIPTIC;
    else if(!strcmp(value, "G"))
        return GALACTIC;

    // TODO: throw exception if a invalid coordsys received
    else
        return GALACTIC;
}


bool HealpixMap::isOrderRing()
{
    return ordering==RING;
}

bool HealpixMap::isOrderNested()
{
    return ordering==NESTED;
}

bool HealpixMap::isCoordsysCelestial()
{
    return coordsys==CELESTIAL;
}

bool HealpixMap::isCoordsysEcliptic()
{
    return coordsys==ECLIPTIC;
}

bool HealpixMap::isCoordsysGalactic()
{
    return coordsys==GALACTIC;
}





float* HealpixMap::getTemperature()
{
    return temperature;
}

long HealpixMap::getNumberPixels()
{
    return npixels;
}

unsigned char* HealpixMap::getTexture()
{
    return texture;
}

void HealpixMap::buildTexture()
{
    double minv=-0.000145, maxv=0.000145;
    float v = 0.0;
    long texk = 0;
    QColor color, blank(255, 255, 255, 255);
    texk = 0;

    ColorTable *ct = new ColorTable(2);

    texture_res = 4*nside;
    texture = new unsigned char[texture_res*texture_res*4];

    buildLut();
    if(ordering==RING)
        lut = &lut_cache_ring[nside];
    else
        lut = &lut_cache_nest[nside];


    for(uint pix = 0; pix < npixels; pix++)
    {
        v = temperature[pix];

        if (v < minv) v = minv;
        if (v > maxv) v = maxv;
        v = (v-minv)/(maxv-minv);
        color = (*ct)(v);

        texk = (*lut)[pix];
        texture[texk++] = color.red();
        texture[texk++] = color.green();
        texture[texk++] = color.blue();
        texture[texk++] = 255;

        //if(pix<10)
        //    qDebug() << "color " << pix << ": " << color.red() << ", " << color.green() << ", " << color.blue() << ", 255";
    }


    /*
    int nside_ = 2;
    texture_res = 4*nside_;
    texture = new unsigned char[texture_res*texture_res*4];

    float inc = 0;
    for(int i=0; i<nside2npix(nside_); i++)
    {
        color = (*ct)(inc);

        texture[i*4] = color.red();
        texture[i*4+1] = color.green();
        texture[i*4+2] = color.blue();
        texture[i*4+3] = 255;


        inc+=0.02;
    }

    int i;
    */
/*
    i = 0;
    texture[i*4] = 125;
    texture[i*4+1] = 128;
    texture[i*4+2] = 50;

    i = 1;
    texture[i*4] = 0;
    texture[i*4+1] = 255;
    texture[i*4+2] = 0;

    i = 2;
    texture[i*4] = 255;
    texture[i*4+1] = 255;
    texture[i*4+2] = 0;

    i = 3;
    texture[i*4] = 0;
    texture[i*4+1] = 0;
    texture[i*4+2] = 255;

    i = 4;
    texture[i*4] = 255;
    texture[i*4+1] = 0;
    texture[i*4+2] = 255;
    */


}


void HealpixMap::drawMap()
{
    glEnable( GL_TEXTURE_2D );

    glTexImage2D(
            GL_TEXTURE_2D, 0, 4,
            texture_res, texture_res, 0,
            GL_RGBA, GL_UNSIGNED_BYTE,
            texture
    );
}

void readFitsHeader()
{

}


bool HealpixMap::buildLut()
{
    PixLUTCache &lut_cache = (ordering == RING) ? lut_cache_ring : lut_cache_nest;
    PixLUT &lut = lut_cache[nside];
    lut.resize(12*nside*nside);
    int  face = 0, x = 0, y = 0;
    long dy = 4*nside;
    long pix;
    long k = 0;
    for(face = 0; face < 12; face++)
    {
        long xo = nside*(face % 4);
        long yo = nside*(face / 4);
        long face_offset = face*nside*nside;
        for(y = 0; y < nside; y++)
        {
            for(x = 0; x < nside; x++)
            {
                k = x + xo + (y+yo)*dy;
                pix = xy2pix(x,y)  + face_offset;
                if (ordering == RING)
                    nest2ring(nside,pix,&pix);
                lut[pix] = 4*k;
            }
        }
    }
    return true;
}



long HealpixMap::xy2pix(long ix, long iy)
{
        static int x2pix[128];
        static int y2pix[128];
        static bool xy2pix_called = false;
        if( ! xy2pix_called ) {
                mk_xy2pix(x2pix, y2pix);
        xy2pix_called = true;
        }
        //int ix_low = (int)fmod(ix,128);
        int ix_low = ix % 128;
        int ix_hi  = ix / 128;
        int iy_low = iy % 128;
        int iy_hi  = iy / 128;
        return (x2pix[ix_hi]+y2pix[iy_hi]) * (128 * 128)
                        + (x2pix[ix_low]+y2pix[iy_low]);
}
