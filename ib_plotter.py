import numpy as np
import scipy as sc
from matplotlib import pyplot as plt

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
inline=ibfile.readline().strip().split()
num_iblines=int(inline[0])

ib_x=np.zeros((num_iblines+1,1))
ib_y=np.zeros((num_iblines+1,1))
nx=np.zeros((num_iblines+1,1))
ny=np.zeros((num_iblines+1,1))

for i in range(0,num_iblines):
        inline=ibfile.readline().strip().split()
        ib_x[i,0]   = float(inline[0])
        ib_y[i,0]   = float(inline[1])
        ib_x[i+1,0] = float(inline[2])
        ib_y[i+1,0] = float(inline[3])
        nx[i,0] = float(inline[4])
        ny[i,0] = float(inline[5])
        plt.quiver(0.5*(ib_x[i,0]+ib_x[i+1,0]),0.5*(ib_y[i,0]+ib_y[i+1,0]),nx[i,0],ny[i,0])

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

plt.figure(0);
plt.plot(np.transpose(X),np.transpose(Y),'k')
plt.contourf(X_contour,Y_contour,distances)
plt.plot(ib_x[:,0],ib_y[:,0],'r')
plt.plot(X,Y,'k')
plt.colorbar()
plt.title("Distance field")
plt.show()

plt.figure(1)
plt.plot(X,Y,'k')
plt.plot(np.transpose(X),np.transpose(Y),'k')
plt.plot(ib_x[:,0],ib_y[:,0],'r')
plt.pcolor(X_contour,Y_contour,laplacian)
plt.colorbar()
plt.title("Laplacian")
plt.show()

plt.figure(2)
plt.plot(X,Y,'k')
plt.plot(np.transpose(X),np.transpose(Y),'k')
plt.plot(ib_x[:,0],ib_y[:,0],'r')
plt.pcolor(X_contour,Y_contour,frozen)
plt.colorbar()
plt.title("Frozen cells")
plt.show()
