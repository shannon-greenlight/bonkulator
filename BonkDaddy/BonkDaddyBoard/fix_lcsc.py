import csv
import re
import os
import sys

# Get the input file path from the command-line arguments
input_file_path = sys.argv[1]
output_file_path = 'output.csv'

# Compile the regular expression for "SK1, "
pattern = re.compile(r'SK1, ')

# Open the input file in read mode and output file in write mode
with open(input_file_path, 'r') as input_file, open(output_file_path, 'w', newline='') as output_file:
    reader = csv.reader(input_file)
    writer = csv.writer(output_file)

    # Iterate over each row in the input file
    for row in reader:
        # Replace "SK1, " with "U1, " in each field
        row = [pattern.sub('U1, ', field) for field in row]
        # Write the modified row to the output file
        writer.writerow(row)

# Delete the original file
os.remove(input_file_path)

# Rename the output file to the original file name
os.rename(output_file_path, input_file_path)