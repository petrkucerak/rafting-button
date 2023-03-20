import matplotlib.pyplot as plt
import numpy as np

y = np.array([24, 81])
labels = ['1. možnost - do ruky', '2. možnost - robustní']

explode = [0.2, 0.0]

def abs_value(val):
    a = np.round(val/100. * y.sum(), 0)
    return a


plt.pie(y, labels=labels, autopct=abs_value, explode=explode)
plt.title("Výsledky formuláře o provedení hlasovacího tlačítka")
plt.show()
