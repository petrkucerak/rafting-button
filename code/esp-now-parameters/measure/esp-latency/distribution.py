import numpy as np
import matplotlib.pyplot as plt

filenames = ["output-COM6A.txt", "output-COM6B.txt", "output-COM7A.txt", "output-COM7B.txt"]

max_main_interval = 1050
major = 0
minor = 0

for filename in filenames:
   data = np.loadtxt(filename, dtype=float)

   for e in data:
      if e <= 1050:
         major += 1
      else:
         minor += 1

print("major: ", (major*100)/(major+minor), " %")
print("minor: ", (minor*100)/(major+minor), " %")