#include "floor.h"
#include <QFile>
/* exclude these lines when merge to floo.cpp of navm  */
#define IN_AVMCAL
#include "./imglab/ImgProcess.h"
/* exclude these lines when merge to floo.cpp of navm  */

IMAGE* m_pImage = NULL;
TexProcess  texProcess;
bool Return_Error(const char* reason)
{
    perror(reason);
    return false;
}
Floor::Floor()
    : m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    m_texture = NULL;
    // Generate 2 VBOs
    m_arrayVerBuf.create();
    m_arrayTexBuf.create();
    m_indexBuf.create();
    m_nDrawNum = 0;

    init();
}

Floor::~Floor()
{
    m_arrayVerBuf.destroy();
    m_arrayTexBuf.destroy();
    m_indexBuf.destroy();
    if(m_texture){
        delete m_texture;
        m_texture = NULL;
    }

    if(m_pImage) {
        ImgProcess::freeImage(m_pImage);
        m_pImage = NULL;

    }
}

bool Floor::initShaders()
{
    // Compile vertex shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
    {
        return Return_Error("addShaderFromSourceFile Vertex failed!\n");
    }
    // Compile fragment shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
    {
        return Return_Error("addShaderFromSourceFile Fragment failed!\n");
    }

    // Link shader pipeline
    if (!m_program.link())
    {
        return Return_Error("OpenGLShaderProgram link failed!\n");
    }

    // Bind shader pipeline for use
    if (!m_program.bind())
    {
        return Return_Error("OpenGLShaderProgram bind failed!\n");
    }

    return true;
}

bool Floor::initTextures()
{

#ifdef IN_AVMCAL
    if (!m_pImage) {
        m_pImage = ImgProcess::initImage(1800,1440);
            QFile fp(":/camera1800x1440.yuv");
            if(!fp.open(QIODevice::ReadOnly))
                    return false;

            char* pSrc = (char*) malloc(m_pImage->width*2*m_pImage->height);
            fp.read(pSrc, m_pImage->width*2*m_pImage->height);
            fp.close();
            ImgProcess::YuyvToRgb32((unsigned char*) pSrc, m_pImage->width,
                                    m_pImage->width*2, m_pImage->height, m_pImage->buffer, true);
            free(pSrc);
    }
#else
    m_pImage = ImgProcess::refImage(m_pVs->GetFrameData(),m_pVs->Width(),m_pVs->Height());
#endif
    m_texture = new QOpenGLTexture(QImage(m_pImage->buffer,
            m_pImage->width, m_pImage->height, QImage::Format_RGBA8888) );

    // Set nearest filtering mode for texture minification
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_texture->setWrapMode(QOpenGLTexture::Repeat);

    return true;
}

void Floor::CreateVerticesData(vector<QVector3D> & vert, vector<QVector2D>& uvs,
                               vector<unsigned short>& indices)
{
    texProcess.init();
    texProcess.createVertices(vert, indices);
    texProcess.updateUv(uvs);
}

void Floor::init()
{
    vector<QVector3D> vertices;
    vector<QVector2D> uvs;
    vector<unsigned short> indices;
    CreateVerticesData(vertices, uvs, indices);
    m_nDrawNum = indices.size();
    printf("%d - %d - %d\n", (int)vertices.size(), (int)uvs.size(), (int)indices.size());
    // Transfer vertex data to VBO 0
    m_arrayVerBuf.bind();
    m_arrayVerBuf.allocate(&vertices[0], vertices.size() * sizeof(QVector3D));

    m_arrayTexBuf.bind();
    m_arrayTexBuf.allocate(&uvs[0], uvs.size() * sizeof(QVector2D));

    // Transfer index data to VBO 1
    m_indexBuf.bind();
    m_indexBuf.allocate(&indices[0], indices.size() * sizeof(GLushort));

    initShaders();
    initTextures();

    m_matModle.setToIdentity();
}

/* \brief update basic matrics
 * \param pojection
 * \param view
 * \param light
 */
void Floor::update(QMatrix4x4& pojection, QMatrix4x4& view, QVector3D& light)
{
    (void)light;
    m_matProjection = pojection*view;
}


void Floor::draw(bool bReload )
{
    if (bReload) {
#ifdef IN_AVMCAL
#else
        unsigned char * pData = m_pVs->GetFrameData();
        m_texture->destroy();
        m_texture->create();
        m_texture->setData(QImage(pData, m_pVs->Width(),
        m_pVs->Height(), QImage::Format_RGBA8888).mirrored());
#endif
    }
    m_program.bind();

    m_texture->bind();
    m_program.setUniformValue("mvp_matrix", m_matProjection * m_matModle);

    m_program.setUniformValue("texture", 0);

    // Tell OpenGL which VBOs to use
    m_arrayVerBuf.bind();
    m_indexBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program.attributeLocation("a_position");
    m_program.enableAttributeArray(vertexLocation);
    m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));
    m_arrayVerBuf.release();

    m_arrayTexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program.attributeLocation("a_texcoord");
    m_program.enableAttributeArray(texcoordLocation);
    m_program.setAttributeBuffer(texcoordLocation, GL_FLOAT, 0, 2, sizeof(QVector2D));
    m_arrayTexBuf.release();

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, m_nDrawNum, GL_UNSIGNED_SHORT, 0);
    m_texture->release();
    m_indexBuf.release();
    m_program.release();
}

////
/// \brief UpdateUv reload m_arrayTexBuf, called when Calibration modifis FEC parameters
///
void Floor::UpdateUv()
{
    vector<QVector2D> uvs;
    m_arrayTexBuf.release();
    m_arrayTexBuf.destroy();
    m_arrayTexBuf.create();
    m_arrayTexBuf.bind();
    texProcess.init();
    texProcess.updateUv(uvs);
    m_arrayTexBuf.allocate(&uvs[0], uvs.size() * sizeof(QVector2D));


}

////
/// \brief UpdateIndices reload m_indexBuf, called if region display polocy has changed
///
void Floor::UpdateIndices()
{
    vector<unsigned short> indices;
    m_indexBuf.release();
    m_indexBuf.destroy();
    m_indexBuf.create();
    m_indexBuf.bind();
    texProcess.reloadIndices(indices);
    m_indexBuf.allocate(&indices[0], indices.size() * sizeof(unsigned short));
}
