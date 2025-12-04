#pragma once
#include"utils.h"

void sweep_controller(var_t* distances, bool* frozen);
void fast_sweep(var_t* distances, bool* frozen,int,int,int,int,int,int);
void define_ROI(int curr_body, int* body_ID, var_t* distances, var_t* distances_old);

//Obtaining rough medial axis
void smooth_distance_field(var_t* distances);
void compute_laplacian(var_t* distances, var_t * laplacian);
void filter_medial_axis(var_t* laplacian, int* filtered_laplacian, var_t laplacian_limit);


//Thinning the medial axis
std::array<int,4> thinning_heuristics(int idx, int idy, int* filtered_laplacian, bool is_even);
void thin_medial_axis(int* filtered_laplacian, bool is_even);

//Pruning spurious/noisy branches
