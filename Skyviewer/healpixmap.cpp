#include "healpixmap.h"

#include <QDebug>
#include "fieldmap.h"


/* keywords used to specify the columns */
char  ICOLNAME[]  = "TEMPERATURE";
char  QCOLNAME1[]  = "Q_POLARISATION";
char  QCOLNAME2[] = "QPOLARISATION";
char  QCOLNAME3[] = "Q_POLARIZATION";
char  QCOLNAME4[] = "QPOLARIZATION";
char  UCOLNAME1[]  = "U_POLARISATION";
char  UCOLNAME2[] = "UPOLARISATION";
char  UCOLNAME3[] = "U_POLARIZATION";
char  UCOLNAME4[] = "UPOLARIZATION";
char  NCOLNAME[]  = "N_OBS";

char  DEFTABLE[]  = "Sky Maps";
char  DEFTUNIT[]  = "unknown";
char  DEFNUNIT[]  = "counts";
char  DEFFORM[]   = "1024E";


HealpixMap::HealpixMap(QString _path)
{
    path = _path;
    processFile(_path);

    /* current map is temperature map */
    currentMap = I;
    // TODO: verify if file exists, etc..
}


void HealpixMap::processFile(QString path)
{
    fitsfile *fptr;
    int status=0, hducount, exttype, result, t, icol=0, ncol, qcol, ucol;
    bool correctHDU;
    QByteArray pathByteArray = path.toLocal8Bit();

    /* open fits file */
    if(fits_open_file(&fptr, pathByteArray.data(), READONLY, &status)!=0)
    {
        qDebug("error opening fits file");
        qDebug() << " status " << status;
        return;
    }

    /* read primary header */
    readFITSPrimaryHeader(fptr);

    /* get the number of hdus in fits file */
    result = fits_get_num_hdus(fptr, &hducount, &status);
    qDebug() << "Hdus = " << hducount;
    if(result!=0)
    {
        qDebug("error on fits_get_num_hdus");
        qDebug() << " return " << result;
        qDebug() << " status " << status;
        fits_close_file(fptr, &status);
        // TODO: throw exception
        return;
    }

    correctHDU = false;
    for(int i=1; i<=hducount && !correctHDU; i++)
    {
        if(fits_movabs_hdu(fptr, i, &exttype, &status)!=0)
        {
            qDebug("error on fits_movabs_hdu");
            fits_close_file(fptr, &status);
            // TODO: throw exception
            return;
        }

        if(exttype==BINARY_TBL)
        {
            fits_get_colnum(fptr, CASEINSEN, ICOLNAME, &t, &status);
            if(status==0)
            {
                correctHDU = true;
                icol = t;
            }
        }
    }

    if(!correctHDU)
    {
        /* something wrong happened... invalid file ? */
        qDebug("Correct extension not found");
        fits_close_file(fptr, &status);
        return;
        /* TODO: throw exception */
    }

    /* temperature map is always available */
    availableMaps.append(I);


    /* read the extension header */
    readFITSExtensionHeader(fptr);


    /* get fields positions */

    /* verify existing maps. Both Q and U must be supplied for polarization */
    if (fits_get_colnum(fptr, CASEINSEN, NCOLNAME, &t, &status) == 0)
    {
        ncol = t;
        availableMaps.append(NObs);
    }

    // TODO: need to set status to 0 everytime ?

    status = 0;
    if (fits_get_colnum(fptr, CASEINSEN, QCOLNAME1, &t, &status) == 0) qcol = t;
    /* try the different names for Q column */
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME2, &t, &status) == 0)) qcol = t;
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME3, &t, &status) == 0)) qcol = t;
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME4, &t, &status) == 0)) qcol = t;
    /* add Q to available maps */
    if(qcol!=0)
        availableMaps.append(Q);

    status = 0;
    if (fits_get_colnum(fptr, CASEINSEN, UCOLNAME1, &t, &status) == 0) ucol = t;
    /* try the different names for U column */
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME2, &t, &status) == 0)) ucol = t;
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME3, &t, &status) == 0)) ucol = t;
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME4, &t, &status) == 0)) ucol = t;
    /* add U to available maps */
    if(ucol!=0)
        availableMaps.append(U);



    /* create map fields */

    float nul = -999.;
    temperature = new float[npixels];

    status = 0;
    if (fits_read_col(fptr, TFLOAT, icol, 1, 1, npixels, &nul, temperature, &t, &status) != 0)
    {
        qDebug("error reading temperature");
    }


    int newNside = 64;
    QTime time;
    int total = 0;

    qDebug() << "starting tests";
    //for(int i=0; i<10; i++)
    //{
        time.restart();

        /* just for test */
        FieldMap temperatureField;
        float* degraded_map = temperatureField.downgradeMap(temperature, maxNside, newNside);
        int elapsed = time.elapsed();
        qDebug() << "Total time from " << maxNside << " to " << newNside << " = " << elapsed << " ms";
        total += elapsed;
    //}
    //qDebug() << "Mean time from " << maxNside << " to " << newNside << " = " << total/10 << " ms";

    qDebug("Write to FITS");
    writeFITS("2048to64.fits", "Sky Maps", degraded_map, newNside);
    qDebug("Write done");

    /* close fits file */
    fits_close_file(fptr, &status);
}


void HealpixMap::readFITSPrimaryHeader(fitsfile *fptr)
{
    /* TODO: just check if file conforms to FITS format */
    /* some FITS file declare Ordering here ?! */
}


void HealpixMap::readFITSExtensionHeader(fitsfile *fptr)
{
    char value[24];
    char comment[80];
    int status;

    /* Read Ordering */
    status = 0;
    if(fits_read_keyword(fptr, "ORDERING", value, comment, &status)!=0)
    {
        qDebug("error on reading keyword ordering");
        fits_close_file(fptr, &status);
        // TODO: throw exception
        return;
    }
    /* trim string */
    this->ordering = parseOrdering(value);
    qDebug() << "Ordering is " << value;

    /* Read Coordsys */
    status = 0;
    if(fits_read_keyword(fptr, "COORDSYS", value, comment, &status)!=0)
    {
        qDebug("error on reading keyword coordsys");
    }
    else
    {
        /* trim string */
        this->coordsys = parseCoordsys(value);
        qDebug() << "Coordsys is " << value;
    }

    /* Read Nside */
    status = 0;
    if(fits_read_keyword(fptr, "NSIDE", value, comment, &status)!=0)
    {
        qDebug("error on reading keyword nside");
        fits_close_file(fptr, &status);
        // TODO: throw exception
        return;
    }
    /* save nside and npixels value */
    QString nside_qs(value);
    this->maxNside = nside_qs.toInt();
    this->npixels = nside2npix(this->maxNside);
    qDebug() << "Nside is " << maxNside << " with " << npixels << " pixels";
}


HealpixMap::Ordering HealpixMap::parseOrdering(char *value)
{
    if(!strcmp(value, "RING"))
        return RING;
    else if(!strcmp(value, "NESTED"))
        return NESTED;

    // TODO: throw exception if invalid order
    else
        return RING;
}


HealpixMap::Coordsys HealpixMap::parseCoordsys(char* value)
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

bool HealpixMap::hasTemperature()
{
    return availableMaps.contains(I);
}

bool HealpixMap::hasPolarization()
{
    // TODO: we need to have both ?
    return availableMaps.contains(Q) && availableMaps.contains(U);
}

bool HealpixMap::hasNObs()
{
    return availableMaps.contains(NObs);
}

void HealpixMap::drawMap()
{
    /* TODO: draw current map */
}


void HealpixMap::changeCurrentMap(MapType mapType)
{
    /* check if map exists */
    if(availableMaps.contains(mapType))
    {
        currentMap = mapType;
    }
    else
    {
        /* TODO: throw exception: maptype doesnt exist */
    }
}


FieldMap* HealpixMap::getMap(MapType mapType)
{
    /* check if map exists and returns it */
    if(availableMaps.contains(mapType))
    {
        /* TODO: return map type */
        return NULL;
    }
    else
    {
        /* TODO: throw exception */
    }
}

/* Return list of available maps in FITS file */
QList<HealpixMap::MapType> HealpixMap::getAvailableMaps()
{
    return availableMaps;
}

long HealpixMap::getNumberPixels()
{
    return npixels;
}


/* get higher resolution for the current map field */
bool HealpixMap::zoomIn()
{
    long nextNside = currentNside * 2;

    /* check if can zoom in */
    if(nextNside <= min(maxNside, long(MAXZOOM)))
    {
        /* get higher map texture resolution */
        //loadMap(nextNside);
        currentNside = nextNside;
        return true;
    }
    else
        return false;
}


/* get lower resolution for the current map field */
bool HealpixMap::zoomOut()
{
    long nextNside = currentNside / 2;

    /* check if can zoom out */
    if(nextNside >= MINZOOM)
    {
        /* get lower map texture resolution */
        //loadMap(nextNside);
        currentNside = nextNside;
        return true;
    }
    else
        return false;
}







void HealpixMap::writeFITS(char* filename, char* tabname, float* temperature, int newnside)
{
    int maxcols = 5;

    char        *ttype[maxcols], *tform[maxcols], *tunit[maxcols];
    fitsfile    *fptr;
    int          status = 0;
    int          ncol, col;
    unsigned int i;
    float       *tmp = NULL;

    /* Initialize */
    //if (strlen(tabname) <= 0) tabname = NULL;
    //if (tabname == NULL) tabname = DEFTABLE;
    ncol = 0;
    ttype[ncol] = ICOLNAME; tform[ncol] = DEFFORM; tunit[ncol++] = DEFTUNIT;


    /* Try to open and prepare the FITS file for writing. */
    if (fits_create_file(&fptr, filename, &status) == 0)
    {
        fits_create_img(fptr, LONG_IMG, 0, NULL, &status); // Create an empty primary HDU.
    }
    else
    {
        status = 0;
        fits_open_file(&fptr, filename, READWRITE, &status);
    }
    if (status != 0) qDebug("error opening fits file for write");
    writeFITSPrimaryHeader(fptr);

    /*
        Search the file for named table.  If found, remove it.
        Create a new binary table HDU and extend its header.
    */
    /*
    fits_movnam_hdu(fptr, BINARY_TBL, tabname, 0, &status);
    if ((status != 0) && (status != BAD_HDU_NUM)) qDebug("Error....");
    if (status == 0)
    {
            fits_delete_hdu(fptr, NULL, &status);
            if (status != 0) qDebug("error deleting hdu");
    }
    */
    status = 0;
    if (fits_create_tbl(fptr, BINARY_TBL, 0, ncol, ttype, tform, tunit, tabname, &status) != 0)
            qDebug("error creating table");
    writeFITSExtensionHeader(fptr, newnside);

    col = 0;

/*
                                Stokes I/temperature.
*/
    col++;
    if (fits_write_col(fptr, TFLOAT, col, 1, 1, 12*newnside*newnside, temperature, &status) != 0)
            qDebug("error writing columns");

    /* Done! */
    fits_close_file(fptr, &status);
    if (tmp != NULL) delete [] tmp;
}


void HealpixMap::writeFITSPrimaryHeader(fitsfile *fptr)
{

}



void HealpixMap::writeFITSExtensionHeader(fitsfile *fptr, int newnside)
{
    char         stmp[80], comm[80];
    unsigned int itmp;
    int          status = 0;

    strcpy(comm, "Pixel algorithm");
    strcpy(stmp, "HEALPIX");
    fits_write_key(fptr, TSTRING, "PIXTYPE", stmp, comm, &status);

    strcpy(comm, "Ordering scheme");
    strcpy(stmp, "NEST");
    fits_write_key(fptr, TSTRING, "ORDERING", stmp, comm, &status);

    strcpy(comm, "Resolution index");
    itmp = NSide2Res(newnside);
    fits_write_key(fptr, TUINT, "RESOLUTN", &itmp, comm, &status);

    strcpy(comm, "Resolution parameter");
    itmp = newnside;
    fits_write_key(fptr, TUINT, "NSIDE", &itmp, comm, &status);

    strcpy(comm, "First pixel index (0 based)");
    itmp = 0;
    fits_write_key(fptr, TUINT, "FIRSTPIX", &itmp, comm, &status);

    strcpy(comm, "Last pixel index (0 based)");
    itmp = newnside*newnside*12 - 1;
    fits_write_key(fptr, TUINT, "LASTPIX", &itmp, comm, &status);
}
