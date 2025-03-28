#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Improved SpMV MPI Performance Analysis Tool
-------------------------------------------
Analyzes SpMV benchmark logs, accurately extracting metrics, and generates clear visualizations.
Date: March 28, 2025
"""

import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

# Configuration parameters
log_directory = "."  # Directory containing log files
output_csv = "spmv_results.csv"
plots_dir = "spmv_plots"

# Improved Regex patterns
norm_y_pattern = r"\|\|y\|\|\s*=\s*([\d\.eE+-]+)"
norm_y2_pattern = r"\|\|y2\|\|\s*=\s*([\d\.eE+-]+)"
mpispvm_pattern = r"MPISpMV:\s*([\d\.eE+-]+)"
spvm_pattern = r"SpMV:\s*([\d\.eE+-]+)"
filename_pattern = r"log-NX(\d+)-NPI(\d+)"

# Ensure output directories exist
Path(plots_dir).mkdir(parents=True, exist_ok=True)

results = []

# Process log files
for filename in os.listdir(log_directory):
    if filename.startswith("log-") and not filename.startswith("."):
        filepath = os.path.join(log_directory, filename)
        try:
            with open(filepath, "r") as file:
                content = file.read()

            # Extract NX and NPI
            filename_match = re.search(filename_pattern, filename)
            if filename_match:
                nx = int(filename_match.group(1))
                npi = int(filename_match.group(2))
            else:
                continue

            norm_y = float(re.search(norm_y_pattern, content).group(1))
            norm_y2 = float(re.search(norm_y2_pattern, content).group(1))
            mpispvm_time = float(re.search(mpispvm_pattern, content).group(1))
            spvm_time = float(re.search(spvm_pattern, content).group(1))

            speedup = spvm_time / mpispvm_time
            efficiency = speedup / npi

            results.append({
                "File": filename,
                "NX": nx,
                "NPI": npi,
                "Norm_Y": norm_y,
                "Norm_Y2": norm_y2,
                "MPI_SpMV_Time": mpispvm_time,
                "SpMV_Time": spvm_time,
                "Speedup": speedup,
                "Efficiency": efficiency
            })
        except Exception as e:
            print(f"Error in {filename}: {e}")

# Create and sort DataFrame
df = pd.DataFrame(results).sort_values(by=["NX", "NPI"])
df.to_csv(output_csv, index=False)

# Visualization setup
plt.style.use('ggplot')
colors = plt.cm.plasma(np.linspace(0, 1, len(df["NX"].unique())))

# Execution Time Plot
fig, ax = plt.subplots(figsize=(10, 6))
for i, size in enumerate(sorted(df["NX"].unique())):
    subset = df[df["NX"] == size]
    ax.plot(subset["NPI"], subset["MPI_SpMV_Time"], 'o-', color=colors[i], label=f"NX={size}")
ax.set_title("MPI SpMV Execution Time")
ax.set_xlabel("Number of Processes")
ax.set_ylabel("Execution Time (s)")
ax.legend()
ax.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "execution_time.png"), dpi=300)

# Speedup Plot
fig, ax = plt.subplots(figsize=(10, 6))
for i, size in enumerate(sorted(df["NX"].unique())):
    subset = df[df["NX"] == size]
    ax.plot(subset["NPI"], subset["Speedup"], 'o-', color=colors[i], label=f"NX={size}")
ideal_npi = np.array(sorted(df["NPI"].unique()))
ax.plot(ideal_npi, ideal_npi, 'k--', label="Ideal Speedup")
ax.set_title("Parallel Speedup")
ax.set_xlabel("Number of Processes")
ax.set_ylabel("Speedup")
ax.legend()
ax.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "speedup.png"), dpi=300)

# Efficiency Plot
fig, ax = plt.subplots(figsize=(10, 6))
for i, size in enumerate(sorted(df["NX"].unique())):
    subset = df[df["NX"] == size]
    ax.plot(subset["NPI"], subset["Efficiency"], 'o-', color=colors[i], label=f"NX={size}")
ax.axhline(1, color='gray', linestyle='--', label="Ideal Efficiency")
ax.set_title("Parallel Efficiency")
ax.set_xlabel("Number of Processes")
ax.set_ylabel("Efficiency")
ax.legend()
ax.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "efficiency.png"), dpi=300)

# Combined Metrics Plot
fig, axes = plt.subplots(3, 1, figsize=(12, 16), sharex=True)
for metric, ax, ylabel in zip(["MPI_SpMV_Time", "Speedup", "Efficiency"], axes,
                              ["Time (s)", "Speedup", "Efficiency"]):
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        ax.plot(subset["NPI"], subset[metric], 'o-', color=colors[i], label=f"NX={size}")
    ax.set_ylabel(ylabel)
    ax.grid(True)
    ax.legend()
axes[0].set_title("Performance Metrics Overview")
axes[2].set_xlabel("Number of Processes")
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "combined_performance.png"), dpi=300)

print("Analysis and visualization complete!")
