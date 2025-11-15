#pragma once
#include<cmath>
#include<string.h>
#include<fstream>
#include<vector>
#include<iostream>

#define SMALL 1E-14
#define LARGE 1E50

typedef double var_t;

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
var_t pointToSegmentDistance(var_t px,var_t py,struct line l);

// Check if box intersects polygon using distance
std::pair<bool,var_t> boxIntersectsPolygonDist(const Box2& b,const struct line poly[],var_t eps=0.01);

struct Quadtree{
    Box2 box;
    int depth;
    bool isLeaf=true;
    std::vector<Quadtree*> children;

    Quadtree(Box2 b,int d=0):box(b),depth(d){}

    void build(const struct line poly[],bool frozen[],var_t distances[]);
    void write(Quadtree& tree);
    void delete_tree(Quadtree& tree);

};

//---------Grid management-------------
void write_grid();
void write_soln(var_t distances[],var_t laplacian[],bool frozen[]);
