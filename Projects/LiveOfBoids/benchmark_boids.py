import pandas as pd
import matplotlib.pyplot as plt
import os

# Load the data from CSV
df = pd.read_csv("boids_benchmark.csv")
output_dir = "benchmark_results"
os.makedirs(output_dir, exist_ok=True)

# Techniques to evaluate
modes = df['Mode'].unique()
baseline = df[df['Mode'] == "seq"]
df = df.merge(
    baseline[['NumBoids', 'nbThreads', 'Time']].rename(columns={'Time': 'seq_baseline'}),
    on=['NumBoids', 'nbThreads'], 
    how='left'
    )

# Derived metrics: Calculate speedup and efficiency for each mode
for mode in modes:
    df[f'{mode}_speedup'] = df['seq_baseline'] / df[f'Time']
    df[f'{mode}_efficiency'] = df[f'{mode}_speedup'] / df['nbThreads']

# Group the data by NumBoids
grouped = df.groupby('NumBoids')

# Plot performance metrics for each NumBoids
for num_boids, group in grouped:
    plt.figure(figsize=(12, 6))
    
    # Plot execution time
    for mode in modes:
        mode_data = group[group['Mode'] == mode]
        plt.plot(
            mode_data['nbThreads'],
            mode_data['Time'],
            marker='o',
            label=f'{mode} Execution Time'
        )

    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time (s)')
    plt.title(f'Execution Time for NumBoids = {num_boids}')
    plt.legend()
    plt.grid()
    plt.savefig(os.path.join(output_dir, f'boids_execution_time_{num_boids}.png'))
    plt.show()

    # Plot speedup
    plt.figure(figsize=(12, 6))
    for mode in modes:
        mode_data = group[group['Mode'] == mode]
        plt.plot(
            mode_data['nbThreads'],
            mode_data[f'{mode}_speedup'],
            marker='o',
            label=f'{mode} Speedup'
        )

    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup')
    plt.title(f'Speedup for NumBoids = {num_boids}')
    plt.legend()
    plt.grid()
    plt.savefig(os.path.join(output_dir, f'boids_speedup_{num_boids}.png'))
    plt.show()

    # Plot efficiency
    plt.figure(figsize=(12, 6))
    for mode in modes:
        mode_data = group[group['Mode'] == mode]
        plt.plot(
            mode_data['nbThreads'],
            mode_data[f'{mode}_efficiency'],
            marker='o',
            label=f'{mode} Efficiency'
        )

    plt.xlabel('Number of Threads')
    plt.ylabel('Efficiency')
    plt.title(f'Efficiency for NumBoids = {num_boids}')
    plt.legend()
    plt.grid()
    plt.savefig(os.path.join(output_dir, f'boids_efficiency_{num_boids}.png'))
    plt.show()
