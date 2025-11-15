#include"eikonal.h"

void sweep_controller(var_t* distances, bool* frozen){
        fast_sweep(distances,frozen,0,N-1,1,0,N-1,1);
        fast_sweep(distances,frozen,N-2,-1,-1,0,N-1,1);
        fast_sweep(distances,frozen,N-2,-1,-1,N-2,-1,-1);
        fast_sweep(distances,frozen,0,N-1,1,N-2,-1,-1);
}

void fast_sweep(var_t* distances, bool* frozen,int i_min,int i_max,int i_step,int j_min,int j_max,int j_step){
//i_max/j_max: 1 GREATER than necessary

        var_t f,h;
        h=delta;
        f=1.0;
        var_t d_left,d_right,d_top,d_bottom;
        var_t d_xmin,d_ymin;
        int i,j,i_left,i_right,j_top,j_bottom;
        i=i_min;
        j=j_min;

        var_t d_new;

        while(j!=j_max){
                i=i_min;
                while(i!=i_max){
                        if(frozen[i+j*(N-1)]){
                                i=i+i_step;
                                continue;
                        }
                        i_left=(i>=1)?i-1:i+1;
                        i_right=(i<=N-3)?i+1:i-1;
                        j_bottom=(j>=1)?j-1:j+1;
                        j_top=(j<=N-3)?j+1:j-1;

                        d_left=distances[i_left+j*(N-1)];
                        d_right=distances[i_right+j*(N-1)];
                        d_bottom=distances[i+j_bottom*(N-1)];
                        d_top=distances[i+j_top*(N-1)];

                        d_xmin=std::min(d_left,d_right);
                        d_ymin=std::min(d_bottom,d_top);

                        if(fabs(d_xmin-d_ymin)>=f*h){
                                d_new=std::min(d_xmin,d_ymin)+f*h;
                        }
                        else{
                                d_new=0.5*(d_xmin+d_ymin+sqrt(2.0*f*f*h*h-(d_xmin-d_ymin)*(d_xmin-d_ymin)));
                        }

                        distances[i+j*(N-1)]=std::min(distances[i+j*(N-1)],d_new);
                        i=i+i_step;
                        
                }
                j=j+j_step;
        }
}

void filter_medial_axis(var_t* distances, var_t* laplacian){

        var_t d_left,d_right,d_top,d_bottom,d_centre;
        int i,j,i_left,i_right,j_top,j_bottom;

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        i_left=(i>=1)?i-1:i;
                        i_right=(i<=N-3)?i+1:i;
                        j_bottom=(j>=1)?j-1:j;
                        j_top=(j<=N-3)?j+1:j;

                        d_left=distances[i_left+j*(N-1)];
                        d_right=distances[i_right+j*(N-1)];
                        d_bottom=distances[i+j_bottom*(N-1)];
                        d_top=distances[i+j_top*(N-1)];
                        d_centre=distances[i+j*(N-1)];
                        
         laplacian[i+(N-1)*j]=(d_left+d_right-2.0*d_centre)/(delta*delta)+(d_top+d_bottom-2.0*d_centre)/(delta*delta);
         laplacian[i+(N-1)*j]=(laplacian[i+(N-1)*j]<=laplacian_limit)?1.0:0.0;
                }
        }

}
