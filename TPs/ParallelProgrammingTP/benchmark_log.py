import os
import re
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

PROJECT_PATH = "/gext/mahammed.el_sharkawy/parallel-programming-project/TPs/ParallelProgrammingTP"
LOG_FILE = os.path.join(PROJECT_PATH, "Bench.log")
OUTPUT_DIR = os.path.join(PROJECT_PATH, "results")

os.makedirs(OUTPUT_DIR, exist_ok=True)

line_regex = re.compile(
    r"SPMV\s+np:\s*(\d+)\s+nx:\s*(\d+)\s+Time SPMV:\s*(\d+\.\d+(?:e[-+]?\d+)?)\s+Time MPI:\s*(\d+\.\d+(?:e[-+]?\d+)?)\s+Acceleration:\s*(\d+\.\d+)"
)


results = []

with open(LOG_FILE, "r") as f:
    for line in f:
        match = line_regex.search(line)
        if match:
            np_val = int(match.group(1))
            nx_val = int(match.group(2))
            seq_time = float(match.group(3))  # Time SPMV
            par_time = float(match.group(4))  # Time MPI
            speedup = float(match.group(5))   # Acceleration

            results.append({
                "Matrix Size (nx)": nx_val,
                "Cores (np)": np_val,
                "Sequential Time": seq_time,
                "Parallel Time": par_time,
                "Speedup": speedup
            })

df = pd.DataFrame(results)
df = df.sort_values(by=["Matrix Size (nx)", "Cores (np)"])

output_csv = os.path.join(OUTPUT_DIR, "benchmark_results.csv")
df.to_csv(output_csv, index=False)
print(f"Results saved to {output_csv}")

execution_times = df.pivot(index="Cores (np)", columns="Matrix Size (nx)", values="Parallel Time")
speedup_times = df.pivot(index="Cores (np)", columns="Matrix Size (nx)", values="Speedup")

# Plot Execution Time (Parallel Time)
plt.figure(figsize=(10, 6))
for nx in execution_times.columns:
    plt.plot(execution_times.index, execution_times[nx], marker='o', label=f"nx={nx}")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Number of Cores (log scale)")
plt.ylabel("Parallel Execution Time (s, log scale)")
plt.title("Parallel Execution Time vs Number of Cores")
plt.legend()
plt.grid()
execution_time_plot = os.path.join(OUTPUT_DIR, "execution_time.png")
plt.savefig(execution_time_plot)
plt.close()
print(f"Execution time plot saved to {execution_time_plot}")

# Plot Speedup
plt.figure(figsize=(10, 6))
for nx in speedup_times.columns:
    plt.plot(speedup_times.index, speedup_times[nx], marker='o', label=f"nx={nx}")
plt.xlabel("Number of Cores")
plt.ylabel("Speedup")
plt.title("Speedup vs Number of Cores")
plt.legend()
plt.grid()
speedup_plot = os.path.join(OUTPUT_DIR, "speedup.png")
plt.savefig(speedup_plot)
plt.close()
print(f"Speedup plot saved to {speedup_plot}")

print("Benchmark parsing and plotting completed.")

