import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('Time synchronization in DS')
ax.set_ylabel('Node time\n(ticks)')
ax.set_xlabel('DS time\n(ns)')

plt.plot(data)
plt.show()
