import numpy as np

filename = "output-P-S.txt"
data = np.loadtxt(filename, dtype=float, delimiter=",")

mean_diff = np.mean(np.abs(1000000 - data))
diff_max = np.max(np.abs(1000000 - data))
diff_min = np.min(np.abs(1000000 - data))

print("Mean diff", mean_diff)
print("Max", diff_max)
print("Min", diff_min)
