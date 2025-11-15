#!/usr/bin/python3

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import patches as patch

ibfile=open("ib.dat","r")
inline=ibfile.readline().strip().split()
num_iblines=int(inline[0])

x=[]
y=[]

for i in range(0,num_iblines):
        inline=ibfile.readline().strip().split()
        x.append(float(inline[0]))
        y.append(float(inline[1]))

treefile=open("tree.dat","r")
data=treefile.readlines()
n=len(data)


fig, ax = plt.subplots()
plt.plot(x,y,'r')
plt.xlabel("x")
plt.ylabel("y")
plt.title("Quadtree")

size_min=1E6

for i in range(0,n):
        line=data[i].strip().split()
        centerx=float(line[0])
        centery=float(line[1])
        size=float(line[2])
        size_min=min(size,size_min);
        rect=patch.Rectangle([centerx-size/2.0,centery-size/2.0],size,size,linewidth=1, edgecolor='g', facecolor='none')
        ax.add_patch(rect)

print(size_min)
plt.show()
