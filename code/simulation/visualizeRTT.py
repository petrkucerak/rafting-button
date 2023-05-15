import numpy as np
import matplotlib.pyplot as plt

filename = "output.txt"
data = np.loadtxt(filename, dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('RTT synchronization in DS')
ax.set_ylabel('Size of D~\n(ms)')
ax.set_xlabel('Time\n(ms)')
# ax.set_yscale('log')

plt.plot(data[:, 0]/1000, data[:, 1]/1000, label="~RTT [B]")
plt.plot(data[:, 0]/1000, data[:, 4]/1000, label="~RTT [C]")
plt.plot(data[:, 0]/1000, data[:, 7]/1000, label="~RTT [D]")

# plt.plot(data[:, 0]/10, data[:, 3]/10, label="RTT l [B]")
# plt.plot(data[:, 0]/10, data[:, 9]/10, label="RTT l [C]")
# plt.plot(data[:, 0]/10, data[:, 15]/10, label="RTT l [D]")



# plt.axhline(y=np.mean(data[:,3]/10), color='green', label="latency [B] mean")
# plt.axhline(y=np.mean(data[:,6]/10), color='brown', label="latency [C] mean")



plt.legend()
plt.grid(True)

plt.show()