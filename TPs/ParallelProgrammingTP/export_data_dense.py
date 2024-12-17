import os
import re
import pandas as pd

# Function to extract information from a single log file
def extract_benchmark_data(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    # Extract the DenseMV and related values
    densemv_pattern = r"DenseMV:([\d\.e\+-]+)"
    omp_densemv_pattern = r"OMPDenseMV:([\d\.e\+-]+)"
    omp_task_densemv_pattern = r"OMPTaskDenseMV:([\d\.e\+-]+)"
    omp_tile_densemv_pattern = r"OMPTileDenseMV:([\d\.e\+-]+)"
    tbb_densemv_pattern = r"TBBDenseMV:([\d\.e\+-]+)"
    tbb_range_densemv_pattern = r"TBBRangeDenseMV:([\d\.e\+-]+)"
    tbb_range2d_densemv_pattern = r"TBBRange2DDenseMV:([\d\.e\+-]+)"
    
    # Search for the DenseMV related values
    densemv_value = re.search(densemv_pattern, content)
    omp_densemv_value = re.search(omp_densemv_pattern, content)
    omp_task_densemv_value = re.search(omp_task_densemv_pattern, content)
    omp_tile_densemv_value = re.search(omp_tile_densemv_pattern, content)
    tbb_densemv_value = re.search(tbb_densemv_pattern, content)
    tbb_range_densemv_value = re.search(tbb_range_densemv_pattern, content)
    tbb_range2d_densemv_value = re.search(tbb_range2d_densemv_pattern, content)
    
    # Extract the DenseMV values, if found
    densemv_value = float(densemv_value.group(1)) if densemv_value else None
    omp_densemv_value = float(omp_densemv_value.group(1)) if omp_densemv_value else None
    omp_task_densemv_value = float(omp_task_densemv_value.group(1)) if omp_task_densemv_value else None
    omp_tile_densemv_value = float(omp_tile_densemv_value.group(1)) if omp_tile_densemv_value else None
    tbb_densemv_value = float(tbb_densemv_value.group(1)) if tbb_densemv_value else None
    tbb_range_densemv_value = float(tbb_range_densemv_value.group(1)) if tbb_range_densemv_value else None
    tbb_range2d_densemv_value = float(tbb_range2d_densemv_value.group(1)) if tbb_range2d_densemv_value else None
    
    # Extract the NX and NT values from the filename
    file_name = os.path.basename(file_path)
    nx_value = re.search(r'NX(\d+)', file_name)
    nt_value = re.search(r'NT(\d+)', file_name)
    
    nx_value = int(nx_value.group(1)) if nx_value else None
    nt_value = int(nt_value.group(1)) if nt_value else None
    
    return {
        'nx': nx_value,
        'nb_threads': nt_value,
        'densemv': densemv_value,
        'ompdensemv': omp_densemv_value,
        'omptaskdensemv': omp_task_densemv_value,
        'omptiledensemv': omp_tile_densemv_value,
        'tbbdensemv': tbb_densemv_value,
        'tbbrangedensemv': tbb_range_densemv_value,
        'tbbrange2ddensemv': tbb_range2d_densemv_value
    }

# Function to process all log files in a directory with the specific format
def process_benchmark_logs(directory):
    benchmark_data = []
    
    for file_name in os.listdir(directory):
        file_path = os.path.join(directory, file_name)
        
        # Process only files with the format 'log-NX{nx}-NT{np}'
        if file_name.startswith('log-NX') and 'NT' in file_name:
            data = extract_benchmark_data(file_path)
            benchmark_data.append(data)
    
    # Convert the extracted data into a Pandas DataFrame
    df = pd.DataFrame(benchmark_data)
    return df

# Example usage: Process all logs in a specific directory
log_directory = '.'  # Replace with your directory
df = process_benchmark_logs(log_directory)

# Print the resulting DataFrame
print(df)

# Optionally, save the DataFrame to CSV for further analysis
df.to_csv('results_densemv.csv', index=False)