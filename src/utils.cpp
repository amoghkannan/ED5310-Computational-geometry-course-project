#include"utils.h"

var_t dot_product(var_t vec1[],var_t vec2[]){
        var_t ans;
        ans=vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
        return ans;
}

void cross_product(var_t vec1[],var_t vec2[],var_t ans[]){
        ans[0]=vec1[1]*vec2[2]-vec1[2]*vec2[1];
        ans[1]=vec1[2]*vec1[0]-vec1[0]*vec2[2];
        ans[2]=vec1[0]*vec2[1]-vec1[1]*vec2[0];
        return;
}

var_t norm(var_t vec[]){
        var_t ans;
        ans=dot_product(vec,vec);
        ans=sqrt(ans);
        return ans;
}

var_t pointToSegmentDistance(var_t px,var_t py,struct line& l){
    var_t x1,y1,x2,y2;
    x1=l.st[0];
    y1=l.st[1];
    x2=l.en[0];
    y2=l.en[1];
    var_t dx=x2-x1, dy=y2-y1;
    if(dx==0 && dy==0) return sqrt((px-x1)*(px-x1)+(py-y1)*(py-y1));
    var_t t = ((px-x1)*dx + (py-y1)*dy)/(dx*dx+dy*dy);
    t = std::max((var_t)0.0, std::min((var_t)1.0, t));
    var_t projx = x1 + t*dx;
    var_t projy = y1 + t*dy;
    return sqrt((px-projx)*(px-projx)+(py-projy)*(py-projy));
}

std::pair<bool,var_t> boxIntersectsPolygonDist(const Box2& b,std::vector<struct line>& poly,var_t eps){
    var_t r = b.size/2.0 + eps;
    std::pair<bool,var_t> ans;
    ans.first=false;
    var_t dummy=LARGE;
    int num_iblines=poly.size();
  
    for(int i=0;i<num_iblines;i++){
        dummy=std::min(dummy,pointToSegmentDistance(b.x,b.y,poly[i])); 
    }

    ans.second=dummy;

    if(ans.second<=r) ans.first=true;

    return ans;
}

void Quadtree2:: build(std::vector<struct line>& poly,int frozen[], var_t distances[], int body_ID[], var_t errf){
        std::pair<bool,var_t>ans=boxIntersectsPolygonDist(box, poly,errf);
        if(!ans.first) return; // only subdivide near boundary
        if(depth==maxDepth){ //Cell intersects polygon, and is at required resolution; fill exact distance and freeze
                int cell_ID1,cell_ID2;
                cell_ID1=((box.x-0.5*box.size)+0.5*box_size)/delta;
                cell_ID2=((box.y-0.5*box.size)+0.5*box_size)/delta;
                frozen[cell_ID1+cell_ID2*(N-1)]=1;
                distances[cell_ID1+cell_ID2*(N-1)]=ans.second;
                body_ID[cell_ID1+cell_ID2*(N-1)]=poly[0].body_ID;
                return;
        }

        isLeaf=false;
        var_t hs=box.size/2.0;
        var_t q=hs/2.0;
        Box2 sub[4]={
            {box.x-q,box.y-q,hs},{box.x+q,box.y-q,hs},
            {box.x-q,box.y+q,hs},{box.x+q,box.y+q,hs}
        };
        
        for(int i=0;i<4;i++){
            Quadtree2* c=new Quadtree2(sub[i],depth+1);
            c->build(poly,frozen,distances,body_ID,0.5*errf);
            children.push_back(c);
        }
        return;
}

void Quadtree2::write(Quadtree2& tree){

        std::fstream outfile("tree.dat",std::fstream::out | std::fstream::app);
        outfile<<tree.box.x<<" "<<tree.box.y<<" "<<tree.box.size<<std::endl;
        
        outfile.close();

        if(tree.isLeaf==true) return;
        
        for (int i=0;i<4;i++){
                tree.write(*(tree.children[i]));
        }
}

void Quadtree2::delete_tree(Quadtree2& tree){
       
       if((tree.isLeaf)==true){
                return;
       }

       for(int i=0;i<4;i++){
                tree.delete_tree(*(tree.children[i]));
                delete (tree.children[i]);
                tree.children[i]=NULL;
       }
       return;
}

void write_grid(){

        std::ofstream gridfile("grid.dat");
        gridfile<<N<<" "<<N<<std::endl;

        for(int j=0;j<N;j++){
                for(int i=0;i<N;i++){
                        gridfile<<i*delta-0.5*box_size<<" "<<j*delta-0.5*box_size<<std::endl;
                }
        }

        gridfile.close();
}

void write_soln(var_t distances[], var_t laplacian[], int filtered_laplacian[], int frozen[], int body_ID[]){

        std::ofstream solnfile("soln.dat");
        solnfile<<N<<" "<<N<<std::endl;

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        solnfile<<distances[i+j*(N-1)]<<std::endl;
                }
        }

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        solnfile<<laplacian[i+j*(N-1)]<<std::endl;
                }
        }

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        solnfile<<filtered_laplacian[i+j*(N-1)]<<std::endl;
                }
        }

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        solnfile<<frozen[i+j*(N-1)]<<std::endl;
                }
        }

        for(int j=0;j<N-1;j++){
                for(int i=0;i<N-1;i++){
                        solnfile<<body_ID[i+j*(N-1)]<<std::endl;
                }
        }

        solnfile.close();

}
