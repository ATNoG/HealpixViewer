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

    /* get name of file */
    QFileInfo pathInfo(path);
    filename = pathInfo.fileName();

    /* creating progress dialog */
    loadingDialog = new QProgressDialog("Loading Map (reading fits info)", "Cancel", 0, 6);
    loadingDialog->setWindowModality(Qt::WindowModal);
    loadingDialog->setValue(1);

    /* Map already processed ? Check cache */
    if(!checkMapCache())
    {
        qDebug() << "Map doesnt exists on cache";
        /* creating folder on cache */
        QDir cachedDir(QString(CACHE_DIR));
        cachedDir.mkdir(filename);
        /* map not exists on cache: read FITS info and create individual maps */
        processFile(_path, true);
        /* save map info */
        writeMapInfo();
    }
    else
    {
        qDebug() << "Map already cached";
        /* read FITS information */
        processFile(_path, false);
    }

    loadingDialog->setValue(loadingDialog->maximum());

    changeCurrentMap(I);
}


void HealpixMap::processFile(QString path, bool generateMaps)
{
    // TODO: verify if file exists, etc..

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

    /* read the extension header */
    readFITSExtensionHeader(fptr);


    /* update progress */
    loadingDialog->setLabelText("Loading Map (reading available maps)");
    loadingDialog->setValue(2);


    // TODO: optimization: information about available maps can be saved on cache

    /* temperature map is always available */
    availableMaps.append(I);


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


    int MIN_NSIDE = 64;


    /* need to create the maps ? */
    if(generateMaps)
    {
        int i = 4;
        loadingDialog->setMaximum(availableMaps.size()+loadingDialog->maximum());

        foreach(MapType maptype, availableMaps)
        {
            int col;
            QString prefix;

            switch(maptype)
            {
                case I: col=icol; prefix="I"; break;
                case Q: col=qcol; prefix="Q"; break;
                case U: col=ucol; prefix="U"; break;
                case NObs: col=ncol; prefix="NObs"; break;
            }

            /* update progress */
            loadingDialog->setLabelText("Loading Map (processing " + prefix + ")");
            loadingDialog->setValue(i);
            i++;

            /* create map fields */
            float nul = -999.;
            float *values = new float[npixels];

            status = 0;
            if (fits_read_col(fptr, TFLOAT, col, 1, 1, npixels, &nul, values, &t, &status) != 0)
            {
                qDebug("error reading values from file");
            }

            qDebug() << "Pre processing map " << maptype;

            /* create field map */
            FieldMap *fieldMap = new FieldMap(values, maxNside, isOrderNested());

            // TODO: define minNside in some place...
            fieldMap->generateDowngrades(cachePath, prefix, MIN_NSIDE);

            qDebug() << "Map saved";
        }

        /* generate polarization map ? */
        if(hasPolarization())
        {
            /* create polarization map for each nside */
            for(int nside=maxNside; nside>=MIN_NSIDE; nside=nside/2)
            {
                qDebug() << "Generating polarization for " << nside;

                /* read Q and U */
                float *_Q = readMapCache(nside, Q);
                float *_U = readMapCache(nside, U);

                /* calculate total pixels */
                long totalPixels = nside2npix(nside);

                /* allocate space for polarization angles and magnitude */
                float *polarizationAng = new float[totalPixels];
                float *polarizationMag = new float[totalPixels];

                /* calculate polarization angles and magnitude */
                for(int i=0; i<totalPixels; i++)
                {
                    polarizationAng[i] = atan2(_U[i], _Q[i]) / 2;
                    polarizationMag[i] = sqrt(_Q[i]*_Q[i] + _U[i]*_U[i]);
                }

                /* calculate filename to write */
                QString nsideStr;
                nsideStr.setNum(nside);
                QString filepath = cachePath + "/P_" + nsideStr;

                /* save polarization into cache */
                QFile file(filepath);
                file.open(QIODevice::WriteOnly);
                file.write((const char*)polarizationMag, totalPixels*sizeof(float));
                file.close();
            }
        }
    }

    /* close fits file */
    fits_close_file(fptr, &status);

    loadingDialog->setValue(loadingDialog->maximum());

    //qDebug() << "Available Maps: " << availableMaps;
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
    QString aux(value);

    if(aux.contains("RING", Qt::CaseInsensitive))
        return RING;
    else if(aux.contains("NESTED", Qt::CaseInsensitive))
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




/* Return list of available maps in FITS file */
QList<HealpixMap::MapType> HealpixMap::getAvailableMaps()
{
    return availableMaps;
}

long HealpixMap::getNumberPixels()
{
    return npixels;
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



float* HealpixMap::getFaceValues(int faceNumber, int nside)
{
    int pixelsPerFace = nside2npix(nside)/12;

    float *values = readMapCache(nside, currentMapType, faceNumber*pixelsPerFace, pixelsPerFace);
    return values;
}


float* HealpixMap::getPolarizationVectors(int faceNumber, int nside)
{
    int pixelsPerFace = nside2npix(nside)/12;

    float *values = readMapCache(nside, P, faceNumber*pixelsPerFace, pixelsPerFace);
    return values;
}


float* HealpixMap::readMapCache(int nside, MapType mapType, int firstPosition, int length)
{
    QString mapTypeStr;

    switch(mapType)
    {
        case I: mapTypeStr="I"; break;
        case Q: mapTypeStr="Q"; break;
        case U: mapTypeStr="U"; break;
        case P: mapTypeStr="P"; break;
        case NObs: mapTypeStr="NObs"; break;
    }

    /* get filename */
    QString nsideStr;
    nsideStr.setNum(nside);
    QString path = cachePath + "/" + mapTypeStr + "_" + nsideStr;

    /* if length is 0 read all content */
    if(length==0)
        length = nside2npix(nside);

    float *values = new float[length];

    /* read content */
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    f.seek(firstPosition*sizeof(float));
    f.read((char*)values, length*sizeof(float));
    f.close();

    return values;
}



void HealpixMap::writeMapInfo()
{
    // save file with map info
    QFile infoFile(cacheInfo);
    infoFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&infoFile);
    out << path << endl;
    out << maxNside << endl;
    /*
    out << coordsys << endl;
    QString availableMapsStr = "";
    foreach(MapType maptype, availableMaps)
    {
        availableMapsStr.append(getMapType(maptype));
        availableMapsStr.append(",");
    }
    out << availableMapsStr << endl;
    */
    infoFile.close();
}

void HealpixMap::readMapInfo()
{

}


bool HealpixMap::checkMapCache()
{
    cachePath = QString(CACHE_DIR) + "/" + filename;
    cacheInfo = cachePath + "/info";

    QDir cachedDir(cachePath);

    //qDebug() << "Checking cached map on " << cachedDirStr;

    return false;
    return cachedDir.exists();
}


QString HealpixMap::mapTypeToString(MapType type)
{
    QString mapTypeName;
    switch(type)
    {
        case I:
            mapTypeName = "Temperature";
            break;
        case Q:
            mapTypeName = "Q Polarization";
            break;
        case U:
            mapTypeName = "U Polarization";
            break;
        case P:
            mapTypeName = "Polarization";
            break;
        case NObs:
            mapTypeName = "NObs";
            break;
    }
    return mapTypeName;
}

/*
QString HealpixMap::coordsysToString(Coordsys coordsys)
{
    QString coordsysName;
    switch(coordsys)
    {
        case CELESTIAL:
            coordsysName = "CELESTIAL";
            break;
        case ECLIPTIC:
            coordsysName = "ECLIPTIC";
            break;
        case GALACTIC:
            coordsysName = "GALACTIC";
            break;
    }
    return coordsysName;
}
*/


int HealpixMap::getMaxNside()
{
    return maxNside;
}


void HealpixMap::changeCurrentMap(MapType type)
{
    // TODO: check if map is available
    currentMapType = type;
}
