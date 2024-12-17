import pandas as pd
import matplotlib.pyplot as plt
import os

df = pd.read_csv("results_spmv.csv")
output_dir = "results_spmv"
os.makedirs(output_dir, exist_ok=True)

df['Speedup'] = df['SpMV_time'] / df['MPI_SpMV_time']
df['Efficiency'] = df['Speedup'] / df['np']


plt.figure(figsize=(12, 6))
for nx in df['nx'].unique():
    subset = df[df['nx'] == nx]
    plt.plot(subset['np'], subset['MPI_SpMV_time'], marker='o', label=f'MPI Time (nx={nx})')

plt.xlabel('Number of Processors (np)')
plt.ylabel('Execution Time (s)')
plt.title('Execution Time vs Number of Processors')
plt.legend()
plt.grid()
plt.savefig(os.path.join(output_dir,'execution_time.png'))
plt.show()

# Step 4: Plot speedup
plt.figure(figsize=(12, 6))
for nx in df['nx'].unique():
    subset = df[df['nx'] == nx]
    plt.plot(subset['np'], subset['Speedup'], marker='o', label=f'Speedup (nx={nx})')

plt.xlabel('Number of Processors (np)')
plt.ylabel('Speedup')
plt.title('Speedup vs Number of Processors')
plt.legend()
plt.grid()
plt.savefig(os.path.join(output_dir,'speedup.png'))
plt.show()

# Step 5: Plot efficiency
plt.figure(figsize=(12, 6))
for nx in df['nx'].unique():
    subset = df[df['nx'] == nx]
    plt.plot(subset['np'], subset['Efficiency'], marker='o', label=f'Efficiency (nx={nx})')

plt.xlabel('Number of Processors (np)')
plt.ylabel('Efficiency')
plt.title('Efficiency vs Number of Processors')
plt.legend()
plt.grid()
plt.savefig(os.path.join(output_dir,'efficiency.png'))
plt.show()