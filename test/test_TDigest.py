import numpy as np
import matplotlib.pyplot as plt
import random

def create_sample_data_txt():
    # Frequência de amostragem e tempo
    Fs = 10000
    t = np.arange(0, 60, 1/Fs)

    ## Parâmetros da onda de verificação (IEC TR 61547-1 - A.3)
    f_r = 100
    d_r = 22/100

    f_m = 0.3250; d_E = 2.5386/100
    f_m = 0.9167; d_E = 2.0473/100
    f_m = 8.8;    d_E = 0.6832/100
    f_m = 13.5;   d_E = 0.7780/100
    f_m = 33.3;   d_E = 2.0027/100

    E = (1 - d_r/2 * np.cos(2*np.pi*f_r*t)) * (1 + d_E/2 * np.sign(np.sin(2*np.pi*f_m*t)))

def main():
    # 1. GERAÇÃO DOS DADOS
    size = 10000

    normal_data = np.random.normal(50, 10, size)
    uniform_data = np.random.uniform(0, 100, size)
    exponential_data = np.random.exponential(1, size)

    with open("tests/normal_data.txt", "w") as file:
        for value in normal_data:
            file.write(str(value) + '\n')

    with open("tests/uniform_data.txt", "w") as file:
        for value in uniform_data:
            file.write(str(value) + '\n')

    with open("tests/exponential_data.txt", "w") as file:
        for value in exponential_data:
            file.write(str(value) + '\n')

    fig, axes = plt.subplots(1, 3)

    axes[0].hist(normal_data, bins=100)
    axes[0].set_title("Normal distribution")
    axes[1].hist(uniform_data, bins=100)
    axes[1].set_title("Uniform distribution")
    axes[2].hist(exponential_data, bins=100)
    axes[2].set_title("Exponential distribution")

    plt.show()

    # 2. ANÁLISE ESTATÍSTICA EXATA
    ## Leitura
    normal_data = []
    with open("tests/normal_data.txt", "r") as file:
        for line in file:
            normal_data.append(float(line))

    uniform_data = []
    with open("tests/uniform_data.txt", "r") as file:
        for line in file:
            uniform_data.append(float(line))

    exponential_data = []
    with open("tests/exponential_data.txt", "r") as file:
        for line in file:
            exponential_data.append(float(line))

    ## Calculo dos percentis
    percentiles = [0.1, 0.7, 1, 1.5, 2.2, 3, 4, 6, 8, 10, 13, 17, 30, 50, 80]
    normal_results = np.percentile(normal_data, percentiles)
    uniform_results = np.percentile(uniform_data, percentiles)
    exponential_results = np.percentile(exponential_data, percentiles)

    print('Normal distribution percentiles:')
    for index, value in enumerate(normal_results):
        print(f"{percentiles[index]};{str(value)}")
    print('\n')

    print('Uniform distribution percentiles:')
    for index, value in enumerate(uniform_results):
        print(f"{percentiles[index]};{str(value)}")
    print('\n')

    print('Exponential distribution percentiles:')
    for index, value in enumerate(exponential_results):
        print(f"{percentiles[index]};{str(value)}")
    print('\n')


if __name__ == "__main__":
    main()