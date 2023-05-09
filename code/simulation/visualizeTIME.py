import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('Time synchronization in DS')
ax.set_ylabel('Difference between MASTER and SLAVE\n(ms)')
ax.set_xlabel('Time\n(ms)')
# ax.set_yscale('log')

plt.plot(data[:, 0]/1000, data[:, 3]/1000, label="SLAVE [B]")
plt.plot(data[:, 0]/1000, data[:, 6]/1000, label="SLAVE [C]")
plt.plot(data[:, 0]/1000, data[:, 9]/1000, label="SLAVE [D]")

plt.axhline(y=np.mean(data[:,3]/1000), color='green', label="SLAVE [B] mean")
plt.axhline(y=np.mean(data[:,6]/1000), color='brown', label="SLAVE [C] mean")
plt.axhline(y=np.mean(data[:,9]/1000), color='grey', label="SLAVE [D] mean")

print("B mean",np.mean(data[:,3]/1000))
print("C mean",np.mean(data[:,6]/1000))
print("D mean",np.mean(data[:,9]/1000))

plt.legend()
plt.grid(True)

plt.show()