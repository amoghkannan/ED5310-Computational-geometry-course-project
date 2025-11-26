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
frozen=np.zeros((N-1,N-1))

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
         frozen[i,j]=float(inline[0])

X_contour=0.25*(X[0:N-1,0:N-1]+X[1:N,0:N-1]+X[0:N-1,1:N]+X[1:N,1:N])
Y_contour=0.25*(Y[0:N-1,0:N-1]+Y[1:N,0:N-1]+Y[0:N-1,1:N]+Y[1:N,1:N])

plt.figure(1)
plt.contourf(X_contour,Y_contour,distances)
for k in range(0,num_bodies):
        plt.plot(ib_x_fin[k],ib_y_fin[k],'k',linewidth=2)
plt.colorbar()
plt.grid()
plt.title("Distance field")
plt.show()

plt.figure(2)
for k in range(0,num_bodies):
        plt.plot(ib_x_fin[k],ib_y_fin[k],'k',linewidth=2)
plt.pcolor(X_contour,Y_contour,laplacian)
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
