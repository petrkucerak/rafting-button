import numpy as np
import matplotlib.pyplot as plt

folder = "RTT_100_K_0/"
# files = ["log_COM4.txt", "log_COM6.txt",
#          "log_COM7.txt", "log_COM8.txt", "log_COM9.txt"]
files = ["log_COM4.txt", "log_COM6.txt",
         "log_COM7.txt"]
filenames = [folder+files[0], folder+files[1],
             folder+files[2]]
# filenames = [folder+files[0], folder+files[1],
#              folder+files[2], folder+files[3], folder+files[4]]

data0 = np.loadtxt(filenames[0], dtype=float, delimiter=',')
data1 = np.loadtxt(filenames[1], dtype=float, delimiter=',')
data2 = np.loadtxt(filenames[2], dtype=float, delimiter=',')
# data3 = np.loadtxt(filenames[3], dtype=float, delimiter=',')
# data4 = np.loadtxt(filenames[4], dtype=float, delimiter=',')

fig, ax = plt.subplots(figsize=(12, 8))

ax.set_title('Time synchronization in DS')
ax.set_ylabel('Difference between MASTER and SLAVE\n(ms)')
ax.set_xlabel('Time\n(s)')
# ax.set_yscale('log')

# plt.plot(data0[:, 0]/1000, label="~RTT [COM4]")
plt.plot((data0[:, 2] - data1[:, 2])/1000, label="[COM6]")
plt.plot((data0[:, 2] - data2[:, 2])/1000, label="[COM7]")
# plt.plot((data0[:,2] - data3[:, 2])/1000, label="[COM8]")
# plt.plot((data0[:,2] - data4[:, 2])/1000, label="[CON9]")

# plt.plot(data0[:, 2]/1000, label="~O [COM4]")
# plt.plot(data1[:, 2]/1000, label="~O [COM6]")
# plt.plot(data2[:, 2]/1000, label="~O [COM7]")
# plt.plot(data3[:, 2]/1000, label="~O [COM8]")
# plt.plot(data4[:, 2]/1000, label="~O [CON9]")

# plt.plot(data[:, 0]/10, data[:, 3]/10, label="RTT l [B]")
# plt.plot(data[:, 0]/10, data[:, 9]/10, label="RTT l [C]")
# plt.plot(data[:, 0]/10, data[:, 15]/10, label="RTT l [D]")


plt.legend()
plt.grid(True)

plt.show()
