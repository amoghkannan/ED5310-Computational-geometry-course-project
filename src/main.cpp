#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include"utils.h"
#include"eikonal.h"

char ibfile_name[]="ib.dat"; 
int num_iblines;
int N;
int maxDepth;
var_t box_size;
var_t delta;

int main(int argc, char**argv){
        
        box_size=(var_t)std::stof(argv[1]);
        maxDepth=std::atoi(argv[2]);
        int n_bodies;
        n_bodies=std::atoi(argv[3]);
        N=std::pow(2,maxDepth)+1; 
        delta=box_size/(N-1);
        bool frozen[(N-1)*(N-1)];
        var_t distances[(N-1)*(N-1)]; //Distance array used for each object
        var_t laplacian[(N-1)*(N-1)];

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        distances[i+j*(N-1)]=LARGE;
                        frozen[i+j*(N-1)]=false;
                }
        }

        std::ifstream ibfile(ibfile_name);
        ibfile>>num_iblines;

        struct line iblines[num_iblines];

        for(int i=0;i<num_iblines;i++){
                ibfile>>iblines[i].st[0]>>iblines[i].st[1]>>iblines[i].en[0]>>iblines[i].en[1]>>\
                iblines[i].normal[0]>>iblines[i].normal[1];
        }

        //For the immersed bodies

        Quadtree qt(Box2(0.0f,0.0f,box_size),0);
        qt.build(iblines,frozen,distances);

        //For the walls

        //Upper wall
        for(int j=N-2;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        //Lower wall
        for(int j=0;j<1;j++){
                for(int i=0;i<N-1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        //Left wall
        for(int j=0;j<N-1;j++){
                for(int i=0;i<1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        //Right wall
        for(int j=0;j<N-1;j++){
                for(int i=N-2;i<N-1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        sweep_controller(distances,frozen);

        filter_medial_axis(distances,laplacian);

        //Write out tree partitions,grid,solution and then free heap-allocated memory
        qt.write(qt);
        write_grid();
        write_soln(distances,laplacian,frozen);
        qt.delete_tree(qt);
}

