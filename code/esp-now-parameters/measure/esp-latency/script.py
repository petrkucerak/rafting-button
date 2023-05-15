import numpy as np
import matplotlib.pyplot as plt

filename1 = "output-COM6A.txt"
filename2 = "output-COM7A.txt"
data1 = np.loadtxt(filename1, dtype=float, delimiter=",")
data2 = np.loadtxt(filename2, dtype=float, delimiter=",")

print("A | mean: ", np.mean(data1), " | max: ",
      np.max(data1), " | min: ", np.min(data1))
print("B | mean: ", np.mean(data2), " | max: ",
      np.max(data2), " | min: ", np.min(data2))

plt.figure(figsize=(12, 8))
plt.hist([data1, data2], bins=np.arange(np.min(data1), 1200), align="left")
plt.xlabel("Latency µs")
plt.ylabel("Count of message")
plt.title("Latency of ESP-NOW protocol\nin unicast mode")
plt.legend(["COM6", "COM7"])
plt.show()
