#include "mainwidget.h"
#include <QMouseEvent>
#include "floor.h"

Floor* m_pFloor = NULL;

MainWidget::MainWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_pFloor = NULL;
}

MainWidget::~MainWidget()
{
    makeCurrent();
    if (m_pFloor)
        delete m_pFloor;
    doneCurrent();
}
void MainWidget::InitViewMode()
{
    m_posCamera = QVector3D(0,20,0);
    m_posCenter = QVector3D(0,0,0);
    m_dirCamera = QVector3D(0,0,1);
    m_matProj.setToIdentity();
    m_matProj.perspective(60.0f, 4.0f/3.0f, 1, 100);
    update();
}
void MainWidget::keyPressEvent(QKeyEvent *ev)
{
    double rotateSpeed = 3.1415967/180.0;
    float ca = (float) cos(rotateSpeed);
    float sa = (float) sin(rotateSpeed);
    bool bUpdate = false;
    switch (ev->key()) {
    case Qt::Key_Up:
        m_posCamera.setY(m_posCamera.y() - 0.1f );
        bUpdate = true;
        break;
    case Qt::Key_Down:
        m_posCamera.setY(m_posCamera.y() + 0.1f );
        bUpdate = true;
        break;
    case Qt::Key_Left:
        m_dirCamera.setX(m_dirCamera.x() * ca - m_dirCamera.z()*sa);
        m_dirCamera.setZ(m_dirCamera.x() * sa + m_dirCamera.z()*ca);
        bUpdate = true;

        break;
    case Qt::Key_Right:
        m_dirCamera.setX(m_dirCamera.x() * ca + m_dirCamera.z()*sa);
        m_dirCamera.setZ(-m_dirCamera.x() * sa + m_dirCamera.z()*ca);
        bUpdate = true;

        break;
    default:
        break;
    }
    if (bUpdate)
        update();
}
void MainWidget::mousePressEvent(QMouseEvent *e)
{
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
}
void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);


     m_pFloor = new Floor;
}
void MainWidget::resizeGL(int w, int h)
{
    InitViewMode();
}
void MainWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 view;
    view.lookAt(m_posCamera, m_posCenter, m_dirCamera);
    QVector3D light = QVector3D(0,0,0);
    m_pFloor->update(m_matProj, view, light);
    m_pFloor->draw(false);
}
