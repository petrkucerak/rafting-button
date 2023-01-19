import matplotlib.pyplot as plt
import numpy as np

categories = ["Range", "Coverage", "Deployment", "Cost Efficiency", "Battery Life", "QoS", "Payload Length", "Latency Performance", "Scalability"]

categories = [*categories, categories[0]]           

sigfox = [11,11,9,11,11,4,3.3,3.8,6.3]
lora = [8,8,11,10.8,10.8,4.2,6,8,7.2]
nbiot = [5,4,3,4,7.5,11,11,11,11]

sigfox = [*sigfox, sigfox[0]]
lora = [*lora, lora[0]]
nbiot = [*nbiot, nbiot[0]]

label_loc = np.linspace(start=0, stop=2 * np.pi, num=len(sigfox))

plt.subplot(polar=True)
plt.plot(label_loc, sigfox, label="Sigfox")
plt.plot(label_loc, lora, label="LoRa")
plt.plot(label_loc, nbiot, label="NB-IoT")
plt.title("Srovnání parametrů LPWAN technologií")
lines, labels = plt.thetagrids(np.degrees(label_loc), labels=categories)
plt.legend()
plt.show()
