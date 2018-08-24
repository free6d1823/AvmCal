#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>


class GeometryEngine;

class MainWidget  : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void InitViewMode();

    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *ev) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void CreateMenus();
private slots:
    void onShowFecDialog();
    void onUpdateUv();
    void onRedraw();
    void onOpenFile();
    void onExportFile();
    void onLoadFile();
    void onSelectCamera();
    void onSelectImageFile();
private:
    QMatrix4x4 m_matProj;
    QVector3D m_posCamera;
    QVector3D m_posCenter;
    QVector3D m_dirCamera;
};

#endif // MAINWIDGET_H
