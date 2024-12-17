import csv

# Input and output file paths
input_file = "boids.log"
output_file = "boids_benchmark.csv"

# Open the log file and create a CSV file
with open(input_file, "r") as log_file, open(output_file, "w", newline="") as csv_file:
    # Define the CSV writer
    writer = csv.writer(csv_file)
    # Write the header row
    writer.writerow(["Mode", "nbThreads", "NumBoids", "Time"])

    # Process each line in the log file
    for line in log_file:
        # Parse the log line to extract key-value pairs
        parts = line.split()
        mode = parts[1]  # Extract Mode value
        nb_threads = parts[3]  # Extract nbThreads value
        num_boids = parts[5]  # Extract NumBoids value
        time = parts[7]  # Extract Time value

        # Write the extracted values to the CSV file
        writer.writerow([mode, nb_threads, num_boids, time])

print(f"Data has been successfully converted to {output_file}")
