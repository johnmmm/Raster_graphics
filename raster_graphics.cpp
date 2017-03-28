//
//  main.cpp
//  myfirstopencvpro
//
//  Created by mac on 17/3/7.
//  Copyright © 2017年 mac. All rights reserved.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <cv.h>
#include <cxcore.h>
#include <stack>
#include <cmath>
#include <math.h>
using namespace std;
using namespace cv;

int num = 0;
double xp[10] = {0, (double)1/6, (double)1/2, (double)5/6, (double)1/6, (double)1/2,(double) (double)5/6, 1/6, (double)1/2, (double)5/6};
double yp[10] = {0, (double)5/6, (double)5/6, (double)5/6, (double)1/2, (double)1/2, (double)1/2, (double)1/6, (double)1/6, (double)1/6};
double quan[10] = {0, 1, 2, 1, 2, 4, 2, 1, 2, 1}; // 加权区域采样的权值

typedef struct RGB                          //创建一个RGB结构
{
    int r;
    int g;
    int b;
    RGB(int R, int G, int B)
    {
        r = R;
        g = G;
        b = B;
    }
}rgb;

Mat M(800,600,CV_8UC3);                     //创建一个高800，宽600的Mat对象

void paintpoint(int x, int y, double k, double c, RGB color, bool rev, int sym)//只处理k是正的且大于0小于1的
{
    //sym来处理k是负的时候
    if(abs(k * x - y + c) / sqrt(k * k + 1) > 1)
        return;
    double intensity;
    intensity = 1;

    if(k * x - y + c + 1 - abs(k) <= 0)     //上线之上，为e
    {
        double y01, y02;
        y01 = k * x + c + 1 - abs(k);
        y02 = k * (x + 1) + c + 1 - abs(k);
        intensity = (y02 - y) * (y02 - y) / (2 * k);
        if(intensity > 1)
            return;
    }
    else if(k * x - y + c - abs(k) >= 0)    //下线之下，为a或b
    {
        double y01, y02;
        y01 = k * x + c - abs(k);
        y02 = k * (x + 1) + c - abs(k);
        if(y01 - y < 1 && y02 - y > 1)  //a
            intensity = (y + 1 - y01) * (y + 1 - y01) / (2 * k);
        else   //b
        {
            intensity = (2 + 2 * y - y01 - y02) / 2;
        }
        if(intensity > 1)
            return;
    }
    else                                    //中间的肯定就是cd型了
    {
        intensity = 1 ;
        double y01, y02, y11, y12;
        y01 = k * x + c + 1 - abs(k);
        y02 = k * (x + 1) + c + 1 - abs(k);
        y11 = k * x + c - abs(k);
        y12 = k * (x + 1) + c - abs(k);
        if(y01 > y && y02 > y && y11 <= y && y12 <= y)
            intensity = (y01 + y02 - y * 2) / 2;
        else
            intensity = 1 - (y + 1 - y01) * (y + 1 - y01) / (2 * k) - (y12 - y) * (y12 - y) / (2 * k);
    }
    
    if(rev)//判定
    {
        if(sym == 0)
            M.at<Vec3b>(y, x) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
        else
            M.at<Vec3b>(y, 2 * sym - x) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
    }
    else
    {
        if(sym == 0)
            M.at<Vec3b>(x, y) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
        else
            M.at<Vec3b>(x, 2 * sym - y) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
    }
}

void paintparapoint(int x0, int y0, int x1, int y1, RGB color)
{
    if(x0 >= x1 && y0 >= y1)                //面对反着的情况
    {
        int tmp = y1;
        y1 = y0;
        y0 = tmp;
        tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    
    if(x0 == x1)
    {
        for(int i = y0; i <= y1; i++)
        {
            M.at<Vec3b>(x0, i) = Vec3b(color.r, color.g, color.b);
        }
    }
    else
    {
        for(int i = x0; i <= x1; i++)
        {
            M.at<Vec3b>(i, y0) = Vec3b(color.r, color.g, color.b);
        }
    }
}

void IntergerBresenhamline(int x0, int y0, int x1, int y1, RGB color)
{
    if(x0 == x1 || y0 == y1)
    {
        paintparapoint(x0, y0, x1, y1, color);
        return;
    }
    
    double k, c;
    bool rev;
    int sym = 0;
    rev = false;
    k = (double)(y1 - y0) / (x1 - x0);
    if(k < 0)                               //判定k小于0，直接反过来
    {
        sym = y0;
        y1 = 2 * sym - y1;
    }
    k = (double)(y1 - y0) / (x1 - x0);
    if(k > 1)                               //开始判定是否需要反向画图， 因此k肯定小于1
    {
        rev = true;
        int tmp = y1;
        y1 = x1;
        x1 = tmp;
        tmp = x0;
        x0 = y0;
        y0 = tmp;
        k = (double)(y1 - y0) / (x1 - x0);
    }
    c = (double)(y1 - k * x1);
    
    if(x0 >= x1 && y0 >= y1)                //面对反着的情况
    {
        int tmp = y1;
        y1 = y0;
        y0 = tmp;
        tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    
    int x, y, dx, dy, e;
    dx = x1 - x0;
    dy = y1 - y0;
    e = -dx;
    x = x0;
    y = y0;
    for(int i = 0; i < dx; i++)
    {
        paintpoint(x, y, k, c, color, rev, sym);
        paintpoint(x+1, y, k, c, color, rev, sym);
        paintpoint(x, y+1, k, c, color, rev, sym);
        paintpoint(x-1, y, k, c, color, rev, sym);
        paintpoint(x, y-1, k, c, color, rev, sym);
        x++;
        e = e + 2 * dy;
        if(e >= 0)
        {
            y++;
            e = e - 2 * dx;
        }
    }
}

void paintbyquan(int x, int y, double k, double c, RGB color, bool rev, int sym)//用加权区域采样
{
    //sym来处理k是负的时候
    double intensity;
    intensity = 0;
    
    for(int i = 1; i <= 9; i++)
    {
        if(k * (xp[i]+x) - (yp[i]+y) + c + 1 - k >= 0 && k * (xp[i]+x) - (yp[i]+y) + c - k <= 0)
            intensity += quan[i];
    }
    intensity = intensity / 16;
    
    if(intensity == 0)
        return;
    
    if(rev)//判定
    {
        if(sym == 0)
            M.at<Vec3b>(y, x) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
        else
            M.at<Vec3b>(y, 2 * sym - x) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
    }
    else
    {
        if(sym == 0)
            M.at<Vec3b>(x, y) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
        else
            M.at<Vec3b>(x, 2 * sym - y) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));
    }
}

void IntergerBresenhamlineJia(int x0, int y0, int x1, int y1, RGB color)//加权区域采样反走样
{
    if(x0 == x1 || y0 == y1)
    {
        paintparapoint(x0, y0, x1, y1, color);
        return;
    }
    
    double k, c;
    bool rev;
    int sym = 0;
    rev = false;
    k = (double)(y1 - y0) / (x1 - x0);
    if(k < 0)                               //判定k小于0，直接反过来
    {
        sym = y0;
        y1 = 2 * sym - y1;
    }
    k = (double)(y1 - y0) / (x1 - x0);
    if(k > 1)                               //开始判定是否需要反向画图， 因此k肯定小于1
    {
        rev = true;
        int tmp = y1;
        y1 = x1;
        x1 = tmp;
        tmp = x0;
        x0 = y0;
        y0 = tmp;
        k = (double)(y1 - y0) / (x1 - x0);
    }
    c = (double)(y1 - k * x1);
    
    if(x0 >= x1 && y0 >= y1)                //面对反着的情况
    {
        int tmp = y1;
        y1 = y0;
        y0 = tmp;
        tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    
    int x, y, dx, dy, e;                    //按照原来的方法画线
    dx = x1 - x0;
    dy = y1 - y0;
    e = -dx;
    x = x0;
    y = y0;
    for(int i = 0; i < dx; i++)
    {
        paintbyquan(x, y, k, c, color, rev, sym);
        paintbyquan(x+1, y, k, c, color, rev, sym);
        paintbyquan(x, y+1, k, c, color, rev, sym);
        paintbyquan(x-1, y, k, c, color, rev, sym);
        paintbyquan(x, y-1, k, c, color, rev, sym);
        x++;
        e = e + 2 * dy;
        if(e >= 0)
        {
            y++;
            e = e - 2 * dx;
        }
    }
}

void CirclePoint(int xx, int yy, int x, int y, RGB color, int r)//xx，yy为圆心
{
    double intensity = 0;
    x = (double)x;
    y = (double)y;
    r = (double)r;
    for(int i = 1; i <= 9; i++)
    {
        if((x+xp[i])*(x+xp[i])+(y+yp[i])*(y+yp[i]) >= (r-0.5)*(r-0.5) && (x+xp[i])*(x+xp[i])+(y+yp[i])*(y+yp[i]) <= (r+0.5)*(r+0.5))
            intensity += quan[i];
    }
    intensity = intensity / 16;
    
    M.at<Vec3b>(x+xx, y+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(y+xx, x+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(-x+xx, y+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(y+xx, -x+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(x+xx, -y+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(-y+xx, x+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(-x+xx, -y+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
    M.at<Vec3b>(-y+xx, -x+yy) = Vec3b(255-intensity*(255-color.r), 255-intensity*(255-color.g), 255-intensity*(255-color.b));;
}

void DrawPointCircle(int xx, int yy, int r, RGB color)
{
    int x, y;
    float d;
    x = 0;
    y = r;
    d = 1.25 - r;
    CirclePoint(xx, yy, x, y, color, r);
    CirclePoint(xx, yy, x+1, y, color, r);
    CirclePoint(xx, yy, x, y+1, color, r);
    CirclePoint(xx, yy, x-1, y, color, r);
    CirclePoint(xx, yy, x, y-1, color, r);
    while(x <= y)
    {
        if(d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        CirclePoint(xx, yy, x, y, color, r);
        CirclePoint(xx, yy, x+1, y, color, r);
        CirclePoint(xx, yy, x, y+1, color, r);
        CirclePoint(xx, yy, x-1, y, color, r);
        CirclePoint(xx, yy, x, y-1, color, r);
    }
}

typedef struct Seeds
{
    int x;
    int y;
}Seed;

stack<Seeds>s;                              //建立一个栈

void ScanLineFill4(int x, int y, RGB oldcolor, RGB newcolor)
{
    int xl, xr;
    bool spanNeedFill;
    Seed pt;
    //Stack s;
    while(!s.empty())
        s.pop();
    pt.x = x;
    pt.y = y;
    s.push(pt);                             //将前面生成的区段压入堆栈
    while(!s.empty())
    {
        pt = s.top();
        s.pop();
        y = pt.y;
        x = pt.x;
        while(M.at<Vec3b>(x, y) == Vec3b(oldcolor.r, oldcolor.g, oldcolor.b))
        {
            M.at<Vec3b>(x, y) = Vec3b(newcolor.r, newcolor.g, newcolor.b);
            x++;
        }
        xr = x - 1;
        x = pt.x - 1;
        while(M.at<Vec3b>(x, y) == Vec3b(oldcolor.r, oldcolor.g, oldcolor.b))
        {
            M.at<Vec3b>(x, y) = Vec3b(newcolor.r, newcolor.g, newcolor.b);
            x--;
        }
        xl = x + 1;
        
        //处理上面一条扫描线
        x = xl;
        y = y + 1;
        while(x <= xr)
        {
            spanNeedFill = false;
            while(M.at<Vec3b>(x, y) == Vec3b(oldcolor.r, oldcolor.g, oldcolor.b))
            {
                spanNeedFill = true;
                x++;
            }
            if(spanNeedFill)
            {
                pt.x = x - 1;
                pt.y = y;
                s.push(pt);
                spanNeedFill = false;
            }
            while(M.at<Vec3b>(x, y) != Vec3b(oldcolor.r, oldcolor.g, oldcolor.b))
                x++;
        }
        
        x = xl;                             //处理下面一条扫描线，代码与处理上面一条扫描线类似
        y = y - 2;
        while(x <= xr)
        {
            spanNeedFill = false;
            while(M.at<Vec3b>(x, y) == Vec3b(oldcolor.r, oldcolor.g, oldcolor.b))
            {
                spanNeedFill = true;
                x++;
            }
            if(spanNeedFill)
            {
                pt.x = x - 1;
                pt.y = y;
                s.push(pt);
                spanNeedFill = true;
            }
            while (M.at<Vec3b>(x, y) != Vec3b(oldcolor.r, oldcolor.g, oldcolor.b))
                x++;
        }
    }
}


int main(int argc, char* argv[])
{
    namedWindow("Test");                    //创建一个名为Test窗口
    
    for (int i = 0; i < M.rows; i++)        //遍历每一行每一列并设置其像素值
    {
        for (int j = 0; j < M.cols; j++)
        {
            M.at<Vec3b>(i,j) = Vec3b(255, 255, 255);
        }
    }

    IntergerBresenhamline(400, 300, 200, 150, RGB(0, 255, 0));//普通区域采样
    IntergerBresenhamline(400, 300, 250, 100, RGB(0, 255, 0));
    IntergerBresenhamline(400, 300, 600, 450, RGB(0, 255, 0));
    IntergerBresenhamline(400, 300, 550, 500, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 550, 100, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 600, 150, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 200, 450, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 250, 500, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 650, 300, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 150, 300, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 400, 550, RGB(0, 255, 0));
//    IntergerBresenhamline(400, 300, 400, 50, RGB(0, 255, 0));
    
//    IntergerBresenhamlineJia(400, 300, 200, 150, RGB(0, 255, 0));
//    IntergerBresenhamlineJia(400, 300, 250, 100, RGB(0, 255, 0));
//    IntergerBresenhamlineJia(400, 300, 600, 450, RGB(0, 255, 0));
//    IntergerBresenhamlineJia(400, 300, 550, 500, RGB(0, 255, 0));
    IntergerBresenhamlineJia(400, 300, 550, 100, RGB(255, 255, 0));//加权区域采样
    IntergerBresenhamlineJia(400, 300, 600, 150, RGB(255, 255, 0));
    IntergerBresenhamlineJia(400, 300, 200, 450, RGB(255, 255, 0));
    IntergerBresenhamlineJia(400, 300, 250, 500, RGB(255, 255, 0));
    IntergerBresenhamlineJia(400, 300, 650, 300, RGB(128, 255, 128));
    IntergerBresenhamlineJia(400, 300, 150, 300, RGB(128, 255, 128));
    IntergerBresenhamlineJia(400, 300, 400, 550, RGB(128, 255, 128));
    IntergerBresenhamlineJia(400, 300, 400, 50, RGB(128, 255, 128));
    
    DrawPointCircle(400, 300, 250, RGB(255, 100, 100));
    
    ScanLineFill4(399, 200, RGB(255, 255, 255), RGB(0,0,255));
    ScanLineFill4(399, 400, RGB(255, 255, 255), RGB(100,228,255));
    
    imshow("Test",M);                       //窗口中显示图像
    imwrite("/Users/mac/Desktop/myself/pictures/my space/test.jpg",M);    //保存生成的图片
    waitKey();                              //等待
    
    return 0;
}



