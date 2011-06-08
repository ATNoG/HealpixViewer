#include "mapviewer.h"

using namespace qglviewer;

MapViewer::MapViewer(QWidget *parent) :
    QGLViewer(parent)
{

}

/* create the gl model for the map */
void MapViewer::draw()
{
    // Save the current model view matrix (not needed here in fact)
      //glPushMatrix();

      // Multiply matrix to get in the frame coordinate system.
      //glMultMatrixd(manipulatedFrame()->matrix());

      // Scale down the drawings
      //glScalef(0.3f, 0.3f, 0.3f);

      // Draw an axis using the QGLViewer static function
      //drawAxis();

      tesselation->draw();

      // Restore the original (world) coordinate system
      // glPopMatrix();
}


void MapViewer::loadMap(HealpixMap* map)
{
    skymap = map;
}


void MapViewer::init()
{
    /*
        General configuration.
    */
    setBackgroundColor(QColor(0,0,0));	// Black background.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
        Define the lighting.
    */
    GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_zero[] = { 0.0, 0.0, 0.0, 1.0 };
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_zero);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_zero);

    /*
        Enable GL textures.
    */
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glEnable( GL_TEXTURE_2D );
                    // Nice texture coordinate interpolation
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    /*
        Configure the camera.
    */
    camera()->setPosition( Vec(3.0,0,0) );
    camera()->lookAt( Vec(0,0,0) );
    camera()->setUpVector(Vec(0,0,1));


    /*
    qglviewer::CameraConstraint *constraint;
    constraint = new CameraConstraint(camera());
    constraint->setRotationConstraintType(AxisPlaneConstraint::FREE);
    camera()->frame()->setConstraint(constraint);*/

    //ManipulatedFrame* mframe = new ManipulatedFrame();
    //setManipulatedFrame(mframe);
    //setAxisIsDrawn();



    /*
        Recover the state from a prior run.
    */
    restoreStateFromFile();


    /* create the sphere tesselation */
    tesselation = new Tesselation(64);
}

void MapViewer::changeToMollview()
{

}

void MapViewer::changeTo3D()
{

}


void MapViewer::mousePressEvent(QMouseEvent* e)
{
    QGLViewer::mousePressEvent(e);
    emit(cameraChanged(e, MOUSEPRESS, this));
}


void MapViewer::mouseReleaseEvent(QMouseEvent *e)
{
    // TODO: just a hack to disable spinning... may have problems when selecting a pixel for example
    //QGLViewer::mouseReleaseEvent(e);
    emit(cameraChanged(e, MOUSERELEASE, this));
}

void MapViewer::mouseMoveEvent(QMouseEvent* e)
{
    QGLViewer::mouseMoveEvent(e);
    emit(cameraChanged(e, MOUSEMOVE, this));
}


void MapViewer::wheelEvent(QWheelEvent *e)
{
    QGLViewer::wheelEvent(e);
    emit(cameraChanged(e, MOUSEWHEEL, this));
}


void MapViewer::synchronize(QEvent *e, int type)
{
    switch(type)
    {
        case MOUSEMOVE:
            QGLViewer::mouseMoveEvent((QMouseEvent*) e);
            break;

        case MOUSEPRESS:
            QGLViewer::mousePressEvent((QMouseEvent*) e);
            break;

        case MOUSERELEASE:
            QGLViewer::mouseReleaseEvent((QMouseEvent*) e);
            break;

        case MOUSEWHEEL:
            QGLViewer::wheelEvent((QWheelEvent*) e);
            break;
    }
}


void MapViewer::startSpinning(int interval)
{
    qDebug("start spinning");
}
