import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('Time synchronization in DS')
ax.set_ylabel('Node time\n(ticks)')
ax.set_xlabel('DS time\n(ns)')

plt.plot(data[:,0], label="SLAVE [B]")
plt.plot(data[:,1], label="SLAVE [C]")

plt.legend()
plt.grid(True)

plt.show()