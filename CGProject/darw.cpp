#pragma warning(disable:4996)
#include <gl/glut.h>
#ifdef WIN32
#include<time.h>
#include <shlobj.h>
#endif
#include<string>
#include <stdio.h>
#include <cstdlib>
#include<iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <cmath>
#include "stb_image.h"
#include <map>
#include <list>
#define STB_IMAGE_IMPLEMENTATION
#define GL_PI 3.1415f
#define BMP_Header_Length 54
#define sunshine 255.0/255.0, 210.0/255.0, 166.0/255.0
#define DEFAULT_PARTICLE_NUMBER 200
#define DEFAULT_PARTICLE_LIFESPAN 30000

using namespace std;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


GLfloat winw = 800;
GLfloat winh = 800;
GLfloat look_x_angle = 0;
GLfloat look_y_angle = 0;
GLfloat pos_x = 0.0f;
GLfloat pos_y = 0.0f;
GLfloat pos_z = 300.0f;
GLfloat sky_r = 1000.0f;

GLfloat matiral_specular[4] = { 0.00, 0.00, 0.00, 1.00 };  //材质参数：镜面反射，所得到的光的强度（颜色）
GLfloat matiral_emission[4] = { 0.00, 0.00, 0.00, 1.00 };  //材质本身就微微的向外发射光线
const GLfloat matiral_shininess = 11.00;


struct Point {
    double objx;
    double objy;
    double objz;
};

const double PI = 3.1416f / 18.0f;
typedef struct {
    float life;  // last time
    float r, g, b;     // color
    float x, y, z;     // the position
    float xi, yi, zi;  // what direction to move
} particlepice;

float randFloat01() { return 1.0 * rand() / RAND_MAX; }
float randFloat(float from, float to) {
    return from + (to - from) * randFloat01();
}
int randInt(int from, int to) { return from + rand() % (to - from); }


GLuint tex2;
GLuint tex3;
GLuint tex4;
GLuint tex5;
GLuint tex6;

GLuint texs1;
GLuint texf1;
GLuint texs2;
GLuint texf2;

GLuint texc1;
GLuint texc2;
GLuint texsh1;
GLuint texsh2;
GLuint texw1;
GLuint texw2;

int whatsky = 2;
int whattex = 1;
double lightpow = 1.4;
double objx = 0, objy = 0, objz = 0;
double objnx = 0, objny = 0, objnz = 0;
double objfx = 0, objfy = 0, objfz = 0;
Point pt[4]; //存放点信息的数组，计算曲线时用
Point lastpoint;
Point firstpoint;
vector<Point> lastpointlist;
vector<Point> vpt; //存放点的向量，输入数据时用到
vector<Point> temp_linepoint; //预画的线上的点
vector<Point> linepoint;//确定的轮廓线上的点
bool bDraw; //是否绘制曲线，false则确定没有开始绘制曲线
bool show = false;
int highestnum = 0;
int lowestnum = 0;
int nInput; //点的数量
int linenum = 0;//线的数量
int whichpoint = 4; //判断选中哪个点
bool mouseLeftIsDown = false; //是否点击鼠标左键
bool mouseRightIsDown = false; //是否点击鼠标右键
bool particleflag = false;//粒子效果
unsigned int tex;//纹理



class particle {
public:
    particle(GLuint particleNumber = DEFAULT_PARTICLE_NUMBER,
        GLuint particleLifespan = DEFAULT_PARTICLE_LIFESPAN)
        : mParticleNumber(particleNumber),
        mParticleLifespan(particleLifespan) {}
    void Update();
    //渲染，普通的渲染函数就是渲染每一粒存在的粒子
    virtual void Render();
    int getSize() { return ls.size(); }

private:
    GLuint mParticleNumber;
    GLuint mParticleLifespan;
    std::list<particlepice> ls;
    void RenderParticle(const particlepice& p);
};

//粒子的状态更新，可以尽情发挥自己的创意编写代码
void particle::Render() {
    auto p = ls.begin();
    while (p != ls.end()) {
        p->x += p->xi * 0.1;
        p->y += p->yi * 0.1;
        p->z += p->zi * 0.1;
        p->life--;
        if (p->life == 0|| p->y<-300.0f) {
            ls.erase(p++);
        }
        else {
            p++;
        }
    }
    glBindTexture(GL_TEXTURE_2D, tex2);
    for (auto& item : ls) RenderParticle(item);
}

void particle::Update() {
    //新粒子的创建
    particlepice particle;
    int newParticleNumber = mParticleNumber;
    for (int i = 0; i < newParticleNumber; ++i) {
        double sqrt2 = sqrt(2);
        double sqrt6 = sqrt(6);
        particle.x = randFloat(-1, 1) * 1200;
        particle.y = randFloat(0, 1) * 1200;
        particle.z = randFloat(-1, 1) * 1200;
        particle.r = randFloat(0, 1);
        particle.g = randFloat(0, 1);
        particle.b = randFloat(0, 1);
        particle.xi = randFloat(-0.1f, 0.1f) * 5;
        particle.yi = -80.0f;
        particle.zi = randFloat(-0.1f, 0.1f) * 5;
        particle.life = mParticleLifespan;
        ls.push_back(particle);
    }
}

void particle::RenderParticle(const particlepice& p) {
    //int k = 4;
    //glTexCoord2f(0.0f, 1.0f); glVertex3f(p.x, p.y, p.z);
    //glTexCoord2f(1.0f, 1.0f); glVertex3f(p.x + randFloat(1, 6)*k, p.y + randFloat(2, 5) * k, p.z + randFloat(1, 6) * k);
    //glTexCoord2f(0.0f, 0.0f); glVertex3f(p.x + randFloat(-4, -1)*k, p.y + randFloat(-5, -2) * k, p.z + randFloat(-4, -1) * k);
    int fs = 20;
    for (float i = 0; i < 8; i++) {
        glBegin(GL_QUAD_STRIP);

        for (float j = 0; j < 16; j++) {
            glColor3f(sinf(3.1416f / 2.0f - 3.1416f / 36.0f * j) + (sinf(3.1416f / 144.0f * i)),
                0.0f,
                cosf(3.1416f / 2.0f - 3.1416f / 12.0f * j));//颜色

            glVertex3f(sinf(PI * j) * cosf(PI * i)*fs+p.x,
                cosf(PI * j)*fs+p.y,
                sinf(PI * j) * sinf(PI * i)*fs+p.z);//顶点

            glColor3f(sinf(3.1416f / 2.0f - 3.1416f / 36.0f * j) + (sinf(3.1416f / 144.0f * i)),
                0.0f,
                cosf(3.1416f / 2.0f - 3.1416f / 12.0f * j));//颜色

            glColor3f(1,
                0.0f,
                0);//颜色
            glVertex3f(sinf(PI * j) * cosf(PI * (i + 1))*fs + p.x,
                cosf(PI * j)*fs+p.y,
                sinf(PI * j) * sinf(PI * (i + 1))*fs + p.z

            );//顶点



        }glEnd();
    }
}




particle w; 


//纹理对象的编号

int power_of_two(int n)
{
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}

/* 函数load_texture
* 读取一个BMP文件作为纹理
* 如果失败，返回0，如果成功，返回纹理编号
*/
GLuint load_texture(const char* file_name)
{
    GLint width, height, total_bytes;
    GLubyte* pixels = 0;
    GLuint last_texture_ID = 0, texture_ID = 0;
    FILE* pFile;
    errno_t err;

    // 打开文件，如果失败，返回
    if ((err = fopen_s(&pFile, file_name, "rb")) != 0)
    {
        exit(0);                               //终止程序
    }

    // 读取文件中图象的宽度和高度
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

    // 计算每行像素所占字节数，并根据此数据计算总像素字节数
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }

    // 根据总像素字节数分配内存
    pixels = (GLubyte*)malloc(total_bytes);
    if (pixels == 0)
    {
        fclose(pFile);
        return 0;
    }

    // 读取像素数据
    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    // 对就旧版本的兼容，如果图象的宽度和高度不是的整数次方，则需要进行缩放
    // 若图像宽高超过了OpenGL规定的最大值，也缩放
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if (!power_of_two(width)
            || !power_of_two(height)
            || width > max
            || height > max)
        {
            const GLint new_width = 256;
            const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
            GLint new_line_bytes, new_total_bytes;
            GLubyte* new_pixels = 0;

            // 计算每行需要的字节数和总字节数
            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;

            // 分配内存
            new_pixels = (GLubyte*)malloc(new_total_bytes);
            if (new_pixels == 0)
            {
                free(pixels);
                fclose(pFile);
                return 0;
            }

            // 进行像素缩放
            gluScaleImage(GL_RGB,
                width, height, GL_UNSIGNED_BYTE, pixels,
                new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

            // 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
            free(pixels);
            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }

    // 分配一个新的纹理编号
    glGenTextures(1, &texture_ID);
    if (texture_ID == 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    // 绑定新的纹理，载入纹理并设置纹理参数
    // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
    GLint lastTextureID = last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定
    free(pixels);
    return texture_ID;
}

Point caculateObject(int x, int y)
{
    double modelview[16], projection[16];
    int viewport[4];
    float z = 1;

    /*Read the projection, modelview and viewport matrices
    using the glGet functions.*/
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetIntegerv(GL_VIEWPORT, viewport);

    //Read the window z value from the z-buffer 
    glReadPixels(x, viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

    //Use the gluUnProject to get the world co-ordinates of
    //the point the user clicked and save in objx, objy, objz.
    gluUnProject(x, viewport[3] - y, 0.0, modelview, projection, viewport, &objnx, &objny, &objnz);
    gluUnProject(x, viewport[3] - y, z, modelview, projection, viewport, &objx, &objy, &objz);
    gluUnProject(x, viewport[3] - y, 1.0, modelview, projection, viewport, &objfx, &objfy, &objfz);
    Point objxyz;
    objxyz.objx = objx;
    objxyz.objy = objy;
    objxyz.objz = objz;

    return objxyz;
}

double caculateSquareDistance(int x, int y, Point b) //计算鼠标与点的位置
{
    Point a = caculateObject(x, y);
    return sqrt((a.objx - b.objx) * (a.objx - b.objx) + (a.objy - b.objy) * (a.objy - b.objy));
}

int Factorial(int n)
{
    if (n == 1 || n == 0)
    {
        return 1;
    }
    else
    {
        return n * Factorial(n - 1);
    }
}

double C(int n, int i)
{
    return ((double)Factorial(n)) / ((double)(Factorial(i) * Factorial(n - i)));
}
//求一个数u的num次方
double N(double u, int n)
{
    double sum = 1.0;
    if (n == 0)
    {
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        sum *= u;
    }
    return sum;
}

void ControlPoint(vector<Point> vpt) //绘制目标点
{
    glPointSize(5); //点的大小

    for (int i = 0; i < vpt.size(); i++)
    {
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(vpt[i].objx, vpt[i].objy, vpt[i].objz); //点为红色
        glEnd();
    }
}

void ReControlPoint(double x, double y, double z) //绘制修改的目标点
{
    glPointSize(5); //点的大小
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(x, y, z); //点为红色
    glEnd();
}

void PolylineGL(Point* pt) //绘制多项式曲线

{

    glBegin(GL_POINTS);
    glPointSize(15.0f);
    glColor3f(1.0f, 0, 0);
    glVertex3f(pt[0].objx, pt[0].objy, 200.0f);
    glVertex3f(pt[1].objx, pt[1].objy, 200.0f);
    glVertex3f(pt[2].objx, pt[2].objy, 200.0f);
    glVertex3f(pt[3].objx, pt[3].objy, 200.0f);
    glEnd();


    glBegin(GL_LINE_STRIP);
    glColor3f(0, 1.0f, 0);

    GLfloat u = 0;
    double x, y;
    int i;
    Point a;
    temp_linepoint.clear();
    for (u = 0.01; u <= 1; u = u + 0.02)
    {
        x = 0;
        y = 0;
        for (i = 0; i < 4; i++)
        {
            x += C(4 - 1, i) * N(u, i) * N((1 - u), (4 - 1 - i)) * pt[i].objx;
            y += C(4 - 1, i) * N(u, i) * N((1 - u), (4 - 1 - i)) * pt[i].objy;
        }
        a.objx = x; a.objy = y; a.objz = 200.0f;
        temp_linepoint.push_back(a);
        glVertex3f(x, y, 200.0f);
    }
    glEnd();
}

void DrawSky() //绘制修改的目标点
{
    //前
    glEnable(GL_TEXTURE_2D);
    if (whatsky == 1) {
        glBindTexture(GL_TEXTURE_2D, texs1);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z + sky_r);

        glEnd();

        //后
        glBindTexture(GL_TEXTURE_2D, texs1);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z - sky_r);

        glEnd();

        //左
        glBindTexture(GL_TEXTURE_2D, texs1);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z - sky_r);

        glEnd();

        //右
        glBindTexture(GL_TEXTURE_2D, texs1);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z + sky_r);

        glEnd();

        //上
        glBindTexture(GL_TEXTURE_2D, texs1);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z + sky_r);

        glEnd();

        //下
        glBindTexture(GL_TEXTURE_2D, texf1);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z + sky_r);

        glEnd();
    }
    if (whatsky == 2) {
        glBindTexture(GL_TEXTURE_2D, texs2);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z + sky_r);

        glEnd();

        //后
        glBindTexture(GL_TEXTURE_2D, texs2);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z - sky_r);

        glEnd();

        //左
        glBindTexture(GL_TEXTURE_2D, texs2);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z - sky_r);

        glEnd();

        //右
        glBindTexture(GL_TEXTURE_2D, texs2);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z + sky_r);

        glEnd();

        //上
        glBindTexture(GL_TEXTURE_2D, texs2);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y + sky_r, pos_z + sky_r);

        glEnd();

        //下
        glBindTexture(GL_TEXTURE_2D, texf2);

        glBegin(GL_QUADS);

        /** 指定纹理坐标和顶点坐标 */
        glTexCoord2f(1.0f, 0.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z + sky_r);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(pos_x + sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z - sky_r);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(pos_x - sky_r, pos_y - sky_r, pos_z + sky_r);

        glEnd();
    }


}


void CertainlineGL() //绘制确定的曲线
{
    GLfloat u = 0;
    double x, y;
    Point a;
    int i;
    if (linepoint.size() > 0) {
        vector<Point>::iterator it = linepoint.begin();
        for (i = 0; i <= linenum; i++) {
            glBegin(GL_LINE_STRIP);
            glColor3f(1.0f, 1.0f, 1.0f);
            for (; it != linepoint.begin() + 50 * i; it++)
            {
                x = (*it).objx;
                y = (*it).objy;
                glVertex3f(x, y, 200.0f);
            }
            glEnd();
        }
    }
}



void getHighestpoint()
{
    double hightest = -900.0f;
    for (int i = 0; i < linepoint.size(); ++i) {
        if (linepoint[i].objy >= hightest) {
            hightest = linepoint[i].objy;
            highestnum = i;
        }
    }

}
void getLowestpoint()
{
    double lowest = 900.0f;
    for (int i = 0; i < linepoint.size(); ++i) {
        if (linepoint[i].objy <= lowest) {
            lowest = linepoint[i].objy;
            lowestnum = i;
        }
    }
}
void myDisplay()
{
    if (show == false) {

        GLfloat nRange = 100.0f;

        glViewport(0, 0, winw, winh);

        // 重置投影矩阵堆栈
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //建立裁剪区（左，右，下，上，近，远）
        if (winw <= winh)
            glOrtho(-nRange, nRange, -nRange * winh / winw, nRange * winh / winw, -nRange * 2, nRange * 2);
        else
            glOrtho(-nRange * winw / winh, nRange * winw / winh, -nRange, nRange, -nRange * 2, nRange * 2);
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.0f, 1.0f, 0.0f);
        if (linepoint.size() > 0) {
            CertainlineGL();
        }
        if (vpt.size() > 0 && vpt.size() < 4) {
            ControlPoint(vpt); //绘制鼠标点击的点
        }
        if (bDraw)
        {
            glColor3f(1.0f, 1.0f, 0.0f);
            PolylineGL(pt); //render curve
        }
        //画中轴
        glBegin(GL_LINE_STRIP);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(0, 200.0f, 200.0f);
        glVertex3f(0, -200.0f, 200.0f);
        glEnd();
        glutSwapBuffers();
    }
    else {
        GLfloat mat_specular[] = { 1.0,1.0,1.0,1.0 };
        GLfloat mat_shininess[] = { 128.0 };

        GLfloat light_position[] = { 500.0,500.0,500.0,1.0 };//1.0表示光源为点坐标x,y,z
        GLfloat white_light[] = { 1.0,1.0,1.0,1.0 };   //光源的颜色
        GLfloat lmodel_ambient[] = { lightpow,lightpow,lightpow,1.0 };//微弱环境光，使物体可见
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glShadeModel(GL_SMOOTH);//GL_SMOOTH


        glLightfv(GL_LIGHT0, GL_POSITION, light_position);//光源编号-7，光源特性，参数数据
        glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
        glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); //指定全局的环境光，物体才能可见//*/

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, (GLfloat)winw / (GLfloat)winh, 100.0f, 2000.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(pos_x, pos_y, pos_z, 100 * cos(look_y_angle * GL_PI / 180) * sin(look_x_angle * GL_PI / 180), 100 * sin(look_y_angle * GL_PI / 180), 100 * cos(look_y_angle * GL_PI / 180) * cos(look_x_angle * GL_PI / 180), 0.0, 1.0, 0.0);
        GLfloat x, y, z, angle; // 存储坐标和角度
        GLfloat fx, fy, fz, r, h;//法向量
         // 用当前颜色清除窗口

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);
        // 保存矩阵状态并进行旋转
        // 对所有剩余的点只调用一次

        glEnable(GLUT_DEPTH);
        glEnable(GL_TEXTURE_2D);
        if (whattex == 1) {
            glBindTexture(GL_TEXTURE_2D, texc1);
        }
        if (whattex == 2) {
            glBindTexture(GL_TEXTURE_2D, texc2);
        }
        if (whattex == 3) {
            glBindTexture(GL_TEXTURE_2D, texw1);
        }
        if (whattex == 4) {
            glBindTexture(GL_TEXTURE_2D, texw2);
        }
        if (whattex == 5) {
            glBindTexture(GL_TEXTURE_2D, texsh1);
        }
        if (whattex == 6) {
            cout << "6";
            glBindTexture(GL_TEXTURE_2D, texsh2);
        }
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };  //定义材质的漫反射光颜色，偏蓝色
        GLfloat specular[] = { 1.0f,1.0f, 1.0f, 1.0f };   //定义材质的镜面反射光颜色，蓝色
        GLfloat shininess = 100.0f;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        glBegin(GL_LINE_STRIP);
        int num = (linepoint.size() / 100) + 1;
        for (angle = 0.0f; angle <= 360.0f; angle += 0.2f)
        {
            for (int i = 0; i < linepoint.size(); i += num)
            {
                if ((i + num) <= linepoint.size()) {
                    r = linepoint[i].objy - linepoint[i + num - 1].objy;
                    h = linepoint[i + num - 1].objx - linepoint[i].objx;
                    fx = r * sin(angle * GL_PI / 180);
                    fz = r * cos(angle * GL_PI / 180);
                    fy = h;
                }
                else {
                    r = linepoint[i - num].objy - linepoint[i].objy;
                    h = linepoint[i].objx - linepoint[i - num].objx;
                    fx = r * sin(angle * GL_PI / 180);
                    fz = r * cos(angle * GL_PI / 180);
                    fy = h;
                }
                x = fabs(linepoint[i].objx) * sin(angle * GL_PI / 180);
                y = linepoint[i].objy;
                z = fabs(linepoint[i].objx) * cos(angle * GL_PI / 180);
                glVertex3f(x, y, z);
                glNormal3f(fx, fy, fz);
                glTexCoord2f(angle / 360.0f, (linepoint[i].objy - linepoint[lowestnum].objy) / linepoint[highestnum].objy - linepoint[lowestnum].objy);
            }

        }

        glEnd();
        if (particleflag == true) {
            w.Update();
            particleflag = false;
        }
        w.Render();

        DrawSky();
        // 恢复变换

        // 刷新绘图命令
        glutSwapBuffers();
    }
}




// 在渲染环境下进行初始化工作
void SetupRC()
{
    // 黑色背景
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 绘图颜色为绿色
    glColor3f(0.0f, 1.0f, 0.0f);
}

// 键盘箭头键控制旋转图形
void SpecialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        look_y_angle += 0.5f;

    if (key == GLUT_KEY_DOWN)
        look_y_angle -= 0.5f;

    if (key == GLUT_KEY_LEFT)
        look_x_angle -= 0.5f;

    if (key == GLUT_KEY_RIGHT)
        look_x_angle += 0.5f;


    if (key == GLUT_KEY_F1)
        whatsky = 1;
    if (key == GLUT_KEY_F2)
        whatsky = 2;
    if (key == GLUT_KEY_F3)
        whattex = 1;
    if (key == GLUT_KEY_F4)
        whattex = 2;
    if (key == GLUT_KEY_F5)
        whattex = 3;
    if (key == GLUT_KEY_F6)
        whattex = 4;
    if (key == GLUT_KEY_F7) 
    {
       whattex = 5;
    }

        

  
    
    
    if (look_x_angle > 360.0f)
        look_x_angle = 0.0f;

    if (look_x_angle <= -0.5f)
        look_x_angle = 360.0f;

    if (look_y_angle > 360.0f)
        look_y_angle = 0.0f;

    if (look_y_angle <= -0.5f)
        look_y_angle = 360.0f;

    // 刷新窗口
    glutPostRedisplay();
}

// 修改可视区域的视口，当窗口改变时被调用


void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseLeftIsDown = true;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        mouseLeftIsDown = false;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        mouseRightIsDown = true;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        mouseRightIsDown = false;
    }
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        Point obj = caculateObject(x, y);
        if (state == GLUT_DOWN)
        {
            if (nInput == 0)
            {

                if (linenum == 0) {
                    if (fabs(obj.objx) < 3) {
                        pt[0].objx = 0;
                        pt[0].objy = obj.objy;
                        pt[0].objz = 200.0f;
                        firstpoint.objx = obj.objx;
                        firstpoint.objy = obj.objy;
                        nInput = 1;//第一个点
                        vpt.clear();//清空栈
                        vpt.push_back(pt[0]); //将vpt的数据发送到pt点的数组之中
                        bDraw = false; //对于点击数量不足4，全部不允许绘制多项式曲线
                        glutPostRedisplay();//重新构图
                    }
                }
                else
                {
                    if (caculateSquareDistance(x, y, lastpointlist[linenum - 1]) < 3) {
                        pt[0].objx = lastpointlist[linenum - 1].objx;
                        pt[0].objy = lastpointlist[linenum - 1].objy;
                        pt[0].objz = 200.0f;
                        nInput = 1;//第一个点
                        vpt.clear();//清空栈
                        vpt.push_back(pt[0]); //将vpt的数据发送到pt点的数组之中
                        bDraw = false; //对于点击数量不足4，全部不允许绘制多项式曲线
                        glutPostRedisplay();//重新构图
                    }
                }
            }
            else if (nInput == 1) //第二个点
            {
                pt[1].objx = obj.objx;
                pt[1].objy = obj.objy;
                pt[1].objz = 200.0f;
                vpt.push_back(pt[1]);
                nInput = 2;
                glutPostRedisplay();//重新构图
            }
            else if (nInput == 2)//第三个点
            {
                pt[2].objx = obj.objx;
                pt[2].objy = obj.objy;
                pt[2].objz = 200.0f;
                vpt.push_back(pt[2]);
                nInput = 3;
                glutPostRedisplay();
            }
            else if (nInput == 3)//第四个点
            {
                if (fabs(obj.objx) < 3) {
                    pt[3].objx = 0;
                    pt[3].objy = obj.objy;
                    pt[3].objz = 200.0f;
                    lastpoint.objx = obj.objx;
                    lastpoint.objy = obj.objy;
                    lastpoint.objz = 200.0f;
                    bDraw = true;
                    vpt.push_back(pt[3]);
                    nInput = 4;
                    glutPostRedisplay();
                }
                else {
                    pt[3].objx = obj.objx;
                    pt[3].objy = obj.objy;
                    pt[3].objz = 200.0f;
                    lastpoint.objx = obj.objx;
                    lastpoint.objy = obj.objy;
                    lastpoint.objz = 200.0f;
                    bDraw = true;
                    vpt.push_back(pt[3]);
                    nInput = 4;
                    glutPostRedisplay();
                }
            }
            else if (nInput == 4)//第四个点
            {
                ReControlPoint(obj.objx, obj.objy, 200.0f); //绘制鼠标点击的点//pt[0].x =x; pt[0].y = y;
                if (caculateSquareDistance(x, y, pt[1]) < 3)
                {
                    whichpoint = 1;
                }
                else if (caculateSquareDistance(x, y, pt[2]) < 3)
                {
                    whichpoint = 2;
                }
                else if (caculateSquareDistance(x, y, pt[3]) < 3)
                {
                    whichpoint = 3;
                }
                else {
                    whichpoint = 4;
                }

            }
        }
        break;
    default:
        break;
    }
}

void savefile() {
    struct tm* t;
    time_t tt;
    time_t ts;
    string now;
    struct tm tr = { 0 };
    time(&tt);
    t = localtime(&tt);
    now = "Model" + to_string(t->tm_year + 1900) + "_" + to_string(t->tm_mon + 1) + "_" + to_string(t->tm_mday) + "_" + to_string(t->tm_hour) + "_" + to_string(t->tm_min) + "_" + to_string(t->tm_sec) + ".txt";
    cout << now;
    ofstream outfile;//创建一个ofstream对象
    outfile.open(now);
    std::vector<Point>::iterator it = linepoint.begin();
    for (; it != linepoint.end(); ++it)
    {
        outfile << "x:" << (*it).objx << "y:" << (*it).objy << endl;
    }
    outfile.close();
}
string TCHAR2STRING(TCHAR* str) {
    std::string strstr;
    try
    {
        int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

        char* chRtn = new char[iLen * sizeof(char)];

        WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

        strstr = chRtn;
    }
    catch (exception e)
    {
    }

    return strstr;
}

void loadfile() {
    OPENFILENAME ofn = { 0 };
    TCHAR strFileName[MAX_PATH] = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = TEXT("All\0*.*\0jpg\0*.jpg\0bmp\0*.bmp\0\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = strFileName;
    ofn.nMaxFile = sizeof(strFileName);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = TEXT("请选择一个文件");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    if (GetOpenFileName(&ofn)) {
        string filePath = TCHAR2STRING(strFileName);
        int start = filePath.find_last_of('\\');
        int end = filePath.find_last_of('.');
        string fileName = filePath.substr(start + 1, end - start - 1);
        ifstream file;
        file.open(filePath, ios::in);

        if (!file.is_open())return;
        string strLine;
        while (getline(file, strLine))
        {
            if (strLine.empty())
                continue;
            int pos = strLine.find("y");
            string	newStr1 = strLine.substr(2, pos - 2);
            string  newStr2 = strLine.substr(pos + 2);
            double ox = atof(newStr1.c_str());
            double oy = atof(newStr2.c_str());
            Point a;
            a.objx = ox; a.objy = oy; a.objz = 200.0f;
            linepoint.push_back(a);
        }
        getLowestpoint();
        getHighestpoint();
        firstpoint = linepoint.front();
        lastpoint = linepoint.back();
        show = true;
    }
    else {
        MessageBox(NULL, TEXT("请选择一文件"), NULL, MB_ICONERROR);
    }
}

void keyboard(unsigned char key, int x, int y)
{
    if(key==27){
        exit(0);
    }
    switch (key)
    {
    case 'w': case 'W': {
        pos_z -= 10.0f;
        break;
    }
    case 's': case 'S': {
        pos_z += 10.0f;
        break;
    }
    case 'a': case 'A': {
        pos_x += 10.0f;
        break;
    }
    case 'd': case 'D': {
        pos_x -= 10.0f;
        break;
    }
    case 'z': case 'Z': {
        pos_y += 10.0f;
        break;
    }
    case 'x': case 'X': {
        pos_y -= 10.0f;
        break;
    }
    case 'q': case 'Q': {
        if (nInput == 0) {
            if (linenum > 0) {
                linenum--;
                vector<Point>::iterator it = linepoint.begin();
                lastpointlist.pop_back();
                auto iter = linepoint.erase(it + linenum * 50, linepoint.end());
            }
        }
        if (nInput != 0) {
            temp_linepoint.clear();
            nInput = 0;
            bDraw = false;
            vpt.clear();
        }
        break;
    }
    case 'e': case 'E': {
        if (nInput == 4) {
            linenum++;
            linepoint.insert(linepoint.end(), temp_linepoint.begin(), temp_linepoint.end());
            bDraw = false;
            vpt.clear();
            nInput = 0;
            lastpointlist.push_back(lastpoint);
            temp_linepoint.clear();
        }
        break;
    }
    case 'j': case 'J': {
        getLowestpoint();
        getHighestpoint();
        show = true;
        break;
    }
    case 'r': case 'R': {
        loadfile();
        break;
    }
    case 'm': case 'M': {
        lightpow += 0.1;
        cout << lightpow;
        break;
    }
    case 'n': case 'N': {
        lightpow -= 0.1;
        cout << lightpow;
        break;
    }
    case 'b': case 'B': {
        particleflag = true;
        break;
    }
    case 'f': case 'F': {
        if (linepoint.size() != 0) savefile();
        else  MessageBox(NULL, TEXT("没有可保存的模型"), NULL, MB_ICONERROR);
        break;

    }

    case 'g': case 'G': {
        show = false;
        linepoint.clear();
        temp_linepoint.clear();
        lastpointlist.clear();
        vpt.clear();
        bDraw = false;
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GLUT_DEPTH);
        glDisable(GL_TEXTURE_2D);
        linenum = 0;
        nInput = 0;
        break;
    }
            

    default: break;
    }

}
GLfloat diffuseMaterial[4] = { 0.5, 0.5, 0.5, 1.0 };
//GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.5 };;//光源位置
GLfloat light_position[] = { 1.0,1.0,0,0 };;//光源位置
GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };






void motion(int x, int y)       //移动点
{
    Point obj = caculateObject(x, y);
    if (mouseLeftIsDown)        //左键移动控制点
    {
        if (whichpoint == 2 || whichpoint == 1) {
            pt[whichpoint].objx = obj.objx;
            pt[whichpoint].objy = obj.objy;
        }
        if (whichpoint == 3) {
            if (fabs(obj.objx) < 3) {
                pt[3].objx = 0;
                pt[3].objy = obj.objy;
            }
            else {
                pt[3].objx = obj.objx;
                pt[3].objy = obj.objy;
            }
            lastpoint.objx = pt[3].objx;
            lastpoint.objy = pt[3].objy;
            lastpoint.objz = 200.0f;
        }
    }
    else if (mouseRightIsDown) { //有需要的话，在这里更改按下鼠标右键的事件
        // Display(points);
        // Display(p0, p1, p2, p3);
    }
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();        //重新构图
}

void ChangeSize(int w, int h)
{
    // 防止被0除
    if (h == 0)
        h = 1;
    winw = w;
    winh = h;
    // 将视口设置为窗口尺寸
}



void main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutInitWindowPosition(argc, argc);
    glutInitWindowSize(800, 800);
    glutCreateWindow("cg_project");

    glEnable(GL_TEXTURE_2D);    // 启用纹理

    tex2 = load_texture("huaban.bmp");
    texs1 = load_texture("sky.bmp");
    texf1 = load_texture("floor.bmp");
    texs2 = load_texture("sky2.bmp");
    texf2 = load_texture("floor2.bmp");
    texc1 = load_texture("china.bmp");
    texc2 = load_texture("china2.bmp");


    texsh1 = load_texture("stone.bmp");
    texsh2 = load_texture("stone2.bmp");

    texw1 = load_texture("wood.bmp");
    texw2 = load_texture("wood2.bmp");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(myDisplay);
    glutIdleFunc(myDisplay);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    SetupRC();
    glutMainLoop();
}
