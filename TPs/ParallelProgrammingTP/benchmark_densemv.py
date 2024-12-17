import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

df = pd.read_csv("results_densemv.csv")
output_dir = "results_densemv"
os.makedirs(output_dir, exist_ok=True)

print(df['densemv'])
# Techniques to evaluate
techniques = [
    'densemv','ompdensemv', 'omptaskdensemv', 'omptiledensemv',
    'tbbdensemv', 'tbbrangedensemv', 'tbbrange2ddensemv'
]

# Step 2: Add derived metrics for each technique
for tech in techniques:
    df[f'{tech}_speedup'] = df['densemv'] / df[tech]
    df[f'{tech}_efficiency'] = df[f'{tech}_speedup'] / df['nb_threads']

# Group the data by 'nx'
grouped = df.groupby('nx')

# Step 3: Generate plots for each nx
for nx, group in grouped:
    plt.figure(figsize=(12, 6))
    
    # Plot execution time
    for tech in techniques:
        plt.plot(group['nb_threads'], group[tech], marker='o', label=tech)

    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time (s)')
    plt.title(f'Execution Time for nx = {nx}')
    plt.legend()
    plt.grid()
    plt.savefig(os.path.join(output_dir,f'densemv_execution_time_nx_{nx}.png'))
    plt.show()

    # Plot speedup
    plt.figure(figsize=(12, 6))
    for tech in techniques:
        plt.plot(group['nb_threads'], group[f'{tech}_speedup'], marker='o', label=f'{tech} Speedup')

    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup')
    plt.title(f'Speedup for nx = {nx}')
    plt.legend()
    plt.grid()
    plt.savefig(os.path.join(output_dir,f'densemv_speedup_nx_{nx}.png'))
    plt.show()

    # Plot efficiency
    plt.figure(figsize=(12, 6))
    for tech in techniques:
        plt.plot(group['nb_threads'], group[f'{tech}_efficiency'], marker='o', label=f'{tech} Efficiency')

    plt.xlabel('Number of Threads')
    plt.ylabel('Efficiency')
    plt.title(f'Efficiency for nx = {nx}')
    plt.legend()
    plt.grid()
    plt.savefig(os.path.join(output_dir,f'densemv_efficiency_nx_{nx}.png'))
    plt.show()