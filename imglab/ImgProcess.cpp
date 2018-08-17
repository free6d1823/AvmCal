#include "ImgProcess.h"
#include "Mat.h"

#define PI_2    (3.1415967*2.0)


ImgProcess::ImgProcess()
{

}
ImgProcess::~ImgProcess()
{
}

void ImgProcess::doFec(double u, double v, double &x, double &y, FecParam* m_pFec)
{
    double  u1, v1;
    double x1,y1;
    //transform uu = M x u
    u1 = u-0.5;
    v1 = v-0.5;
    double fr = 2*tan(m_pFec->fov/2); //focus length of rectified image

    double rp = sqrt(u1*u1+v1*v1); //radius of point (u1,v1) on rectified image
    double rq;	//fisheye

    double rq1;

    if(1) //fisheye
        rq1 = atan(rp*fr)/PI_2;		//0~1
    else //normal lens
        rq1 = rp; //if no fec

    //LDC
    if (rp <= 0) {
        x1 = y1 = 0;
    } else {
        u1  = u1 * rq1/rp;
        v1  = v1 * rq1/rp;

        double rq2 = rq1*rq1;
        rq  = (1+m_pFec->k1* rq2+ m_pFec->k2* rq2*rq2);
        x1 = u1/rq;
        //t
        y1 = v1/rq+m_pFec->c*rq2;
    }
    double x2,y2;
    //pitch
    double phi = atan(y1*2);//assume r=0.5
    double sy = 1+tan(m_pFec->pitch)*tan(phi);
    x2 = x1/sy;
    y2 = (y1/sy)/cos(m_pFec->pitch);
    //yaw
    double theda = atan(x2*2);
    double sx = 1+tan(m_pFec->yaw)*tan(theda);
    x2 = (x2/sx)/cos(m_pFec->yaw);
    y2 = y2/sx;

    //spin - z-axis
    x1 = cos(m_pFec->roll)* x2 - sin(m_pFec->roll)*y2;
    y1 = sin(m_pFec->roll)* x2 + cos(m_pFec->roll)*y2;
    //intrinsic parameters calibration
    //t 	x1 = param.a*x1 + param.c*y1;		//x-scale and de-skewness
    x1 = m_pFec->a*x1;		//x-scale and de-skewness
    y1 = m_pFec->b*y1;								//y- scale
    x = (x1+ m_pFec->ptCenter.x);
    y = (y1+ m_pFec->ptCenter.y);

}

int ImgProcess::updateUv(vector <QVector2D> &uv)
{
    return 0;
}
void ImgProcess::findHomoMatreix(dbPOINT s[4], dbPOINT t[4], double hcoef[3][3])
{
    int i;
    //normalize source FP
    //Ah=b
    Mat A(8,8);
    Mat b(1,8);
    int j=4;
    for (i=0;i<4;i++) {
        A.Set(0,i, t[i].x); A.Set(1, i, t[i].y); A.Set(2,i,1); A.Set(3,i, 0); A.Set(4,i, 0); A.Set(5,i, 0);
        A.Set(6,i, -t[i].x*s[i].x); A.Set(7, i, -t[i].y*s[i].x);
        b.Set(0,i, s[i].x);

        A.Set(0,j, 0); A.Set(1,j, 0); A.Set(2,j, 0);
        A.Set(3,j, t[i].x); A.Set(4, j, t[i].y); A.Set(5,j,1);
        A.Set(6,j, -t[i].x*s[i].y); A.Set(7, j, -t[i].y*s[i].y);
        b.Set(0,j, s[i].y);
        j++;
    }
    Mat h (1,8);
    Mat AI(8,8);
    if(A.FindInverse(AI)) {
            h.Multiply(AI, b);
            for(int i=0;i<8;i++)
                hcoef[i/3][i%3] = 	h.Get(0,i);
            hcoef[2][2] = 1;
//verify
#if 0
    printf("derived Matrix A-1=\n");
    AI.Print();
    printf("Find Vector h=\n");
    h.Print();

Mat c(1,8);
c.Multiply(A, h);
printf("Verify A.h = b'\n");
c.Print();
#endif
    }else {
            for(int i=0;i<8;i++)
                hcoef[i/3][i%3] = -1;
    }
}
bool ImgProcess::doHomoTransform(double s, double t, double &u, double &v, double h[3][3])
{
    u = h[0][0] * s +h[0][1]*t + h[0][2];
    v = h[1][0] * s + h[1][1]*t + h[1][2];
    double scale = h[2][0] * s + h[2][1]*t + 1;
    if (scale != 0) {
            u = u/scale;
            v = v/scale;
            if(u>=1 || v>=1 ||u<0 ||v <0) return false;
            return true;
    }
    return false;
}
