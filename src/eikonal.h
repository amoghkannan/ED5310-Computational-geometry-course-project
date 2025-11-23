#pragma once
#include"utils.h"

#define laplacian_limit -1.0

void sweep_controller(var_t* distances, bool* frozen);
void fast_sweep(var_t* distances, bool* frozen,int,int,int,int,int,int);

void filter_medial_axis(var_t* distances, var_t* laplacian);
