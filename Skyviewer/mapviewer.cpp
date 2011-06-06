#include "mapviewer.h"

MapViewer::MapViewer(QWidget *parent) :
    QGLViewer(parent)
{

}

/* create the gl model for the map */
void MapViewer::paintGL()
{
    //qDebug() << "Paiting GL";
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
/*
  glTranslatef(-1.5f,0.0f,-6.0f);

  glBegin(GL_TRIANGLES);
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f,-1.0f, 0.0f);
    glVertex3f( 1.0f,-1.0f, 0.0f);
  glEnd();

  glTranslatef(3.0f,0.0f,0.0f);*/

  glBegin(GL_QUADS);
    glVertex3f(-0.2f, 0.2f, 0.0f);
    glVertex3f( 0.2f, 0.2f, 0.0f);
    glVertex3f( 0.2f,-0.2f, 0.0f);
    glVertex3f(-0.2f,-0.2f, 0.0f);
  glEnd();
}


void MapViewer::changeToMollview()
{

}

void MapViewer::changeTo3D()
{

}

