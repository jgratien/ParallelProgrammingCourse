import pandas as pd
import numpy as np

import os
import sys

def getlogfiles(path):
    files = pd.Series(os.listdir(path))
    files = files[files.str.startswith("log")].values
    return files

def parseFileName(filename):
    l = filename.split("-")
    return l[1][2:], l[2][3:]

if __name__ == "__main__":


    if (len(sys.argv) != 2):
        print("Please provide only one argument from : 'spmv', 'densemv'")
        exit(-1)
    elif ((sys.argv[1] != 'spmv') and (sys.argv[1] != 'densemv')):
        print("Please provide only one argument from : 'spmv', 'densemv'")
        exit(-1)
    else:
        if sys.argv[1] == 'spmv':
            logfiles = getlogfiles("/gext/arthur.viens/ParallelProgrammingCourse/TPs/ParallelProgrammingTP")
            columns = ["logfile", "nx", "npi", "SpMV", "SpVM MPI"]
            logs = pd.DataFrame(columns = columns)
            names = ["OMPSpMV", "SpMV", "SpMV MPI"]
            for file in logfiles:
                nx, npi = parseFileName(file)
                #print(f"File {file} : nx = {nx}, npi = {npi}")
                with open(file) as f:
                    lines = pd.Series(f.readlines())
                    lines = lines[lines.str.startswith(("SpMV", "SpMV MPI"))].str[:-1]
                    assert len(lines) != 0, "Could not extract lines with SpMV or SpMV MPI result from {}. Are the log files right ?".format(file)
                timeSpMV = float(lines[lines.str.startswith("SpMV")].values[0].split(":")[1])
                timeSpMVMPI = float(lines[lines.str.startswith("SpMV MPI")].values[0].split(":")[1])
                row = pd.Series([file, nx, npi, timeSpMV, timeSpMVMPI], index=columns)
                logs = logs.append(row, ignore_index=True)
            logs.to_csv("SpMV_benchmark.csv", index=False, sep=";")
            print("Results from log file written to SpMV_benchmark.csv.")
        else:
            logfiles = getlogfiles("/gext/arthur.viens/ParallelProgrammingCourse/TPs/ParallelProgrammingTP")
            columns = ["logfile", "nx", "npi", "DenseMV", "DenseMV MPI"]
            logs = pd.DataFrame(columns = columns)
            names = ["DenseMV", "DenseMV MPI"]
            for file in logfiles:
                nx, npi = parseFileName(file)
                #print(f"File {file} : nx = {nx}, npi = {npi}")
                with open(file) as f:
                    lines = pd.Series(f.readlines())
                    lines = lines[lines.str.startswith(("DenseMV", "DenseMV MPI"))].str[:-1]
                    assert len(lines) != 0, "Could not extract lines with DenseMV or DenseMV MPI result from {}. Are the log files right ?".format(file)
                timeSpMV = float(lines[lines.str.startswith("DenseMV")].values[0].split(":")[1])
                timeSpMVMPI = float(lines[lines.str.startswith("DenseMV MPI")].values[0].split(":")[1])
                row = pd.Series([file, nx, npi, timeOMPSpMV, timeSpMV, timeSpMVMPI], index=columns)
                logs = logs.append(row, ignore_index=True)
            logs.to_csv("DenseMV_benchmark.csv", index=False, sep=";")
            print("Results from log file written to DenseMV_benchmark.csv.")
