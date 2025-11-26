#pragma once
#include"utils.h"

#define laplacian_limit -1.0

void sweep_controller(var_t* distances, bool* frozen);
void fast_sweep(var_t* distances, bool* frozen,int,int,int,int,int,int);

//Obtaining rough medial axis
void smooth_distance_field(var_t* distances);
void compute_laplacian(var_t* distances, var_t * laplacian);
void filter_medial_axis(var_t* laplacian, bool* filtered_laplacian);


//Thinning the medial axis
std::array<int,4> thinning_heuristics(int idx, int idy, bool* filtered_laplacian, bool is_even);
void thin_medial_axis(bool* filtered_laplacian, bool is_even);
