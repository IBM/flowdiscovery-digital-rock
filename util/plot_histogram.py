#!/usr/bin/env python3

import sys

import matplotlib.pyplot as plt
import numpy as np

# Read threshold and output folder from stdin
t = float(sys.argv[1])
output_folder = sys.argv[2]

# Reading histogram file from output folder
bins, norm_hist, accu_hist = np.loadtxt(output_folder + "/histogram.dat",
                                        unpack=True)

# Creating graph for the normalised histogram
plt.figure(dpi=125)
plt.bar(bins, norm_hist, width=1, color='red', linewidth=0)
plt.axvline(x=t, linewidth=1.5, color='black', linestyle='--')
plt.axis([0, 256, 0, norm_hist.max()])
plt.text(t / 2, norm_hist.max() / 2, "pore\nspace", fontsize=16, ha='center')
plt.text(t + (256 - t) / 2, norm_hist.max() / 2, "solid\nspace", fontsize=16,
         ha='center')
plt.title("Normalised histogram", fontsize=18)
plt.xlabel("Greyscale level [0-255]", fontsize=16)
plt.ylabel("Normalised frequency", fontsize=16)
plt.savefig(output_folder + "/norm_hist.png", dpi=125, bbox_inches='tight')

# Creating graph for the accumulated histogram
plt.figure(dpi=125)
plt.bar(bins, accu_hist, width=1, color='red', linewidth=0)
plt.axvline(x=t, linewidth=1.5, color='black', linestyle='--')
plt.axis([0, 256, 0, accu_hist.max()])
plt.text(t/2, accu_hist.max()/2, "pore\nspace", fontsize=16, ha='center')
plt.text(t+(256-t)/2, accu_hist.max()/2, "solid\nspace", fontsize=16,
         ha='center')
plt.title("Accumulated histogram", fontsize=18)
plt.xlabel("Greyscale level [0-255]", fontsize=16)
plt.ylabel("Accumulated frequency", fontsize=16)
plt.savefig(output_folder + "/accu_hist.png", dpi=125, bbox_inches='tight')
