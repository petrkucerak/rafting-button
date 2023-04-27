import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

print(np.mean(data[:,1]/10))
print(np.mean(data[:,2]/10))