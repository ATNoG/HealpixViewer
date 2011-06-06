#include "healpixmap.h"

#include <QDebug>

HealpixMap::HealpixMap(QString path)
{
    // TODO: verify if file exists, etc..

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
                qDebug() << " binary table"; break;
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
    qDebug() << "Nside is " << nside;

    /* close fits file */
    fits_close_file(fptr, &status);

    /* initialize variables */
    /*
    ordering = readOrdering(_ordering);
    coordsys = readCoordsys(_coordsys);
    npixels  = nside2npix(nside);

    qDebug() << "Healpixmap readed";
    qDebug() << " Nside = " << nside;
    qDebug() << " Npixels = " << npixels;
    qDebug() << " Ordering = " << _ordering << "(" << ordering << ")";
    qDebug() << " Coordsys = " << _coordsys << "(" << coordsys << ")";
    */
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

void readFitsHeader()
{

}
