#pragma once
#include<cmath>
#include<string.h>
#include<fstream>
#include<vector>
#include<iostream>
#include<omp.h>
#include<array>
#include<algorithm>

#define SMALL 1E-14
#define LARGE 1E10

#ifndef M_PI
#define M_PI 3.14159265358979323846 
#endif

typedef float var_t;

extern int num_iblines;
extern int N;
extern int maxDepth;
extern var_t box_size;
extern var_t delta;

var_t dot_product(var_t vec1[],var_t vec2[]);
void cross_product(var_t vec1[],var_t vec2[],var_t ans[]);
var_t norm(var_t vec[]);

struct line{
      var_t st[2];
      var_t en[2];
      var_t normal[2];
      int body_ID=0; //In case of complicated objects multiple bodies may be present
};

//-----Quadtree data structures-----------
struct Box2 {
    var_t x,y,size;
    Box2(var_t a=0.0,var_t b=0.0,var_t s=1.0):x(a),y(b),size(s){}
};

// Distance from point (px,py) to line segment (x1,y1)-(x2,y2)
var_t pointToSegmentDistance(var_t px,var_t py,struct line& l);

// Check if box intersects polygon using distance
std::pair<bool,var_t> boxIntersectsPolygonDist(const Box2& b,std::vector<struct line>& poly,var_t eps=0.01);

struct Quadtree2{
    Box2 box;
    int depth;
    bool isLeaf=true;
    std::vector<Quadtree2*> children;

    Quadtree2(Box2 b,int d=0):box(b),depth(d){}

    void build(std::vector<struct line>& poly,int frozen[],var_t distances[], int body_ID[], var_t errf);
    void write(Quadtree2& tree);
    void delete_tree(Quadtree2& tree);

};

//---------Grid management-------------
void write_grid();
void write_soln(var_t distances[],var_t laplacian[], int filtered_laplacian[],int frozen[], int body_ID[]);
