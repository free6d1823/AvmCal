#ifndef FLOOR_H
#define FLOOR_H

#include "basicobject.h"
#include <vector>
using namespace std;


class Floor : public BasicObject
{
public:
    Floor();
    virtual ~Floor();
    /* \brief render floor image
     * \param bReload set true to reload texture
     */

    void draw(bool bReload = false);
    /* \brief update basic matrics
     * \param pojection
     * \param view
     * \param light
     */
    void update(QMatrix4x4& pojection, QMatrix4x4& view, QVector3D& light);

    /* \brief apply transform matrix to object
     * \param transform
     */
    void transform(QMatrix4x4& transform);

    ////
    /// \brief UpdateUv reload m_arrayTexBuf, called when Calibration modifis FEC parameters
    ///
    void UpdateUv();

    ////
    /// \brief UpdateIndices reload m_indexBuf, called if region display polocy has changed
    ///
    void UpdateIndices();

    ////
    /// \brief UpdateTexture update ver, tex, indice buffers
    ///
    void UpdateTexture();

    bool exportTextureArray(const char* filename);
    bool loadTextureArray(const char* filename);
    /////
    /// \brief selectSource select texture source
    /// \param filename if filename is null, source from camera, else, from a YUV file
    ///
    void selectSource(const char* filename);

protected:
    void CreateVerticesData(vector<QVector3D> & vert, vector<QVector2D>& uvs, vector<unsigned short>& indices);
    void init();
    bool initShaders();
    bool initTextures();

private:
    bool m_bSourceCamera; /*!< true if from camera, false from file */
    int m_nDrawNum;
    QOpenGLBuffer m_arrayVerBuf;
    QOpenGLBuffer m_arrayTexBuf;
    QOpenGLBuffer m_indexBuf;

};
#endif // FLOOR_H
