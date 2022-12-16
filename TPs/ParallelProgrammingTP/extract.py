import os

def extract(pattern, benchmark_fname):

    p = "oarlogs"
    log_files = [fname for fname in os.listdir(p) if fname.startswith("log")]
    w = open(benchmark_fname, "w")
    w.write("np, nx, time_seq, time_mpi, speedup")
    for log_f in log_files:
        with open(os.path.join(p, log_f)) as f:
            to_write = [None] * 5
            tmp = log_f.split("-")[1:]
            to_write[1] = tmp[0][2:].strip()
            to_write[0] = tmp[1][3:].strip()
            for l in f.readlines():
                if l.startswith(pattern):
                    if "MPI" in l:
                        to_write[3] = l.split(":")[1].strip()
                    else:
                        to_write[2] = l.split(":")[1].strip()

            to_write[4] = str(float(to_write[2])/float(to_write[3]))
        w.write("\n" + ",".join(to_write))

    w.close()
if __name__ == "__main__":
    # one call at a time. I clean oarlogs dir beforehand
    extract("SparseMV", "spmv-benchmarks")
    # extract("DenseMV", "spmv-benchmarks")

