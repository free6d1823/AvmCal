#include "floor.h"
#include "videosource.h"
#include <QFile>

#include "./imglab/ImgProcess.h"


CameraSource* m_pVs = NULL;
 IMAGE* m_pImage = NULL;
TexProcess  texProcess;
ImgSource   imgSource;

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

    m_pVs = new CameraSource();

    init();
}
/////
/// \brief selectSource select texture source
/// \param filename if filename is null, source from camera, else, from a YUV file
///  filename must be of the format xxx_#width#x#heigh#.yuv
void Floor::selectSource(const char* filename)
{
    if (filename == NULL){
        m_bSourceCamera = true;

        imgSource.setImageFromRgb32(m_pVs->GetFrameData(), m_pVs->Width(),m_pVs->Height());
    }else {
        m_bSourceCamera = false;
        char value[32];
        const char* p1 = strchr(filename, '_');
        const char* p2 = strchr(filename, '.');
        int length = p2-p1-1;
        if (p1 == NULL || p2 == NULL || length <3 || length>= (int) sizeof(value)-1)
            return;
        memcpy(value, p1+1, length);
        value[length] = 0;
        p1 = strchr(value, 'x');
        if (!p1)
            return;
        *(char*) p1 = 0;

        int width = atoi(value);
        int height = atoi(p1+1);
        imgSource.setImageFileName(filename, width, height);
    }
    UpdateTexture();

}
////
/// \brief UpdateTexture update ver, tex, indice buffers
///
void Floor::UpdateTexture()
{
    if (m_pImage)
        ImgProcess::freeImage(m_pImage);
    if(m_bSourceCamera)
        m_pImage = ImgProcess::refImage(m_pVs->GetFrameData(),m_pVs->Width(),m_pVs->Height());
    else
        m_pImage = imgSource.loadImage();
    m_texture->destroy();
    m_texture->create();

    m_texture->setData(QImage(m_pImage->buffer,
            m_pImage->width, m_pImage->height, QImage::Format_RGBA8888).mirrored() );

    vector<QVector3D> vertices;
    vector<QVector2D> uvs;
    vector<unsigned short> indices;
    CreateVerticesData(vertices, uvs, indices);
    m_nDrawNum = indices.size();

    m_arrayVerBuf.release();
    m_arrayVerBuf.destroy();
    m_arrayVerBuf.create();
    m_arrayVerBuf.bind();
    m_arrayVerBuf.allocate(&vertices[0], vertices.size() * sizeof(QVector3D));

    m_arrayTexBuf.release();
    m_arrayTexBuf.destroy();
    m_arrayTexBuf.create();
    m_arrayTexBuf.bind();
    m_arrayTexBuf.allocate(&uvs[0], uvs.size() * sizeof(QVector2D));

    m_indexBuf.release();
    m_indexBuf.destroy();
    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(&indices[0], indices.size() * sizeof(unsigned short));

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
    if (m_pImage)
        ImgProcess::freeImage(m_pImage);
    if(!m_bSourceCamera){
        m_pImage = imgSource.loadImage();
    } else {
        m_pImage = ImgProcess::refImage(m_pVs->GetFrameData(),m_pVs->Width(),m_pVs->Height());
    }
    if(m_texture)
        delete m_texture;
    m_texture = new QOpenGLTexture(QImage(m_pImage->buffer,
            m_pImage->width, m_pImage->height, QImage::Format_RGBA8888)  );

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

        if(m_bSourceCamera){
           ImgProcess::updateImageData(m_pImage,
                 m_pVs->GetFrameData(),m_pVs->Width(),m_pVs->Height());
        }
        else {
        //    m_pImage = imgSource.loadImage();
        }
        m_texture->destroy();
        m_texture->create();

        m_texture->setData(QImage(m_pImage->buffer,
                m_pImage->width, m_pImage->height, QImage::Format_RGBA8888) );

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
bool Floor::exportTextureArray(const char* filename)
{
    FILE* fp = fopen(filename, "wt");
    if (!fp)
        return false;
    bool ret = false;
    char buffer[256];

    vector<QVector3D> vertices;
    vector<QVector2D> uvs;
    vector<unsigned short> indices;
    CreateVerticesData(vertices, uvs, indices);

    do {
        sprintf(buffer, "NAVM texture file\n%lu\n%lu\n%lu\n", vertices.size(),
                uvs.size(), indices.size());
        if(1 != fwrite(buffer, strlen(buffer), 1, fp))
            break;

        for (unsigned int i=0; i<vertices.size(); i++){
            sprintf(buffer, "%f, %f, %f\n", vertices[i].x(),vertices[i].y(),vertices[i].z());
            if(1 != fwrite(buffer, strlen(buffer), 1, fp))
                break;

        }
        for (unsigned int i=0; i<uvs.size(); i++){
            sprintf(buffer, "%f, %f\n", uvs[i].x(),uvs[i].y());
            if(1 != fwrite(buffer, strlen(buffer), 1, fp))
                break;
        }
        for (unsigned int i=0; i<indices.size(); i++){
            sprintf(buffer, "%d\n", indices[i]);
            if(1 != fwrite(buffer, strlen(buffer), 1, fp))
                break;
        }

        ret = true;
    }while (0);
    fclose(fp);
    vertices.clear();
    uvs.clear();
    indices.clear();
    return ret;
}

bool Floor::loadTextureArray(const char* filename)
{
    FILE* fp = fopen(filename, "rt");
    if (!fp)
        return false;
    bool ret = false;
    char buffer[256];
    vector<QVector3D> vertices;
    vector<QVector2D> uvs;
    vector<unsigned short> indices;
    int nv=0, nu=0, ni=0;
    do {
        if(NULL == fgets(buffer, 256, fp))break;
        if ( strstr(buffer, "NAVM") == NULL) {
            fprintf(stderr, "%s is not a NAVM texture file!!\n", filename);
            break;
        }
        if(NULL == fgets(buffer, 256, fp))break;
        nv = atoi(buffer);
        if(NULL == fgets(buffer, 256, fp))break;
        nu = atoi(buffer);
        if(NULL == fgets(buffer, 256, fp))break;
        ni = atoi(buffer);
        if(nv != nu || ni==0 || nv==0) {
            fprintf(stderr, "%s texture file content is corrupted!!\n", filename);
            break;
        }
        float x,y,z;
        int k;
        for (int i=0; i< nv; i++){
            if(1 > fscanf(fp, "%f,%f,%f", &x,&y,&z))break;
            vertices.push_back(QVector3D(x,y,z));
        }
        for (int i=0; i< nu; i++){
            if(1 > fscanf(fp, "%f,%f", &x,&y))break;
            uvs.push_back(QVector2D(x,y));
        }
        for (int i=0; i< ni; i++){
            if(1 > fscanf(fp, "%d", &k))break;
            indices.push_back(k);
        }

        ret = true;
    }while (0);
    fclose(fp);
    if (ret != true){
        vertices.clear();
        uvs.clear();
        indices.clear();
    } else {
        m_arrayTexBuf.release();
        m_arrayTexBuf.destroy();
        m_arrayTexBuf.create();
        m_arrayTexBuf.bind();
        m_arrayTexBuf.allocate(&uvs[0], uvs.size() * sizeof(QVector2D));

        m_arrayVerBuf.release();
        m_arrayVerBuf.destroy();
        m_arrayVerBuf.create();
        m_arrayVerBuf.bind();
        m_arrayVerBuf.allocate(&vertices[0], vertices.size() * sizeof(QVector3D));

        m_indexBuf.release();
        m_indexBuf.destroy();
        m_indexBuf.create();
        m_indexBuf.bind();
        m_indexBuf.allocate(&indices[0], indices.size() * sizeof(unsigned int));

    }
    return ret;
}
