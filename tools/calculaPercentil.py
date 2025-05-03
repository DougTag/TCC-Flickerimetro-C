import numpy as np
import matplotlib.pyplot as plt

## Parâmetros da onda senoidal
frequencia = 5  # Hz
amplitude = 1    # Amplitude da onda
fase = 0         # Fase inicial
amostragem = 10000  # Número de pontos

tempo = np.linspace(0, 1, amostragem)  # Vetor de tempo de 0 a 1 segundo
senoide = amplitude * np.sin(2 * np.pi * frequencia * tempo + fase)


## Cálculo dos percentis
percentis = [0.7, 1, 1.5, 2.2, 3, 4, 6, 8, 10, 13, 17, 30, 50, 80]
valores_percentis = np.percentile(senoide, percentis)


## Exibir resultados
print("Percentis calculados:")
for p, v in zip(percentis, valores_percentis):
    print(f"{p}%: {v:.4f}")


## Plotando a onda senoidal
plt.plot(tempo, senoide, label="Onda Senoidal")
plt.xlabel("Tempo (s)")
plt.ylabel("Amplitude")
plt.title("Onda Senoidal e seus Percentis")
plt.grid()


## Marcando os percentis
for p, v in zip(percentis, valores_percentis):
    plt.axhline(y=v, linestyle="--", label=f"{p}%: {v:.4f}")

plt.legend()
plt.show()
