#include "healpixmap.h"

#include <QDebug>
#include "fieldmap.h"
#include "exceptions.h"

const double boost = 0.001;


/* keywords used to specify the columns */
char  ICOLNAME1[]  = "TEMPERATURE";
char  ICOLNAME2[]  = "Intensity";
char  ICOLNAME3[]  = "SIGNAL";
char  QCOLNAME1[]  = "Q_POLARISATION";
char  QCOLNAME2[] = "QPOLARISATION";
char  QCOLNAME3[] = "Q_POLARIZATION";
char  QCOLNAME4[] = "QPOLARIZATION";
char  QCOLNAME5[] = "QStokesParameter";
char  UCOLNAME1[]  = "U_POLARISATION";
char  UCOLNAME2[] = "UPOLARISATION";
char  UCOLNAME3[] = "U_POLARIZATION";
char  UCOLNAME4[] = "UPOLARIZATION";
char  UCOLNAME5[] = "UStokesParameter";
char  NCOLNAME[]  = "N_OBS";

char  DEFTABLE[]  = "Sky Maps";
char  DEFTUNIT[]  = "unknown";
char  DEFNUNIT[]  = "counts";
char  DEFFORM[]   = "1024E";


HealpixMap::HealpixMap(QString _path, int minNside)
{
    //throw HealpixMapException("teste");
    path = _path;
    this->minNSide = minNside;

    cacheCreated = false;
    createCache = false;

    /* get name of file */
    QFileInfo pathInfo(path);
    filename = pathInfo.fileName();

    /* creating progress dialog */
    loadingDialog = new QProgressDialog("Loading Map (reading fits info)", "Cancel", 0, 6);
    loadingDialog->setWindowModality(Qt::WindowModal);
    loadingDialog->setValue(1);

    /* check if cache folder exists */
    QDir cachedDir(QString(CACHE_DIR));

    if(!cachedDir.exists())
    {
        QDir::current().mkdir(CACHE_DIR);
    }


    /* Map already processed ? Check cache */
    if(!checkMapCache())
    {
        qDebug() << "Map doesnt exists on cache";

        createCache = true;

        /* creating folder on cache */
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
        /* read extra info */
        readMapInfo();
    }

#if DEBUG > 0
    qDebug() << "Mean nside=256 -> " << meanPolMagnitudes[256];
    qDebug() << "Dev nside=256 -> " << devPolMagnitudes[256];
#endif

    loadingDialog->setValue(loadingDialog->maximum());

    delete loadingDialog;

    changeCurrentMap(I);
}


HealpixMap::~HealpixMap()
{
    //#if DEBUG > 0
        qDebug() << "Calling HealpixMap destructor";
    //#endif
}

void HealpixMap::processFile(QString path, bool generateMaps)
{
    // TODO: verify if file exists, etc..

    fitsfile *fptr;
    int status=0, hducount, exttype, result, t, icol=0, ncol=0, qcol=0, ucol=0;
    bool correctHDU;
    QByteArray pathByteArray = path.toLocal8Bit();

    /* open fits file */
    if(fits_open_file(&fptr, pathByteArray.data(), READONLY, &status)!=0)
    {
        #if DEBUG > 0
            qDebug("error opening fits file");
            qDebug() << " status " << status;
        #endif
        abort();
        throw HealpixMapException("Error opening FITS file");
    }

    /* read primary header */
    readFITSPrimaryHeader(fptr);

    /* get the number of hdus in fits file */
    result = fits_get_num_hdus(fptr, &hducount, &status);

    if(result!=0)
    {
        #if DEBUG > 0
            qDebug("error on fits_get_num_hdus");
            qDebug() << " return " << result;
            qDebug() << " status " << status;
        #endif
        fits_close_file(fptr, &status);
        abort();
        throw HealpixMapException("Error processing FITS file");
    }

    correctHDU = false;
    for(int i=1; i<=hducount && !correctHDU; i++)
    {
        if(fits_movabs_hdu(fptr, i, &exttype, &status)!=0)
        {
            #if DEBUG > 0
                qDebug("error on fits_movabs_hdu");
            #endif
            fits_close_file(fptr, &status);
            abort();
            throw HealpixMapException("Error processing FITS file");
        }

        #if DEBUG > 0
            switch(exttype)
            {
                case BINARY_TBL:
                    qDebug("extension binary table");
                    break;
                case IMAGE_HDU:
                    qDebug("extension image hdu");
                    break;
                case ASCII_TBL:
                    qDebug("extension ascii table");
                    break;
            }
        #endif

        if(exttype==BINARY_TBL)
        {
            /*
            char TESTE[]  = "*";
            char colname[20];
            int totalcols;

            fits_get_num_cols(fptr, &totalcols, &status);
            qDebug() << "Total columns = " << totalcols;

            for(int i=0; i<totalcols; i++)
            {
                fits_get_colname(fptr, CASEINSEN, TESTE, colname, &t, &status);
                qDebug() << "Column name: " << colname << ", Column n: " << t;
            }
            */

            if (fits_get_colnum(fptr, CASEINSEN, ICOLNAME1, &t, &status) == 0) icol = t;
            status = 0;
            if ((icol == 0) && (fits_get_colnum(fptr, CASEINSEN, ICOLNAME2, &t, &status) == 0)) icol = t;
            status = 0;
            if ((icol == 0) && (fits_get_colnum(fptr, CASEINSEN, ICOLNAME3, &t, &status) == 0)) icol = t;

            if(icol!=0)
            {
                correctHDU = true;
            }
        }
    }

    if(!correctHDU)
    {
        /* something wrong happened... invalid file ? */
        #if DEBUG > 0
            qDebug("Correct extension not found");
        #endif
        fits_close_file(fptr, &status);
        abort();
        throw HealpixMapException("Error processing FITS file: field not found");
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

    /* Look for Q field */
    status = 0;
    if (fits_get_colnum(fptr, CASEINSEN, QCOLNAME1, &t, &status) == 0) qcol = t;
    /* try the different names for Q column */
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME2, &t, &status) == 0)) qcol = t;
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME3, &t, &status) == 0)) qcol = t;
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME4, &t, &status) == 0)) qcol = t;
    status = 0;
    if ((qcol == 0) && (fits_get_colnum(fptr, CASEINSEN, QCOLNAME5, &t, &status) == 0)) qcol = t;
    /* add Q to available maps */
    if(qcol!=0)
        availableMaps.append(Q);

    /* Look for U field */
    status = 0;
    if (fits_get_colnum(fptr, CASEINSEN, UCOLNAME1, &t, &status) == 0) ucol = t;
    /* try the different names for U column */
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME2, &t, &status) == 0)) ucol = t;
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME3, &t, &status) == 0)) ucol = t;
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME4, &t, &status) == 0)) ucol = t;
    status = 0;
    if ((ucol == 0) && (fits_get_colnum(fptr, CASEINSEN, UCOLNAME5, &t, &status) == 0)) ucol = t;
    /* add U to available maps */
    if(ucol!=0)
        availableMaps.append(U);


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
            float *values;

            try
            {
                values = new float[npixels];
            }
            catch(const std::bad_alloc &)
            {
                fits_close_file(fptr, &status);
                abort();
                throw HealpixMapException("Not enough memory");
            }

            status = 0;
            if (fits_read_col(fptr, TFLOAT, col, 1, 1, npixels, &nul, values, &t, &status) != 0)
            {
                #if DEBUG > 0
                    qDebug("error reading values from file");
                #endif
                status = 0;
                fits_close_file(fptr, &status);
                abort();
                throw HealpixMapException("Error reading values from FITS file");
            }

            /* create field map */
            FieldMap *fieldMap = NULL;
            try
            {
                 fieldMap = new FieldMap(values, maxNside, isOrderNested());
                 fieldMap->generateDowngrades(cachePath, prefix, minNSide);
            }
            catch(FieldMapException &e)
            {
                status = 0;
                fits_close_file(fptr, &status);
                abort();
                delete[] values;
                throw HealpixMapException(e.what());
            }

            // TODO: define minNside in some place...
            delete fieldMap;
        }

        /* generate polarization map ? */
        if(hasPolarization())
        {
            /* create polarization map for each nside */
            for(int nside=maxNside; nside>=minNSide; nside=nside/2)
            {
                //qDebug() << "Generating polarization for " << nside;

                /* read Q and U */
                float *_Q = readMapCache(nside, Q);
                float *_U = readMapCache(nside, U);

                /* calculate total pixels */
                long totalPixels = nside2npix(nside);

                /* allocate space for polarization angles and magnitude */
                float *polarizationAng = new float[totalPixels];
                float *polarizationMag = new float[totalPixels];

                /* calculate polarization angles and magnitude */
                float maxPolMagnitude = 0.0;
                float mean = 0.0;

                for(int i=0; i<totalPixels; i++)
                {
                    polarizationAng[i] = atan2(_U[i], _Q[i]) / 2;
                    polarizationMag[i] = sqrt(_Q[i]*_Q[i] + _U[i]*_U[i]);

                    mean+=polarizationMag[i];

                    if(polarizationMag[i]>maxPolMagnitude)
                        maxPolMagnitude = polarizationMag[i];
                }

                /* calculate standard deviation */
                mean = mean/totalPixels;
                float aux = 0.0;
                for(int i=0; i<totalPixels; i++)
                {
                    aux += pow(polarizationMag[i]-mean, 2);
                }

                maxPolMagnitudes[nside] = maxPolMagnitude;
                meanPolMagnitudes[nside] = mean;
                devPolMagnitudes[nside] = sqrt(aux/(totalPixels-1));

                #if DEBUG > 0
                    qDebug() << "Media para nside = " << nside << " -> " << meanPolMagnitudes[nside];
                    qDebug() << "Max -> " << maxPolMagnitudes[nside];
                    qDebug() << "Desvio padrao -> " << devPolMagnitudes[nside];
                #endif

                /* calculate filename to write */
                QString nsideStr;
                nsideStr.setNum(nside);
                QString filepath = cachePath + "/P_" + nsideStr;

                /* save polarization into cache */
                QFile file(filepath);
                if(file.open(QIODevice::WriteOnly))
                {
                    file.write((const char*)polarizationAng, totalPixels*sizeof(float));
                    file.write((const char*)polarizationMag, totalPixels*sizeof(float));
                    file.close();
                }
                else
                {
                    abort();
                    throw HealpixMapException("Error writing into cache");
                }

                delete polarizationAng;
                delete polarizationMag;
            }

            qDebug() << "Cache written with success";
        }

        cacheCreated = true;
    }

    /* close fits file */
    fits_close_file(fptr, &status);

    loadingDialog->setValue(loadingDialog->maximum());
}


void HealpixMap::readFITSPrimaryHeader(fitsfile *)
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
        #if DEBUG > 0
            qDebug("error on reading keyword ordering");
        #endif

        status = 0;
        fits_close_file(fptr, &status);
        abort();
        throw HealpixMapException("Can't get Healpix ordering schema from FITS file");
    }
    /* trim string */
    this->ordering = parseOrdering(value);

    /* Read Coordsys */
    status = 0;
    if(fits_read_keyword(fptr, "COORDSYS", value, comment, &status)!=0)
    {
        #if DEBUG > 0
            qDebug("error on reading keyword coordsys");
        #endif
    }
    else
    {
        /* trim string */
        this->coordsys = parseCoordsys(value);
    }

    /* Read Nside */
    status = 0;
    if(fits_read_keyword(fptr, "NSIDE", value, comment, &status)!=0)
    {
        #if DEBUG > 0
            qDebug("error on reading keyword nside");
        #endif

        status = 0;
        fits_close_file(fptr, &status);
        abort();
        throw HealpixMapException("Cant't get the Nside from FITS file");
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
    else
    {
        abort();
        throw HealpixMapException("Invalid Healpix ordering");
    }
}


HealpixMap::Coordsys HealpixMap::parseCoordsys(char* value)
{
    if(!strcmp(value, "C"))
        return CELESTIAL;
    else if(!strcmp(value, "E"))
        return ECLIPTIC;
    else if(!strcmp(value, "G"))
        return GALACTIC;
    else
        return CELESTIAL;
        //throw HealpixMapException("Invalid coordsys in FITS file");
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
    //unsigned int i;
    float       *tmp = NULL;

    /* Initialize */
    //if (strlen(tabname) <= 0) tabname = NULL;
    //if (tabname == NULL) tabname = DEFTABLE;
    ncol = 0;
    ttype[ncol] = ICOLNAME1; tform[ncol] = DEFFORM; tunit[ncol++] = DEFTUNIT;


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


void HealpixMap::writeFITSPrimaryHeader(fitsfile *)
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



float* HealpixMap::getFullMap(int nside)
{
    return readMapCache(nside, currentMapType, 0, nside2npix(nside));
}


float* HealpixMap::getFaceValues(int faceNumber, int nside)
{
    int pixelsPerFace = nside2npix(nside)/12;

    float *values = readMapCache(nside, currentMapType, faceNumber*pixelsPerFace, pixelsPerFace);
    return values;
}


float* HealpixMap::getPolarizationVectors(int faceNumber, int nside, double minMag, double maxMag, double magnification, int spacing, long &totalVectors)
{
    //qDebug() << "Getting polarization vectors with magnification: " << magnification;
    int pixelsPerFace = nside2npix(nside)/12;

    long startPixel = faceNumber*pixelsPerFace;

    float *polAngles = readMapCache(nside, P, startPixel, pixelsPerFace);
    float *polMagnitudes = readMapCache(nside, P, (12+faceNumber)*pixelsPerFace, pixelsPerFace);

    float *nobs = NULL;
    if(hasNObs())
        nobs = readMapCache(nside, NObs, startPixel, pixelsPerFace);

    long npixels = pixelsPerFace;

    /* calculate number of pixels with observations */
    if(hasNObs())
    {
        for(long i=0; i<pixelsPerFace; i++)
        {
            if(nobs[i]==0)
                npixels--;
        }
    }    

    int spacingDivisor;

    if(spacing==0)
        spacingDivisor = 1;
    else
        spacingDivisor = spacing*spacing/2;

    totalVectors = npixels/spacingDivisor;

    /* allocate space (each vector will have 2 endpoints, of 3 coordinates each */
    float* polVectors;
    try
    {
        polVectors = new float[totalVectors*3*2];
    }
    catch(const std::bad_alloc &)
    {
        delete[] nobs;
        delete[] polAngles;
        delete[] polMagnitudes;
        throw HealpixMapException("Not enough memory");
    }

    // TODO: what is this pixsize ?
    double pixsize = (sqrt(M_PI/3.) / nside) / 2.;

    double theta, phi;
    long pointer = 0;


    /* pixel number */
    long pixNest;


    if(minMag==-1 && maxMag==-1)
    {
        getMagMinMax(minMag, maxMag);
    }

    long faceOffset = faceNumber*nside*nside;

    /* get max polarization magnitude */
    for(long i=0; i<pixelsPerFace; i++)
    {
        if(hasNObs() && nobs[i]<=0)
            break;

        int x = i%nside;
        int y = i/nside;

        bool usePixel = false;

        if(spacing==0)
            usePixel = true;
        else if((x%spacing==0 && y%spacing==0) || ((x+spacing/2)%spacing==0 && (y+spacing/2)%spacing==0))
            usePixel = true;
        /*

                if((y%spacing==0 && x%spacing==0) || ((x+2)%4==0 && (y+2)%4==0))
                    usePixel = true;
                break;
        }
        */

        if(usePixel)
        {
            /* convert first from texture position to nest! */
            pixNest = xy2pix(x,y) + faceOffset;

            pix2ang_nest(nside, pixNest, &theta, &phi);

            float* vector;
            try
            {
                vector = calculatePolarizationVector(theta, phi, polAngles[i], polMagnitudes[i], pixsize, minMag, maxMag, magnification);
                //float* vector = calculatePolarizationVector(theta, phi, polAngles[i], polMagnitudes[i], pixsize, (double)(meanPolMagnitudes[nside]-devPolMagnitudes[nside]), (double)(meanPolMagnitudes[nside]+devPolMagnitudes[nside]), magnification);
            }
            catch(const std::bad_alloc &)
            {
                delete[] nobs;
                delete[] polAngles;
                delete[] polMagnitudes;
                delete[] polVectors;
                throw HealpixMapException("Not enough memory");
            }

            for(int j=0; j<6; j++)
            {
                polVectors[pointer] = vector[j];
                pointer++;
            }
            delete[] vector;
        }
    }

    if(nobs!=NULL)
        delete[] nobs;
    delete[] polAngles;
    delete[] polMagnitudes;

    return polVectors;
}


float* HealpixMap::calculatePolarizationVector(double theta, double phi, double angle, double mag, double pixsize, double minMag, double maxMag, double magnification)
{
    //if()
    //{
        //qDebug() << "Using magnitude thresholds!";
        if(mag>maxMag)
            mag = maxMag;
        else if(mag<minMag)
            mag = minMag;
    //}

    //qDebug() << "Minmag " << minMag;
    double size = (mag-minMag) * ((pixsize/2)/(maxMag-minMag)) + pixsize/2;
    size*=magnification;
    //qDebug() << "pixsize = " << pixsize;
    //qDebug() << "size = " << size;
    Vec v0(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
    theta -= M_PI/2;
    Vec vin(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
    Vec v1 = (1+boost)*v0 + size*spinVector(v0,vin,angle);
    Vec v2 = (1+boost)*v0 + size*spinVector(v0,vin,angle+M_PI);

    float* coords = new float[6];
    coords[0] = v1.x;
    coords[1] = v1.y;
    coords[2] = v1.z;
    coords[3] = v2.x;
    coords[4] = v2.y;
    coords[5] = v2.z;
    return coords;
}


Vec HealpixMap::spinVector(const Vec &v0, const Vec &vin, double psi)
{
        double e1,e2,e3;
        double A[3][3];

        e1 = v0[0];	e2 = v0[1];	e3 = v0[2];
        double cosp = cos(psi);
        double sinp = sin(psi);

        A[0][0] = cosp + e1*e1*(1-cosp); 	A[0][1] = e1*e2*(1-cosp)+e3*sinp;	A[0][2] = e1*e3*(1-cosp)-e2*sinp;
        A[1][0] = e1*e2*(1-cosp)-e3*sinp;	A[1][1] = cosp+e2*e2*(1-cosp);		A[1][2] = e2*e3*(1-cosp)+e1*sinp;
        A[2][0] = e1*e3*(1-cosp)+e2*sinp;	A[2][1] = e2*e3*(1-cosp)-e1*sinp;	A[2][2] = cosp+e3*e3*(1-cosp);

        Vec vout;
        for(int i = 0; i < 3; i++) {
                vout[i] = 0;
                for(int j = 0; j < 3; j++)
                        vout[i] += A[i][j]*vin[j];
        }
        return vout;
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

    hv::unique_array<float> values(new float[length]);

    //qDebug() << "Reading file " << path;

    /* read content */
    QFile f(path);

    cacheAccess.lock();
    if(f.open(QIODevice::ReadOnly))
    {
        f.seek(firstPosition*sizeof(float));
        f.read((char*)values.get(), length*sizeof(float));
    }
    else
    {
        cacheAccess.unlock();
        abort();
        throw HealpixMapException("Error reading cache");
    }
    f.close();
    cacheAccess.unlock();

    return values.release();
}



void HealpixMap::writeMapInfo()
{
    // save file with map info
    QFile infoFile(cacheInfo);

    if(infoFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&infoFile);
        out << path << endl;
        out << maxNside << endl;

        /* write polarization magnitudes - means */
        out << "[polmeans]" << endl;
        QMapIterator<int, float> i(meanPolMagnitudes);
        while (i.hasNext())
        {
            i.next();
            out << i.key() << ":" << i.value() << endl;
        }
        out << "[/polmeans]" << endl;

        /* write polarization magnitudes - standar deviation */
        out << "[poldeviations]" << endl;
        QMapIterator<int, float> j(devPolMagnitudes);
        while (j.hasNext())
        {
            j.next();
            out << j.key() << ":" << j.value() << endl;
        }
        out << "[/poldeviations]" << endl;

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
    else
    {
        abort();
        throw HealpixMapException("Error writing map information into cache: check if directory have write permissions");
    }
}

void HealpixMap::readMapInfo()
{
    /* open file */
    QFile infoFile(cacheInfo);

    if(infoFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&infoFile);

        QString line;
        bool readingMeans = false;
        bool readingDeviations = false;

        /* read polarization magnitude means */
        do
        {
            line = in.readLine();

            if(!readingMeans && !readingDeviations)
            {
                if(line=="[polmeans]")
                    readingMeans = true;
                else if(line=="[poldeviations]")
                    readingDeviations = true;
            }
            else
            {
                /* check stop */
                if(line=="[/polmeans]")
                    readingMeans = false;
                else if(line=="[/poldeviations]")
                    readingDeviations = false;
                else
                {
                    QStringList aux = line.split(":");
                    int nside = aux[0].toInt();
                    float value = aux[1].toFloat();
                    if(readingMeans)
                        meanPolMagnitudes[nside] = value;
                    else
                        devPolMagnitudes[nside] = value;
                }
            }
        }while(!line.isNull());
    }
    else
    {
        abort();
        throw HealpixMapException("Error reading map information from cache");
    }
}


bool HealpixMap::checkMapCache()
{
    cachePath = QString(CACHE_DIR) + "/" + filename;
    cacheInfo = cachePath + "/info";

    QDir cachedDir(cachePath);

    //qDebug() << "Checking cached map on " << cachedDirStr;

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

    /* regenerate min and max values */
    hv::unique_array<float> values(getFullMap(minNSide));
    hv::unique_ptr<Histogram> histo(new Histogram(values.move(), nside2npix(minNSide)));
    histo->getMinMax(min, max);
}


HealpixMap::MapType HealpixMap::getCurrentMapField()
{
    return currentMapType;
}


void HealpixMap::getMinMax(float &_min, float &_max)
{
    _min = min;
    _max = max;
}


void HealpixMap::getMagMinMax(double &min, double &max)
{
    min = (double)(meanPolMagnitudes[128]-devPolMagnitudes[128]);
    max = (double)(meanPolMagnitudes[128]+devPolMagnitudes[128]);
}


void HealpixMap::removeCache()
{
    qDebug() << "Removing cache: " << cachePath;

    /* remove cache folder */
    QDir cachedDir(cachePath);
    QDir cacheDir(QString(CACHE_DIR));

    //First delete any files in the current directory
    QFileInfoList files = cachedDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    for(int file = 0; file < files.count(); file++)
    {
        cachedDir.remove(files.at(file).fileName());
    }

    //Finally, remove empty parent directory
    cacheDir.rmdir(filename);
}


void HealpixMap::abort()
{
    qDebug() << "Aborting FITS processing";
    if(createCache && !cacheCreated)
        removeCache();

    delete loadingDialog;
}

void HealpixMap::angle2pix(double theta, double phi, int nside, long &pix)
{
    pix = 0;

    /*
    theta *= deg2rad;
    phi   *= deg2rad;
    */

    switch(ordering)
    {
        case NESTED:
            ang2pix_nest(long(nside), theta, phi, &pix);
            break;
        case RING:
            ang2pix_ring(long(nside), theta, phi, &pix);
            break;
    }
}

std::set<int> HealpixMap::query_disc(int pixel1, int pixel2, int nside)
{
    double radius;
    std::vector<int> listPix;

    Healpix_Base* hp = new Healpix_Base(nside, NEST , SET_NSIDE);

    vec3 v1 = hp->pix2vec(pixel1);
    vec3 v2 = hp->pix2vec(pixel2);
    radius = acos(dotprod(v1, v2));

    hp->query_disc(hp->pix2ang(pixel1), radius, listPix);

    std::set<int> result;
    result.insert(listPix.begin(), listPix.end());

    delete hp;

    return result;
}

std::set<int> HealpixMap::query_triangle(int pixel1, int pixel2, int pixel3, int nside)
{
    std::set<int> result;

    Healpix_Base* hp = new Healpix_Base(nside, NEST , SET_NSIDE);

    long npix, iz, irmin, irmax, n12, n123a, n123b, ndom = 0;
    bool test1, test2, test3;
    double dth1, dth2, determ, sdet;
    double zmax, zmin, z1max, z1min, z2max, z2min, z3max, z3min;
    double z, tgth, st, offset, sin_off;
    double phi_pos, phi_neg;
    std::vector<vec3> vv(3);
    std::vector<vec3> vo(3);
    double sprod[3];
    double sto[3];
    double phi0i[3];
    double tgthi[3];
    double dc[3];
    double dom[3][2];
    double dom12[4];
    double dom123a[4];
    double dom123b[4];
    double alldom[6];
    double a_i, b_i, phi0, dphiring;
    long idom;
    long nsidesq = nside * nside;

    double M_PI2 = 2*M_PI;

    npix = nside2npix(nside);

    bool do_nest = true;

    vv[0] = hp->pix2vec(pixel1);
    vv[1] = hp->pix2vec(pixel2);
    vv[2] = hp->pix2vec(pixel3);

    dth1 = 1.0 / ( 3.0 * nsidesq );
    dth2 = 2.0 / ( 3.0 * nside );

    /*
     * determ = (v1 X v2) . v3 determines the left ( <0) or right (>0)
     * handedness of the triangle
     */

    vec3 vt = crossprod(vv[0], vv[1]);
    determ = dotprod(vt, vv[2]);

    if(abs(determ) < 1.0e-20)
        qDebug("error: the triangle is degenerated");

    if(determ >= 0.)
        sdet = 1.0;
    else
        sdet = -1.0;

    sprod[0] = dotprod(vv[1], vv[2]);
    sprod[1] = dotprod(vv[2], vv[0]);
    sprod[2] = dotprod(vv[0], vv[1]);
    /* vector ortogonal to the great circle containing the vertex doublet */

    vo[0] = crossprod(vv[1], vv[2]);
    vo[1] = crossprod(vv[2], vv[0]);
    vo[2] = crossprod(vv[0], vv[1]);
    vo[0].Normalize();
    vo[1].Normalize();
    vo[2].Normalize();

    /* test presence of poles in the triangle */
    zmax = -1.0;
    zmin = 1.0;
    test1 = ( vo[0].z * sdet >= 0.0 ); // north pole in hemisphere
    // defined
    // by
    // 2-3
    test2 = ( vo[1].z * sdet >= 0.0 ); // north pole in the hemisphere
    // defined
    // by 1-2
    test3 = ( vo[2].z * sdet >= 0.0 ); // north pole in hemisphere
    // defined
    // by
    // 1-3
    if ( test1 && test2 && test3 )
            zmax = 1.0; // north pole in the triangle
    if ( ( !test1 ) && ( !test2 ) && ( !test3 ) )
            zmin = -1.0; // south pole in the triangle
    /* look for northenest and southernest points in the triangle */
    // ! look for northernest and southernest points in the triangle
    // ! node(1,2) = vector of norm=1, in the plane defined by (1,2) and
    // with z=0

    bool test1a = ((vv[2].z - sprod[0] * vv[1].z) >= 0.0); //
    bool test1b = ((vv[1].z - sprod[0] * vv[2].z) >= 0.0);
    bool test2a = ((vv[2].z - sprod[1] * vv[0].z) >= 0.0); //
    bool test2b = ((vv[0].z - sprod[1] * vv[2].z) >= 0.0);
    bool test3a = ((vv[1].z - sprod[2] * vv[0].z) >= 0.0); //
    bool test3b = ((vv[0].z - sprod[2] * vv[1].z) >= 0.0);

    /* sin of theta for orthogonal vector */
    for ( int i = 0; i < 3; i++ ) {
        sto[i] = sqrt(( 1.0 - vo[i].z ) * ( 1.0 + vo[i].z ));
    }

    /*
     * for each segment ( side of the triangle ) the extrema are either -
     * -the 2 vertices
     * - one of the vertices and a point within the segment
     */
    z1max = vv[1].z;
    z1min = vv[2].z;
    double zz;

    // segment 2-3
    if ( test1a == test1b )
    {
        zz = sto[0];
        if ( ( vv[1].z + vv[2].z ) >= 0.0 )
        {
            z1max = zz;
        }
        else
        {
            z1min = -zz;
        }
    }
    // segment 1-3
    z2max = vv[2].z;
    z2min = vv[0].z;
    if ( test2a == test2b )
    {
        zz = sto[1];
        if ( ( vv[0].z + vv[2].z ) >= 0.0 )
        {
            z2max = zz;
        }
        else
        {
            z2min = -zz;
        }
    }
    // segment 1-2
    z3max = vv[0].z;
    z3min = vv[1].z;
    if ( test3a == test3b )
    {
        zz = sto[2];
        if ( ( vv[0].z + vv[1].z ) >= 0.0 )
        {
            z3max = zz;
        }
        else
        {
            z3min = -zz;
        }
    }


    zmax = std::max(std::max(z1max, z2max), std::max(z3max, zmax));
    zmin = std::min(std::min(z1min, z2min), std::min(z3min, zmin));

    offset = 0.0;
    sin_off = 0.0;

    irmin = ringNum(nside, zmax);
    irmax = ringNum(nside, zmin);

    /* loop on the rings */
    for ( int i = 0; i < 3; i++ )
    {
        tgthi[i] = -1.0e30 * vo[i].z;
        phi0i[i] = 0.0;
    }
    for ( int j = 0; j < 3; j++ )
    {
        if ( sto[j] > 1.0e-10 )
        {
            tgthi[j] = -vo[j].z / sto[j]; // - cotan(theta_orth)

            phi0i[j] = atan2(vo[j].y, vo[j].x); // Should make
            // it
            // 0-2pi
            // ?
            /* Bring the phi0i to the [0,2pi] domain if need */

            if ( phi0i[j] < 0.)
            {
                phi0i[j] = MODULO(( atan2(vo[j].y, vo[j].x) + M_PI2 ), M_PI2); // [0-2pi]
            }
        }
    }

    // ---------------------------------------

    //MOD(ATAN2(X,Y) + TWOPI, TWOPI) : ATAN2 in 0-2pi
    /*
     * the triangle boundaries are geodesics: intersection of the sphere
     * with plans going through (0,0,0) if we are inclusive, the boundaries
     * are the intersection of the sphere with plains pushed outward by
     * sin(offset)
     */
    bool found = false;
    for ( iz = irmin; iz <= irmax; iz++ )
    {
        found = false;
        if ( iz <= nside - 1 ) { // North polar cap
            z = 1.0 - iz * iz * dth1;
        } else if ( iz <= 3 * nside ) { // tropical band + equator
            z = ( 2.0 * nside - iz ) * dth2;
        } else {
            z = -1.0 + ( 4.0 * nside - iz ) * ( 4.0 * nside - iz ) * dth1;
        }

        /* computes the 3 intervals described by the 3 great circles */
        st = sqrt(( 1.0 - z ) * ( 1.0 + z ));
        tgth = z / st; // cotan(theta_ring)
        for ( int j = 0; j < 3; j++ )
        {
            dc[j] = tgthi[j] * tgth - sdet * sin_off / ( ( sto[j] + 1.0e-30 ) * st ) ;
        }
        for ( int k = 0; k < 3; k++ )
        {
            if ( dc[k] * sdet <= -1.0 ) { // the whole iso-latitude ring
                // is on
                // right side of the great circle
                dom[k][0] = 0.0;
                dom[k][1] = M_PI2;
            } else if ( dc[k] * sdet >= 1.0 ) { // all on the wrong side
                dom[k][0] = -1.000001 * ( k + 1 );
                dom[k][1] = -1.0 * ( k + 1 );
            } else { // some is good some is bad
                phi_neg = phi0i[k] - ( acos(dc[k]) * sdet );
                phi_pos = phi0i[k] + ( acos(dc[k]) * sdet );
                //
                 if ( phi_pos < 0. )
                        phi_pos += M_PI2;
                if ( phi_neg < 0. )
                        phi_neg += M_PI2;
                //
                dom[k][0] = MODULO(phi_neg, M_PI2);
                dom[k][1] = MODULO(phi_pos, M_PI2);
            }
        }

        /* identify the intersections (0,1,2 or 3) of the 3 intervals */

        int dom12length, dom123alength, dom123blength;

        dom12length = 0;
        intrs_intrv(dom12, dom[0], dom[1], dom12length);

        n12 = dom12length / 2;
        if ( n12 != 0 )
        {
            if ( n12 == 1 )
            {
                dom123alength = 0;
                intrs_intrv(dom123a, dom[2], dom12, dom123alength);
                n123a = dom123alength / 2;

                if ( n123a == 0 )
                    found = true;
                if ( !found )
                {
                    for ( int l = 0; l < dom123alength; l++ )
                    {
                        alldom[l] = dom123a[l];
                    }

                    ndom = n123a; // 1 or 2
                }
            }
            if ( !found )
            {
                if ( n12 == 2 )
                {
                    dom123alength = 0;
                    dom123blength = 0;
                    double tmp[] = { dom12[0], dom12[1] };
                    intrs_intrv(dom123a, dom[2], tmp, dom123alength);
                    double tmp1[] = { dom12[2], dom12[3] };
                    intrs_intrv(dom123b, dom[2], tmp1, dom123blength);
                    n123a = dom123alength / 2;
                    n123b = dom123blength / 2;
                    ndom = n123a + n123b; // 0, 1, 2 or 3

                    if ( ndom == 0 )
                        found = true;
                    if ( !found )
                    {
                        if ( n123a != 0 )
                        {
                            for ( int l = 0; l < 2 * n123a; l++ )
                            {
                                alldom[l] = dom123a[l];
                            }
                        }
                        if ( n123b != 0 )
                        {
                            for ( int l = 0; l < 2 * n123b; l++ )
                            {
                                int x = (int) ( l + 2 * n123a );
                                alldom[x] = dom123b[l];
                            }
                        }
                        if ( ndom > 3 )
                        {
                            qDebug("Error: too many intervals found");
                        }
                    }
                }
            }
            if ( !found )
            {
                for ( idom = 0; idom < ndom; idom++ )
                {
                    a_i = alldom[(int) ( 2 * idom )];
                    b_i = alldom[(int) ( 2 * idom + 1 )];
                    phi0 = ( a_i + b_i ) * 0.5;
                    dphiring = (b_i - a_i) * 0.5;
                    if ( dphiring < 0.0 )
                    {
                        phi0 += M_PI;
                        dphiring += M_PI;
                    }
                    /* finds pixels in the triangle on that ring */
//						listir = inRing( iz, phi0, dphiring, do_nest);
//						ArrayList<Long> listir2 = InRing(nside, iz, phi0, dphiring, do_nest);
//						res.addAll(listir);


                    std::set<int> aux;
                    aux = inRing(iz, phi0, dphiring, nside);

                    //qDebug() << "inRing(" << iz << "," << phi0 << "," << dphiring << ") - " << aux.size();

                    if(do_nest)
                    {
                        std::set<int>::iterator it;
                        long ipixNest;
                        for(it=aux.begin(); it!=aux.end(); it++)
                        {
                            ring2nest(nside, *it, &ipixNest);
                            result.insert(ipixNest);
                        }
                    }
                    else
                        result.insert(aux.begin(), aux.end());
                }
            }
        }
    }

    return result;
}

std::set<int> HealpixMap::query_polygon(std::vector<int> points, int nside)
{
    std::set<int> res;
    std::vector<vec3> vectors;

    int nv = points.size();

    Healpix_Base* hp = new Healpix_Base(nside, NEST , SET_NSIDE);

    for(int i=0; i<nv; i++)
    {
        vec3 v = hp->pix2vec(points.at(i));
        vectors.push_back(v);
    }

    vec3 vp0, vp1, vp2, vo;
    int p0, p1, p2;
    double hand;
    std::vector<double> ss;
    ss.resize(nv);
    long npix;
    int ix = 0;
    int n_remain, np, nm, nlow;

    // Start polygon
    for ( int k = 0; k < nv; k++ )
        ss[k] = 0.;
    /* -------------------------------------- */
    n_remain = nv;
    if ( n_remain < 3 )
    {
        qDebug(" Number of vertices should be >= 3");
        return res;
    }

    /*---------------------------------------------------------------- */
    /* Check that the poligon is convex or has only one concave vertex */
    /*---------------------------------------------------------------- */
    int i0 = 0;
    int i2 = 0;
    if ( n_remain > 3 )
    {
        // a triangle is always convex
        for ( int i1 = 1; i1 <= n_remain - 1; i1++ )
        {
            // in [0,n_remain-1]
            i0 = (int) MODULO(i1 - 1, n_remain);
            i2 = (int) MODULO(i1 + 1, n_remain);
            vp0 = vectors.at(i0); // select vertices by 3
            // neighbour
            vp1 = vectors.at(i1);
            vp2 = vectors.at(i2);
            // computes handedness (v0 x v2) . v1 for each vertex v1
            vo = crossprod(vp0, vp2);
            hand = dotprod(vo, vp1);
            if ( hand >= 0. )
                ss[i1] = 1.0;
            else
                ss[i1] = -1.0;
        }
        np = 0; // number of vert. with positive handedness
        for ( int i = 0; i < nv; i++ )
        {
            if ( ss[i] > 0. )
                np++;
        }
        nm = n_remain - np;

        nlow = std::min(np, nm);

        if ( nlow != 0 )
        {
            if ( nlow == 1 )
            {
                // only one concave vertex
                if ( np == 1 )
                {
                    // ix index of the vertex in the list
                    for ( int k = 0; k < nv - 1; k++ )
                    {
                        if ( abs(ss[k] - 1.0) <= 1.e-12 )
                        {
                            ix = k;
                            break;
                        }
                    }
                }
                else
                {
                    for ( int k = 0; k < nv - 1; k++ )
                    {
                        if ( abs(ss[k] + 1.0) <= 1.e-12 )
                        {
                            ix = k;
                            break;
                        }
                    }
                }

                // rotate pixel list to put that vertex in #0
                int n_rot = vectors.size() - ix;
                int ilast = vectors.size() - 1;
                for ( int k = 0; k < n_rot; k++ )
                {
                    vec3 temp = vectors[ilast];
                    int temppos = points[ilast];
                    vectors.erase(vectors.begin()+ilast);
                    points.erase(points.begin()+ilast);
                    vectors.insert(vectors.begin(), temp);
                    points.insert(points.begin(), temppos);
                }
            }
            if ( nlow > 1 )
            {
                // more than 1concave vertex
                qDebug(" The polygon has more than one concave vertex");
            }
        }
    }
    /* fill the poligon, one triangle at a time */
    npix = (long) nside2npix(nside);
    while ( n_remain >= 3 )
    {
        vp0 = vectors[0];
        vp1 = vectors[n_remain - 2];
        vp2 = vectors[n_remain - 1];

        p0 = points[0];
        p1 = points[n_remain - 2];
        p2 = points[n_remain - 1];

        /* find pixels within the triangle */
        std::set<int> aux = query_triangle(p0, p1, p2, nside);

        res.insert(aux.begin(), aux.end());
        aux.clear();
        n_remain--;
    }

    return res;
}
