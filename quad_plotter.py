#!/usr/bin/python3

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import patches as patch
import sys

num_bodies=int(sys.argv[1])

ibfile=open("ib.dat","r")

fig, ax = plt.subplots()

for k in range(0,num_bodies):
        inline=ibfile.readline().strip().split()
        num_iblines=int(inline[0])

        x=[]
        y=[]

        for i in range(0,num_iblines):
                inline=ibfile.readline().strip().split()
                x.append(float(inline[0]))
                y.append(float(inline[1]))

        plt.plot(x,y,'r')

treefile=open("tree.dat","r")
data=treefile.readlines()
n=len(data)

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
