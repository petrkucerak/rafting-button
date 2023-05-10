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

plt.axhline(y=np.mean(data[:,2]/1000), color='violet', label="O~ [B] mean")
plt.axhline(y=np.mean(data[:,5]/1000), color='brown', label="O~ [C] mean")
plt.axhline(y=np.mean(data[:,8]/1000), color='red', label="O~ [D] mean")

plt.axhline(y=0.25, color='grey', label="limit for using O~")
plt.axhline(y=-0.25, color='grey', label="limit for using O~")


plt.legend()
plt.grid(True)

plt.show()