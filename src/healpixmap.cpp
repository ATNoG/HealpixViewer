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

    /* Map already processed ? Check cache */
    if(!checkMapCache())
    {
        qDebug() << "Map doesnt exists on cache";

        createCache = true;

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

    float *values;

    try
    {
         values = new float[length];
    }
    catch(const std::bad_alloc &)
    {
        abort();
        throw HealpixMapException("Not enough memory");
    }

    //qDebug() << "Reading file " << path;

    /* read content */
    QFile f(path);

    cacheAccess.lock();
    if(f.open(QIODevice::ReadOnly))
    {
        f.seek(firstPosition*sizeof(float));
        f.read((char*)values, length*sizeof(float));
    }
    else
    {
        delete[] values;
        cacheAccess.unlock();
        abort();
        throw HealpixMapException("Error reading cache");
    }
    f.close();
    cacheAccess.unlock();

    return values;
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
        throw HealpixMapException("Error writing map information into cache");
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
    float* values = getFullMap(minNSide);
    Histogram *histo = new Histogram(values, nside2npix(minNSide));
    histo->getMinMax(min, max);

    delete[] values;
    delete histo;
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
