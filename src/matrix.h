#ifndef MATRIX_H
#define MATRIX_H


template <class T>
inline void multiplyMatrices(T a[3][3], T b[3][3], T c[3][3], int m, int n, int p)
{
    int i,j,k;
    T sum=0;
    for(i=0; i<m; i++)
    {
        for(j=0; j<p; j++)
        {
            for (k=0;k<n;k++)
                sum=sum+(a[i][k]*b[k][j]);
            c[i][j]=sum;
            sum=0;
        }
    }
}


// calculate the cofactor of element (row,col)
template <class T>
inline int getMinor(T **src, T **dest, int row, int col, int order)
{
    // indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;

    for(int i = 0; i < order; i++ )
    {
        if( i != row )
        {
            colCount = 0;
            for(int j = 0; j < order; j++ )
            {
                // when j is not the element
                if( j != col )
                {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }

    return 1;
}


// Calculate the determinant recursively.
template <class T>
inline double calcDeterminant(T **mat, int order)
{
    // order must be >= 0
    // stop the recursion when matrix is a single element
    if( order == 1 )
        return mat[0][0];

    // the determinant value
    float det = 0;

    // allocate the cofactor matrix
    T **minor;
    minor = new T*[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = new T[order-1];

    for(int i = 0; i < order; i++ )
    {
        // get minor of element (0,i)
        getMinor<T>( mat, minor, 0, i , order);
        // the recusion is here!

        det += (i%2==1?-1.0:1.0) * mat[0][i] * calcDeterminant<T>(minor,order-1);
    }

    // release memory
    for(int i=0;i<order-1;i++)
        delete [] minor[i];
    delete [] minor;

    return det;
}

template <class T>
inline void matrixInverse(T A[3][3], T Y[3][3], int order)
{
    T **aux;
    aux = (T**) malloc(3 * sizeof *aux);
    for(int i=0; i<3; i++)
    {
        aux[i] = (T*) malloc(3 * sizeof *aux[i]);

        for(int j=0; j<3; j++)
            aux[i][j] = A[i][j];
    }

    // get the determinant of a
    double det = 1.0/calcDeterminant<T>(aux, order);

    // memory allocation
    T *temp = new T[(order-1)*(order-1)];
    T **minor = new T*[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));

    for(int j=0;j<order;j++)
    {
        for(int i=0;i<order;i++)
        {
            // get the co-factor (matrix) of A(j,i)
            getMinor<T>(aux,minor,j,i,order);
            Y[i][j] = det*calcDeterminant<T>(minor,order-1);
            if( (i+j)%2 == 1)
                Y[i][j] = -Y[i][j];
        }
    }

    // release memory
    //delete [] minor[0];
    delete [] temp;
    delete [] minor;
    delete [] aux;
}


#endif // MATRIX_H
