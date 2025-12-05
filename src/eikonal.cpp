#include"eikonal.h"

void sweep_controller(var_t* distances, int* frozen){
        fast_sweep(distances,frozen,0,N-1,1,0,N-1,1);
        fast_sweep(distances,frozen,N-2,-1,-1,0,N-1,1);
        fast_sweep(distances,frozen,N-2,-1,-1,N-2,-1,-1);
        fast_sweep(distances,frozen,0,N-1,1,N-2,-1,-1);
}

void fast_sweep(var_t* distances,int* frozen,int i_min,int i_max,int i_step,int j_min,int j_max,int j_step){
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
                        if(frozen[i+j*(N-1)]==1){
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

void define_ROI(int curr_body, int* body_ID, var_t* distances, var_t* distances_old){
        
        #pragma omp parallel for collapse(2)
        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        if(distances_old[i+(N-1)*j]<distances[i+(N-1)*j]){
                                distances[i+(N-1)*j]=distances_old[i+(N-1)*j];
                                body_ID[i+(N-1)*j]=curr_body;
                        }
                }
        }

}

void smooth_distance_field(var_t* distances){
        var_t d_left,d_right,d_top,d_bottom,d_centre;
        int i,j,i_left,i_right,j_top,j_bottom;
        var_t temp;

        #pragma omp parallel for collapse(2) private(i_left,i_right,j_bottom,j_top,d_left,d_right,d_bottom,d_top,d_centre)
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
                        
                        distances[i+(N-1)*j]=0.25*(d_left+d_right+d_top+d_bottom);
                }
        }

}

void compute_laplacian(var_t* distances, var_t * laplacian){
        var_t d_left,d_right,d_top,d_bottom,d_centre;
        int i,j,i_left,i_right,j_top,j_bottom;
        var_t temp;

        #pragma omp parallel for collapse(2) private(i_left,i_right,j_bottom,j_top,d_left,d_right,d_bottom,d_top,d_centre)
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
                }
        }

}

void filter_medial_axis(var_t* laplacian, int* filtered_laplacian, var_t laplacian_limit){

        #pragma omp parallel for collapse(2)
        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        filtered_laplacian[i+(N-1)*j]=(laplacian[i+(N-1)*j]<=laplacian_limit)?1:0;
                }
        }

}

//Thinning the medial axis
std::array<int,4> thinning_heuristics(int idx, int idy,int* filtered_laplacian,bool is_even){
       bool x[9];
       int incs_x[]={0,-1,0,1,1,1,0,-1,-1};
       int incs_y[]={0,1,1,1,0,-1,-1,-1,0};
       int temp_incs_x,temp_incs_y;
       int b[4];
       int n1,n2;
       std::array<int,4> ans; 
       
       for(int i=0;i<9;i++){
               temp_incs_x=(idx+incs_x[i])>N-1?0:incs_x[i];
               temp_incs_x=(idx+incs_x[i])<0?0:incs_x[i];

               temp_incs_y=(idy+incs_y[i])>N-1?0:incs_y[i];
               temp_incs_y=(idy+incs_y[i])<0?0:incs_y[i];

               x[i]=filtered_laplacian[idx+temp_incs_x+(N-1)*(idy+temp_incs_y)]==1?true:false;
       }

       ans[0]=(int)((!x[2]) && (x[3] || x[4]))+\
              (int)((!x[4]) && (x[5] || x[6]))+\
              (int)((!x[6]) && (x[7] || x[8]))+\
              (int)((!x[8]) && (x[1] || x[2]));

       ans[1]=(int)(x[1] || x[2])+\
              (int)(x[3] || x[4])+\
              (int)(x[5] || x[6])+\
              (int)(x[7] || x[8]);
       
       ans[2]=(int)(x[2] || x[3])+\
              (int)(x[4] || x[5])+\
              (int)(x[6] || x[7])+\
              (int)(x[8] || x[1]);

       if(!is_even){
              ans[3]=(int)((x[2] || x[3] || !x[5]) || x[4]);
       }
       else{
              ans[3]=(int)((x[6] || x[7] || !x[1]) && x[8]);
       }

       return ans;
       
}

void thin_medial_axis(int* filtered_laplacian, bool is_even){

        bool C1,C2,C3;
        std::array<int,4> heuristics;
        int temp;
        bool marked[(N-1)*(N-1)];

        #pragma omp parallel for collapse(2) private(heuristics,C1,temp,C2,C3)
        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        heuristics=thinning_heuristics(i,j,filtered_laplacian,is_even);
                        C1=(heuristics[0]==1);
                        temp=std::min(heuristics[1],heuristics[2]);
                        C2=(temp>=2 && temp<=3);
                        C3=(heuristics[3]==0);
                        marked[i+j*(N-1)]=C1 && C2 && C3;
                }
        }

        #pragma omp parallel for collapse(2)
        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        if(marked[i+j*(N-1)] || i==0 || i==N-2 || j==0 || j==N-2){
                                filtered_laplacian[i+j*(N-1)]=0;
                        }
                }
        }

}

