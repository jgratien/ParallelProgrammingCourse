import os
import re
import pandas as pd

# Function to extract information from a single log file
def extract_benchmark_data(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    
    # Regular expressions to capture the values
    spmv_pattern = r"MPI_SpMV:([\d\.e\+-]+)"
    spmv_seq_pattern = r"SpMV:([\d\.e\+-]+)"
    
    # Search for the values in the content
    mpi_spmv_value = re.search(spmv_pattern, content)
    spmv_value = re.search(spmv_seq_pattern, content)
    
    # Extract the values, if found
    mpi_spmv_value = float(mpi_spmv_value.group(1)) if mpi_spmv_value else None
    spmv_value = float(spmv_value.group(1)) if spmv_value else None
    
    # Extract the NX and NPI values from the filename
    file_name = os.path.basename(file_path)
    nx_value = re.search(r'NX(\d+)', file_name)
    np_value = re.search(r'NPI(\d+)', file_name)
    
    nx_value = int(nx_value.group(1)) if nx_value else None
    np_value = int(np_value.group(1)) if np_value else None
    
    # Return the extracted data as a dictionary
    return {
        'nx': nx_value,
        'np': np_value,
        'MPI_SpMV_time': mpi_spmv_value,
        'SpMV_time': spmv_value
    }

# Function to process all log files in a directory with the specific format
def process_benchmark_logs(directory):
    benchmark_data = []
    
    for file_name in os.listdir(directory):
        file_path = os.path.join(directory, file_name)
        
        # Process only files with the format 'log-NX{nx}-NPI{np}'
        if file_name.startswith('log-NX') and 'NPI' in file_name:
            data = extract_benchmark_data(file_path)
            benchmark_data.append(data)
    
    # Convert the extracted data into a Pandas DataFrame
    df = pd.DataFrame(benchmark_data)
    return df

# Example usage: Process all logs in a specific directory
log_directory = '.'
df = process_benchmark_logs(log_directory)

df.to_csv('results_spmv.csv', index=False)
