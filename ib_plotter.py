import numpy as np
import scipy as sc
from matplotlib import pyplot as plt
import sys


num_bodies=int(sys.argv[1])
gridfile=open("grid.dat","r")
inline=gridfile.readline().strip().split()
N_x=int(inline[0])
N_y=int(inline[1])
X=np.zeros((N_x,N_y))
Y=np.zeros((N_x,N_y))

for j in range(0,N_y):
    for i in range(0,N_x):
        inline=gridfile.readline().strip().split()
        X[i,j]=float(inline[0])
        Y[i,j]=float(inline[1])

delta=X[1,0]-X[0,0]

ibfile=open("ib.dat","r")

ib_x_fin=[]
ib_y_fin=[]
nx_fin=[]
ny_fin=[]

plt.figure(0)
for k in range(0,num_bodies):
        inline=ibfile.readline().strip().split()
        num_iblines=int(inline[0])
        ib_x=[]
        ib_y=[]
        nx=[]
        ny=[]
        for i in range(0,num_iblines):
                inline=ibfile.readline().strip().split()
                ib_x.append(float(inline[0]))
                ib_y.append(float(inline[1]))
                ib_x.append(float(inline[2]))
                ib_y.append(float(inline[3]))
                nx.append(float(inline[4]))
                ny.append(float(inline[5]))
                plt.quiver(0.5*(ib_x[2*i]+ib_x[2*i+1]),0.5*(ib_y[2*i]+ib_y[2*i+1]),nx[i],ny[i])
        ib_x_fin.append(ib_x)
        ib_y_fin.append(ib_y)
        nx_fin.append(nx)
        ny_fin.append(ny)

plt.gca().set_aspect('equal')
plt.grid()
plt.show()

solnfile=open("soln.dat","r")
inline=solnfile.readline().strip().split()
N=int(inline[0])

distances=np.zeros((N-1,N-1))
laplacian=np.zeros((N-1,N-1))
filtered_laplacian=np.zeros((N-1,N-1))
frozen=np.zeros((N-1,N-1))
body_ID=np.ones((N-1,N-1))*(-1)
tree=np.zeros((N-1,N-1))

for j in range(0,N-1):
    for i in range(0,N-1):
         inline=solnfile.readline().strip().split()
         distances[i,j]=float(inline[0])

for j in range(0,N-1):
    for i in range(0,N-1):
         inline=solnfile.readline().strip().split()
         laplacian[i,j]=float(inline[0])

for j in range(0,N-1):
    for i in range(0,N-1):
         inline=solnfile.readline().strip().split()
         filtered_laplacian[i,j]=float(inline[0])

for j in range(0,N-1):
    for i in range(0,N-1):
         inline=solnfile.readline().strip().split()
         frozen[i,j]=float(inline[0])

for j in range(0,N-1):
    for i in range(0,N-1):
         inline=solnfile.readline().strip().split()
         body_ID[i,j]=int(inline[0])


X_contour=0.25*(X[0:N-1,0:N-1]+X[1:N,0:N-1]+X[0:N-1,1:N]+X[1:N,1:N])
Y_contour=0.25*(Y[0:N-1,0:N-1]+Y[1:N,0:N-1]+Y[0:N-1,1:N]+Y[1:N,1:N])

plt.figure(1)
#for k in range(0,num_bodies):
#        plt.plot(ib_x_fin[k],ib_y_fin[k],'k',linewidth=2)
treefile=open("tree.dat","r")
colors=['r','g','b','k','y']

while True:
        inline=treefile.readline().strip().split()
        if(len(inline)==0):
                break
        elif(len(inline)==3):
                node_i=int(inline[0])
                node_j=int(inline[1])
                nodetype=inline[2]
                if(nodetype=='N'):
                        plt.scatter(X[node_i,node_j],Y[node_i,node_j],s=20.0,c='r',zorder=2)
                elif(nodetype=='C'):
                        plt.scatter(X[node_i,node_j],Y[node_i,node_j],s=20.0,c='g',zorder=2)
                elif(nodetype=='D'):
                        plt.scatter(X[node_i,node_j],Y[node_i,node_j],s=20.0,c='b',zorder=2)
                else:
                        plt.scatter(X[node_i,node_j],Y[node_i,node_j],s=20.0,c='y',zorder=2)

        elif(len(inline)==6):
                val=int(inline[1])
                vec_x=float(inline[2])
                vec_y=float(inline[3])
                vec_mag=np.sqrt(vec_x**2+vec_y**2)
                vec_x=vec_x/vec_mag
                vec_y=vec_y/vec_mag
                vec_cord1=int(inline[4])
                vec_cord2=int(inline[5])
                #plt.quiver(X[vec_cord1,vec_cord2],Y[vec_cord1,vec_cord2],vec_x,vec_y,zorder=0,alpha=0.2,color=colors[val])
        else:
                i1=int(inline[0])
                i2=int(inline[1])
                j1=int(inline[2])
                j2=int(inline[3])
                angle=float(inline[4])
                plt.plot([X[i1,j1],X[i2,j2]],[Y[i1,j1],Y[i2,j2]],'k',linewidth=2,zorder=1)
                #plt.quiver(0.5*(X[i1,j1]+X[i2,j2]),0.5*(Y[i1,j1]+Y[i2,j2]),0.01*np.cos(angle),0.01*np.sin(angle),zorder=0)

#plt.pcolor(X_contour,Y_contour,filtered_laplacian,zorder=0)      
#plt.colorbar()
plt.grid()
plt.title("Distance field")
plt.gca().set_aspect('equal')
plt.show()

plt.figure(2)
for k in range(0,num_bodies):
        plt.plot(ib_x_fin[k],ib_y_fin[k],'k',linewidth=2)
plt.pcolor(X_contour,Y_contour,distances)
plt.colorbar()
plt.grid()
plt.title("Laplacian")
plt.show()

plt.figure(3)
for k in range(0,num_bodies):
        plt.plot(ib_x_fin[k],ib_y_fin[k],'k')
plt.pcolor(X_contour,Y_contour,frozen)
plt.colorbar()
plt.grid()
plt.title("Frozen cells")
plt.show()
