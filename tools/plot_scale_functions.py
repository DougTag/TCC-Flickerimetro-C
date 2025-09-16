import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['text.usetex'] = True

def k_1(q, delta):
    return delta/(2*np.pi) * np.asin(2*q-1)

def k_1_inv(q, delta):
    return (np.sin(2*np.pi/delta * q)+1)/2

delta = 10
q = np.linspace(0, 1, 1000)

q_limit = k_1_inv(k_1(q, delta)+1, delta)

difference = q_limit-q

plt.plot(q, q_limit)
plt.plot(q, difference)
plt.grid()

# 3. Add labels and title (good practice!)
plt.xlabel('$q$')
plt.ylabel('$q_{limit}$')

# 4. Display the plot
plt.show()