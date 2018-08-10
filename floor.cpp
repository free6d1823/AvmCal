#include "floor.h"




#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)


// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)


void YuyvToRgb32(unsigned char* pYuv, int width, int height, unsigned char* pRgb)
{
    //YVYU - format
    int nBps = width*4;
    unsigned char* pY1 = pYuv;

//    unsigned char* pV = pY1+1;
//    unsigned char* pU = pV + 2;

    unsigned char* pU = pY1+1;
    unsigned char* pV = pU + 2;


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
        pY1 += width*2;
        pV += width*2;
        pU += width*2;
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

//    m_pImage = new QImage(m_pData, 1024, 1024, QImage::Format_RGBA8888);

    // Load cube.png image
//    m_texture = new QOpenGLTexture(QImage(":/Camera4.png"));
//    unsigned char * pData = m_pVs->GetFrameData();
//    if (!pData)
//        return false;
    int width = 2560;
    int height = 2048;
    if (!m_pData) {
            m_pData = (unsigned char*) malloc(width*height*4);
//            FILE* fp = fopen("/home/cj/workspace/AvmCal/camera1800x1440.yuv", "rb");
            FILE* fp = fopen("/home/cj/workspace/AvmCal/camerain.yuv", "rb");
            void* pSrc = malloc(width*2*height);
            fread(pSrc, height, width*2, fp);
            fclose(fp);
            YuyvToRgb32((unsigned char*) pSrc, width, height, m_pData);
            free(pSrc);
    }
    m_texture = new QOpenGLTexture(QImage(m_pData, width,
                height, QImage::Format_RGBA8888).mirrored());

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
//            printf ("(%f, 0,%f) -(%f, %f)\n", j*diff*TX_SCALEUP-TX_CENTER, TZ_CENTER-i*diff*TZ_SCALEUP, j*diff, i*diff);
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

    /*
    vert.push_back(QVector3D(-10, 0, 10));
    vert.push_back(QVector3D(10, 0, 10));
    vert.push_back(QVector3D(-10, 0, -10));
    vert.push_back(QVector3D(10, 0, -10));
    uvs.push_back(QVector2D(0.0f, 0.0f));
    uvs.push_back(QVector2D(1.0f, 0.0f));
    uvs.push_back(QVector2D(0.0f, 1.0f));
    uvs.push_back(QVector2D(1.0f, 1.0f));
*/

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
    m_matProjection = pojection*view;
}


void Floor::draw(bool bReload )
{
    if (bReload) {
//        unsigned char * pData = m_pVs->GetFrameData();
//        m_texture->destroy();
//        m_texture->create();
//        m_texture->setData(QImage(pData, m_pVs->Width(),
 //                     m_pVs->Height(), QImage::Format_RGBA8888).mirrored());
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

