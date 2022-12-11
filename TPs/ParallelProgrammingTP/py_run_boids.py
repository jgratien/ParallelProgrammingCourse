import os

def run(t):
    # csv_path = f"/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgramming/results/{t}.csv"
    nb_threads = [1, 2, 4, 8, 16, 24]
    nb_birds = [256, 512, 1024, 2048, 4096, 8192]

    for nt in nb_threads:
        for nb in nb_birds:
            os.system(f"./bin/life-of-boids-{t} --nb-threads {nt} --nb-birds {nb}")

if __name__ == "__main__":

    run("omp")
    run("tbb")

