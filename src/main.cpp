#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include"utils.h"
#include"eikonal.h"
#include"glad.h"
#include"glfw3.h"

char ibfile_name[]="ib.dat"; 
int num_iblines;
int N;
int maxDepth;
var_t box_size;
var_t delta;
var_t camX=0.0, camY=0.0, zoom=1.0;
bool dragging=false;
bool slider_dragging=false;
var_t lastX, lastY;
var_t sliderX=0.0;

var_t laplacian_limit=-1.0;
var_t laplacian_limit_old=laplacian_limit;

bool sliderHover(var_t xpos, var_t ypos){
        var_t x1, x2, y1, y2;
        var_t sliderWidth=0.1*box_size;

        x1=camX+(sliderX-0.5*sliderWidth)/zoom;
        x2=camX+(sliderX+0.5*sliderWidth)/zoom;
        y1=camY-0.95*0.5*box_size/zoom;
        y2=camY-0.90*0.5*box_size/zoom;
    
        if(xpos>=x1 && xpos<=x2 && ypos>=y1 && ypos<=y2){
                return true;
        }
        else{
                return false;
        }

}

void scroll_callback(GLFWwindow* window, var_t xoffset, var_t yoffset){
    zoom *= (yoffset>0?1.1f:0.9f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if(button==GLFW_MOUSE_BUTTON_RIGHT){
        if(action==GLFW_PRESS){
            dragging=true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else dragging=false;
    }
    if(button==GLFW_MOUSE_BUTTON_LEFT){
        if(action==GLFW_PRESS){
            slider_dragging=true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else slider_dragging=false;
    }

}

void cursor_pos_callback(GLFWwindow* window, var_t xpos, var_t ypos){
    if(dragging || slider_dragging){
        int w,h; glfwGetFramebufferSize(window,&w,&h);
        var_t dx=(xpos-lastX)/w*box_size/zoom;
        var_t dy=(lastY-ypos)/h*box_size/zoom;
        var_t sliderWidth=0.1*box_size;
        if(slider_dragging){
                sliderX=sliderX+zoom*dx;
                sliderX=std::min(-0.5*sliderWidth+0.95*0.5*box_size,std::max(sliderX,0.5*sliderWidth-0.95*0.5*box_size));
        }
        else{
                camX-=dx; camY-=dy;
        }

        lastX=xpos; lastY=ypos;
    }
}

void drawBoundary(std::vector<struct line>& shape){
    glColor3f(1.0f,1.0f,1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(auto&p:shape) {
        glVertex2f(p.st[0],p.st[1]);
        glVertex2f(p.en[0],p.en[1]);
    }
    glEnd();
}

void drawGrid(bool* filtered_laplacian){
        glBegin(GL_QUADS);
        for(int j=0;j<N;j++){
                for(int i=0;i<N;i++){
                        if(filtered_laplacian[i+(N-1)*j]){
                                glColor3f(1.0f,0.0f,0.0f);
                        }
                        else{
                                glColor3f(0.0f,1.0f,0.0f);
                        }
                        glVertex2f(i*delta-0.5*box_size,j*delta-0.5*box_size);
                        glVertex2f((i+1)*delta-0.5*box_size,j*delta-0.5*box_size);
                        glVertex2f((i+1)*delta-0.5*box_size,(j+1)*delta-0.5*box_size);
                        glVertex2f(i*delta-0.5*box_size,(j+1)*delta-0.5*box_size);
                }
        }
        glEnd();

}

void drawSlider(GLFWwindow* window){
        int w,h; glfwGetFramebufferSize(window,&w,&h);
        var_t sliderWidth=0.1*box_size;

        glBegin(GL_QUADS);
        glColor3f(1.0f,1.0f,1.0f);
        
        glVertex2f(camX-0.95*0.5*box_size/zoom,camY-0.95*0.5*box_size/zoom);
        glVertex2f(camX+0.95*0.5*box_size/zoom,camY-0.95*0.5*box_size/zoom);
        glVertex2f(camX+0.95*0.5*box_size/zoom,camY-0.90*0.5*box_size/zoom);
        glVertex2f(camX-0.95*0.5*box_size/zoom,camY-0.90*0.5*box_size/zoom);
     
        glColor3f(0.23f,0.23f,0.23f);

        glVertex2f(camX+(sliderX-0.5*sliderWidth)/zoom,camY-0.95*0.5*box_size/zoom);
        glVertex2f(camX+(sliderX+0.5*sliderWidth)/zoom,camY-0.95*0.5*box_size/zoom); 
        glVertex2f(camX+(sliderX+(0.5*sliderWidth))/zoom,camY-0.90*0.5*box_size/zoom);
        glVertex2f(camX+(sliderX-0.5*sliderWidth)/zoom,camY-0.90*0.5*box_size/zoom);

        glEnd();

}


int main(int argc, char**argv){
        if(!glfwInit()) return -1;
        GLFWwindow* win=glfwCreateWindow(800,800,"Laplacian filtering-user input",NULL,NULL);
        if(!win){glfwTerminate();return -1;}
        glfwMakeContextCurrent(win);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        glfwSetScrollCallback(win, scroll_callback);
        glfwSetMouseButtonCallback(win, mouse_button_callback);
        glfwSetCursorPosCallback(win, cursor_pos_callback);

        box_size=(var_t)std::stof(argv[1]);
        maxDepth=std::atoi(argv[2]);
        int n_bodies;
        n_bodies=std::atoi(argv[3]);
        N=std::pow(2,maxDepth)+1; 
        delta=box_size/(N-1);
        bool frozen[(N-1)*(N-1)];
        var_t distances[(N-1)*(N-1)]; //Distance array used for each object
        var_t laplacian[(N-1)*(N-1)];
        bool filtered_laplacian[(N-1)*(N-1)];

        #pragma omp parallel for collapse(2)
        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        distances[i+j*(N-1)]=LARGE;
                        frozen[i+j*(N-1)]=false;
                }
        }

        std::ifstream ibfile(ibfile_name);
        int num_iblines_temp,ib_count;
        num_iblines=0;
        ib_count=0;

        std::vector<struct line>iblines;
        
        for(int k=0;k<n_bodies;k++){
                ibfile>>num_iblines_temp;
                num_iblines=num_iblines+num_iblines_temp;

                iblines.resize(num_iblines);

                for(int i=0;i<num_iblines_temp;i++){
                        ibfile>>iblines[ib_count].st[0]>>iblines[ib_count].st[1]>>iblines[ib_count].en[0]>>
                        iblines[ib_count].en[1]>>iblines[ib_count].normal[0]>>iblines[ib_count].normal[1];
                        iblines[ib_count].body_ID=k;
                        ib_count=ib_count+1;
                }

        }
       
        //For the immersed bodies

        Quadtree2 qt(Box2(0.0f,0.0f,box_size),0);
        qt.build(iblines,frozen,distances);

        //For the walls

        //Upper wall
        #pragma omp parallel for collapse(2)
        for(int j=N-2;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        //Lower wall
        #pragma omp parallel for collapse(2)
        for(int j=0;j<1;j++){
                for(int i=0;i<N-1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        //Left wall
        #pragma omp parallel for collapse(2)
        for(int j=0;j<N-1;j++){
                for(int i=0;i<1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        //Right wall
        #pragma omp parallel for collapse(2)
        for(int j=0;j<N-1;j++){
                for(int i=N-2;i<N-1;i++){
                        distances[i+j*(N-1)]=0.5*delta;
                        frozen[i+j*(N-1)]=true;
                }
        }

        sweep_controller(distances,frozen);

        compute_laplacian(distances, laplacian);
        var_t sliderWidth=0.1*box_size;
        var_t power_min=-8.0;
        var_t power_max=16.0;
        var_t slider_min=0.5*sliderWidth-0.95*0.5*box_size;
        var_t slider_max=-0.5*sliderWidth+0.95*0.5*box_size;
        var_t m=(power_max-power_min)/(slider_max-slider_min);
        var_t c=(slider_max*power_min-slider_min*power_max)/(slider_max-slider_min);

        while(!glfwWindowShouldClose(win)){
        laplacian_limit=-pow(10.0,m*sliderX+c);
        if(laplacian_limit!=laplacian_limit_old){
                filter_medial_axis(laplacian, filtered_laplacian,laplacian_limit); //Get thick medial axis
                laplacian_limit_old=laplacian_limit;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        glOrtho(-0.5*box_size/zoom+camX,0.5*box_size/zoom+camX,
                -0.5*box_size/zoom+camY,0.5*box_size/zoom+camY,-1,1);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();

        drawGrid(filtered_laplacian);
        drawSlider(win);
        drawBoundary(iblines);
        glfwSwapBuffers(win);
        glfwPollEvents();
        if(glfwGetKey(win,GLFW_KEY_ESCAPE)==GLFW_PRESS)
            glfwSetWindowShouldClose(win,true);
        };

        //Thin medial axis
        bool is_even=true;
        for(int i=0; i<100;i++){
                thin_medial_axis(filtered_laplacian,is_even);
                is_even=!is_even;
        }

        win=glfwCreateWindow(800,800,"Thinned and filtered axis",NULL,NULL);
        if(!win){glfwTerminate();return -1;}
        glfwMakeContextCurrent(win);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        glfwSetScrollCallback(win, scroll_callback);
        glfwSetMouseButtonCallback(win, mouse_button_callback);
        glfwSetCursorPosCallback(win, cursor_pos_callback);

        while(!glfwWindowShouldClose(win)){
        glClear(GL_COLOR_BUFFER_BIT);
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        var_t aspect=var_t(w)/h;
        glOrtho(-0.5*box_size/zoom+camX,0.5*box_size/zoom+camX,
                -0.5*box_size/zoom+camY,0.5*box_size/zoom+camY,-1,1);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();

        drawGrid(filtered_laplacian);
        drawBoundary(iblines);
        glfwSwapBuffers(win);
        glfwPollEvents();
        if(glfwGetKey(win,GLFW_KEY_ESCAPE)==GLFW_PRESS)
            glfwSetWindowShouldClose(win,true);

        };

        //Write out tree partitions,grid,solution and then free heap-allocated memory
        qt.write(qt);
        write_grid();
        write_soln(distances,filtered_laplacian,frozen);
        qt.delete_tree(qt);
        glfwTerminate();
        return 0;
}

