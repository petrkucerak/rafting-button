import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('RTT synchronization in DS')
ax.set_ylabel('Difference between MASTER and SLAVE\n(ms)')
ax.set_xlabel('Time\n(ms)')
# ax.set_yscale('log')

plt.plot(data[:, 0]/10, data[:, 1]/10, label="~RTT [B]")
plt.plot(data[:, 0]/10, data[:, 2]/10, label="~RTT [C]")
plt.plot(data[:, 0]/10, data[:, 3]/10, label="~RTT [D]")

# plt.plot(data[:, 0]/10, data[:, 3]/10, label="RTT l [B]")
# plt.plot(data[:, 0]/10, data[:, 9]/10, label="RTT l [C]")
# plt.plot(data[:, 0]/10, data[:, 15]/10, label="RTT l [D]")



# plt.axhline(y=np.mean(data[:,3]/10), color='green', label="latency [B] mean")
# plt.axhline(y=np.mean(data[:,6]/10), color='brown', label="latency [C] mean")



plt.legend()
plt.grid(True)

plt.show()