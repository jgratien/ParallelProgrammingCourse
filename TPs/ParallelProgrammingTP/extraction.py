#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SpMV MPI Performance Analysis Tool
---------------------------------
This script analyzes performance data from sparse matrix-vector multiplication 
benchmark logs, extracting metrics and generating visualizations.

"""

import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

# Configuration parameters
log_directory = "."  # Directory containing log files
output_csv = "spmv_results.csv"  # Output CSV file name
plots_dir = "spmv_plots"  # Directory for output plots

# Regex patterns for data extraction
norm_y_pattern = r"\|\|y\|\|=(\d+)"
norm_y2_pattern = r"\|\|y2\|\| = (\d+)"
mpispvm_pattern = r"MPISpMV:(\d+\.\d+e[+-]?\d+)"
spvm_pattern = r"SpMV:(\d+\.\d+e[+-]?\d+)"
filename_pattern = r"log-NX(\d+)-NPI(\d+)"

# Create plots directory if it doesn't exist
Path(plots_dir).mkdir(parents=True, exist_ok=True)

# List to store results
results = []

# Process log files
print("Analyzing benchmark log files...")
for filename in os.listdir(log_directory):
    if filename.startswith("log-") and not filename.startswith("."):
        filepath = os.path.join(log_directory, filename)
        
        try:
            # Read file content
            with open(filepath, "r") as file:
                content = file.read()
            
            # Extract data from filename
            filename_match = re.search(filename_pattern, filename)
            if filename_match:
                nx = int(filename_match.group(1))
                npi = int(filename_match.group(2))
            else:
                print(f"Skipping malformatted filename: {filename}")
                continue
            
            # Extract data from file content
            norm_y_match = re.search(norm_y_pattern, content)
            norm_y = int(norm_y_match.group(1)) if norm_y_match else None
            
            norm_y2_match = re.search(norm_y2_pattern, content)
            norm_y2 = int(norm_y2_match.group(1)) if norm_y2_match else None
            
            mpispvm_match = re.search(mpispvm_pattern, content)
            mpispvm_time = float(mpispvm_match.group(1)) if mpispvm_match else None
            
            spvm_match = re.search(spvm_pattern, content)
            spvm_time = float(spvm_match.group(1)) if spvm_match else None
            
            # Calculate derived metrics
            if mpispvm_time and spvm_time and npi:
                speedup = spvm_time / mpispvm_time
                efficiency = speedup / npi
            else:
                speedup = None
                efficiency = None
            
            # Add results to our list
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
            print(f"Error processing {filename}: {e}")

# Create DataFrame and sort results
df = pd.DataFrame(results)
df = df.sort_values(by=["NX", "NPI"])

# Save to CSV
df.to_csv(output_csv, index=False)
print(f"Results saved to {output_csv}")

# Generate visualizations if we have data
if not df.empty:
    print("Generating performance visualizations...")
    
    # Set plot style for better appearance
    plt.style.use('ggplot')
    colors = plt.cm.viridis(np.linspace(0, 1, len(df["NX"].unique())))
    
    # Plot 1: Execution Time
    fig, ax = plt.subplots(figsize=(10, 6))
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        ax.plot(subset["NPI"], subset["MPI_SpMV_Time"], marker='o', 
                linewidth=2, markersize=8, color=colors[i],
                label=f"NX = {size}")
    
    ax.set_title("MPI Sparse Matrix-Vector Multiplication Performance", fontsize=16, fontweight='bold')
    ax.set_xlabel("Number of Processes (NPI)", fontsize=14)
    ax.set_ylabel("Execution Time (seconds)", fontsize=14)
    ax.legend(title="Matrix Size", fontsize=12, frameon=True, facecolor='white', framealpha=0.9)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xticks(sorted(df["NPI"].unique()))
    
    # Add annotations for min values
    for size in sorted(df["NX"].unique()):
        subset = df[df["NX"] == size]
        min_idx = subset["MPI_SpMV_Time"].idxmin()
        min_npi = subset.loc[min_idx, "NPI"]
        min_time = subset.loc[min_idx, "MPI_SpMV_Time"]
        ax.annotate(f"{min_time:.2e}s",        oarstat -u $USER 
                   xy=(min_npi, min_time),
                   xytext=(5, 5),
                   textcoords='offset points',
                   fontsize=9)
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "execution_time.png"), dpi=300)
    
    # Plot 2: Speedup
    fig, ax = plt.subplots(figsize=(10, 6))
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        ax.plot(subset["NPI"], subset["Speedup"], marker='o', 
                linewidth=2, markersize=8, color=colors[i],
                label=f"NX = {size}")
    
    # Add ideal speedup reference line
    processes = df["NPI"].unique()
    if len(processes) > 0:
        max_procs = max(processes)
        x_ideal = np.array([1, max_procs])
        y_ideal = np.array([1, max_procs])
        ax.plot(x_ideal, y_ideal, 'k--', alpha=0.7, linewidth=2, label="Ideal Speedup")
    
    ax.set_title("Parallel Speedup", fontsize=16, fontweight='bold')
    ax.set_xlabel("Number of Processes (NPI)", fontsize=14)
    ax.set_ylabel("Speedup (Sequential/Parallel)", fontsize=14)
    ax.legend(title="Matrix Size", fontsize=12, frameon=True, facecolor='white', framealpha=0.9)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xticks(sorted(df["NPI"].unique()))
    
    # Add annotations for max values
    for size in sorted(df["NX"].unique()):
        subset = df[df["NX"] == size]
        max_idx = subset["Speedup"].idxmax()
        max_npi = subset.loc[max_idx, "NPI"]
        max_speedup = subset.loc[max_idx, "Speedup"]
        if not pd.isna(max_speedup):
            ax.annotate(f"{max_speedup:.2f}x", 
                       xy=(max_npi, max_speedup),
                       xytext=(5, 5),
                       textcoords='offset points',
                       fontsize=9)
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "speedup.png"), dpi=300)
    
    # Plot 3: Efficiency
    fig, ax = plt.subplots(figsize=(10, 6))
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        ax.plot(subset["NPI"], subset["Efficiency"], marker='o', 
                linewidth=2, markersize=8, color=colors[i],
                label=f"NX = {size}")
    
    ax.axhline(y=1.0, color='r', linestyle='--', alpha=0.7, linewidth=2, label="Ideal Efficiency")
    
    ax.set_title("Parallel Efficiency", fontsize=16, fontweight='bold')
    ax.set_xlabel("Number of Processes (NPI)", fontsize=14)
    ax.set_ylabel("Efficiency (Speedup/Processes)", fontsize=14)
    ax.legend(title="Matrix Size", fontsize=12, frameon=True, facecolor='white', framealpha=0.9)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.set_xticks(sorted(df["NPI"].unique()))
    ax.set_ylim(bottom=0, top=min(1.1, df["Efficiency"].max() * 1.2))
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "efficiency.png"), dpi=300)
    
    # Additional plot: Combined performance metrics
    fig, axes = plt.subplots(3, 1, figsize=(12, 15), sharex=True)
    
    # Execution time subplot
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        axes[0].plot(subset["NPI"], subset["MPI_SpMV_Time"], marker='o', 
                    linewidth=2, markersize=8, color=colors[i],
                    label=f"NX = {size}")
    axes[0].set_title("MPI Sparse Matrix-Vector Multiplication Performance", fontsize=16, fontweight='bold')
    axes[0].set_ylabel("Execution Time (seconds)", fontsize=14)
    axes[0].legend(title="Matrix Size", fontsize=10, frameon=True, facecolor='white', framealpha=0.9)
    axes[0].grid(True, alpha=0.3, linestyle='--')
    
    # Speedup subplot
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        axes[1].plot(subset["NPI"], subset["Speedup"], marker='o', 
                    linewidth=2, markersize=8, color=colors[i],
                    label=f"NX = {size}")
    if len(processes) > 0:
        max_procs = max(processes)
        x_ideal = np.array([1, max_procs])
        y_ideal = np.array([1, max_procs])
        axes[1].plot(x_ideal, y_ideal, 'k--', alpha=0.7, linewidth=2, label="Ideal")
    axes[1].set_title("Parallel Speedup", fontsize=16, fontweight='bold')
    axes[1].set_ylabel("Speedup", fontsize=14)
    axes[1].grid(True, alpha=0.3, linestyle='--')
    
    # Efficiency subplot
    for i, size in enumerate(sorted(df["NX"].unique())):
        subset = df[df["NX"] == size]
        axes[2].plot(subset["NPI"], subset["Efficiency"], marker='o', 
                    linewidth=2, markersize=8, color=colors[i],
                    label=f"NX = {size}")
    axes[2].axhline(y=1.0, color='r', linestyle='--', alpha=0.7, linewidth=2, label="Ideal")
    axes[2].set_title("Parallel Efficiency", fontsize=16, fontweight='bold')
    axes[2].set_xlabel("Number of Processes (NPI)", fontsize=14)
    axes[2].set_ylabel("Efficiency", fontsize=14)
    axes[2].set_ylim(bottom=0, top=min(1.1, df["Efficiency"].max() * 1.2))
    axes[2].grid(True, alpha=0.3, linestyle='--')
    axes[2].set_xticks(sorted(df["NPI"].unique()))
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "combined_performance.png"), dpi=300)
    
    print(f"Visualizations saved to directory: {plots_dir}")

print("Analysis complete!")
