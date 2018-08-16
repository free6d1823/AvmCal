#include "floor.h"
#include <QFile>
/* exclude these lines when merge to floo.cpp of navm  */
#define IN_AVMCAL
#ifdef IN_AVMCAL
#include "ImgProcess.h"
extern ImgProcess* gpImgProcess[MAX_CAMERAS];
#endif //IN_AVMCAL
/* exclude these lines when merge to floo.cpp of navm  */



#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)


// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)


void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst)
{
    //YVYU - format
    int nBps = width*4;
    unsigned char* pY1 = pYuv;

    unsigned char* pV;
    unsigned char* pU;

    if (uFirst) {
        pU = pY1+1; pV = pU+2;
    } else {
        pV = pY1+1; pU = pV+2;
    }


    unsigned char* pLine1 = pRgb;

    unsigned char y1,u,v;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j+=2)
        {
            y1 = pY1[2*j];
            u = pU[2*j];
            v = pV[2*j];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y1 = pY1[2*j+2];
            pLine1[j*4+4] = YUV2B(y1, u, v);//b
            pLine1[j*4+5] = YUV2G(y1, u, v);//g
            pLine1[j*4+6] = YUV2R(y1, u, v);//r
            pLine1[j*4+7] = 0xff;
        }
        pY1 += stride;
        pV += stride;
        pU += stride;
        pLine1 += nBps;

    }
}

unsigned char* m_pData = NULL;
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

    if(m_pData) {
        free(m_pData);
        m_pData = NULL;

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

    int width;
    int height;
#ifdef IN_AVMCAL
    width = 1800;
    height = 1440;
    if (!m_pData) {
            m_pData = (unsigned char*) malloc(width*height*4);
            QFile fp(":/camera1800x1440.yuv");
            if(!fp.open(QIODevice::ReadOnly))
                    return false;

            char* pSrc = (char*) malloc(width*2*height);
            fp.read(pSrc, height*width*2);
            fp.close();
            YuyvToRgb32((unsigned char*) pSrc, width, width*2, height, m_pData, true);
            free(pSrc);
    }
#else
    m_pData = m_pVs->GetFrameData();
    width = m_pVs->Width();
    height = m_pVs->Height();
#endif
    m_texture = new QOpenGLTexture(QImage(m_pData, width,
                height, QImage::Format_RGBA8888) );

    // Set nearest filtering mode for texture minification
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_texture->setWrapMode(QOpenGLTexture::Repeat);

    return true;
}
#define MAX_GRIDES   100
#define TX_SCALEUP    20
#define TX_CENTER      10
#define TZ_SCALEUP    20
#define TZ_CENTER      10
void Floor::CreateVerticesData(vector<QVector3D> & vert, vector<QVector2D>& uvs,
                               vector<unsigned short>& indices)
{
    int i, j;
    float diff = 1.0f/(MAX_GRIDES);
    for (i=0; i<= MAX_GRIDES; i++) {
        for (j=0; j<=MAX_GRIDES; j++) {
            vert.push_back(QVector3D(j*diff*TX_SCALEUP-TX_CENTER, 0, TZ_CENTER-i*diff*TZ_SCALEUP));
            uvs.push_back(QVector2D(j*diff, i*diff));
        }
    }

    int k;
    for (i=0; i< MAX_GRIDES; i++) {
        k = i*(MAX_GRIDES+1);
        for(j=0; j<MAX_GRIDES; j++) {
            indices.push_back(k);
            indices.push_back(k+1);
            indices.push_back(k+MAX_GRIDES+1);
            indices.push_back(k+MAX_GRIDES+1);
            indices.push_back(k+1);
            indices.push_back(k+MAX_GRIDES+2);

            k++;
        }
    }
#ifdef xIN_AVMCAL
    uvs.clear();
    for (i=0;i<4;i++)
    {
        gpImgProcess[i]->updateUv(uvs);
    }
#endif //IN_AVMCAL
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

