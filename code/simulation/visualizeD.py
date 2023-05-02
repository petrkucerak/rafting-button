import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('Time synchronization in DS')
ax.set_ylabel('Difference between MASTER and SLAVE\n(ms)')
ax.set_xlabel('Time\n(ms)')
# ax.set_yscale('log')

plt.plot(data[:, 0]/10, data[:, 1]/10, label="SLAVE [B]")
plt.plot(data[:, 0]/10, data[:, 2]/10, label="SLAVE [C]")

plt.axhline(y=np.mean(data[:,1]/10), color='green', label="SLAVE [B] mean")
plt.axhline(y=np.mean(data[:,2]/10), color='brown', label="SLAVE [C] mean")

# plt.axhline(y=np.mean(data[:,3]/10), color='pink', label="rnd [A] mean")
# plt.axhline(y=np.mean(data[:,4]/10), color='grey', label="rnd [B] mean")
# plt.axhline(y=np.mean(data[:,5]/10), color='yellow', label="rnd [C] mean")

plt.legend()
plt.grid(True)

plt.show()