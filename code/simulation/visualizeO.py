import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('Time synchronization in DS')
ax.set_ylabel('Size of O~\n(ms)')
ax.set_xlabel('Time\n(ms)')
# ax.set_yscale('log')



plt.plot(data[:, 0]/1000, data[:, 2]/1000, label="O~ [B]")
plt.plot(data[:, 0]/1000, data[:, 5]/1000, label="O~ [C]")
plt.plot(data[:, 0]/1000, data[:, 8]/1000, label="O~ [D]")


plt.legend()
plt.grid(True)

plt.show()