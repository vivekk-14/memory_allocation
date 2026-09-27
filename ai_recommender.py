# ai_recommender.py
# loads the saved model and tells us which memory allocation strategy
# works best for the given input. also runs all 3 algos manually to verify.

import os
import pickle
import numpy as np
import pandas as pd


# this function just runs all three allocation algorithms and tells
# us how many processes got allocated and how much memory is left unused
def simulate(blocks, processes):

    # first fit - scan from start, take first block that fits
    temp = blocks.copy()
    ff = 0
    for p in processes:
        for i in range(len(temp)):
            if temp[i] >= p:
                temp[i] = temp[i] - p
                ff += 1
                break
    leftover_ff = sum(temp)

    # best fit - find the smallest block that still fits the process
    temp2 = blocks.copy()
    bf = 0
    for p in processes:
        chosen = -1
        smallest = float('inf')
        for i in range(len(temp2)):
            rem = temp2[i] - p
            if rem >= 0 and rem < smallest:
                smallest = rem
                chosen = i
        if chosen >= 0:
            temp2[chosen] -= p
            bf += 1
    leftover_bf = sum(temp2)

    # worst fit - pick the biggest available block each time
    temp3 = blocks.copy()
    wf = 0
    for p in processes:
        chosen = -1
        biggest = -1
        for i in range(len(temp3)):
            rem = temp3[i] - p
            if rem >= 0 and rem > biggest:
                biggest = rem
                chosen = i
        if chosen >= 0:
            temp3[chosen] -= p
            wf += 1
    leftover_wf = sum(temp3)

    return {
        "First Fit": {"alloc": ff, "free": leftover_ff},
        "Best Fit":  {"alloc": bf,  "free": leftover_bf},
        "Worst Fit": {"alloc": wf,  "free": leftover_wf},
    }


def predict_best_strategy(blocks, processes):

    # check if the model file exists, if not we need to train first
    if not os.path.exists("memory_ai_model.pkl"):
        print("model file missing - running training first")
        import train_ai
        train_ai.train_model()

    f = open("memory_ai_model.pkl", "rb")
    data = pickle.load(f)
    f.close()

    model = data["model"]
    cols  = data["features"]

    # calculate workload stats to feed into the model
    tot_mem  = sum(blocks)
    tot_req  = sum(processes)
    avg_p    = float(np.mean(processes))
    avg_b    = float(np.mean(blocks))
    max_b    = max(blocks)
    max_p    = max(processes)
    min_p    = min(processes)
    pressure = tot_req / tot_mem if tot_mem > 0 else 0.0

    # need std dev but only if more than one element
    sd_p = float(np.std(processes)) if len(processes) > 1 else 0.0
    sd_b = float(np.std(blocks))    if len(blocks) > 1    else 0.0

    ratio = max_p / max_b if max_b > 0 else 0.0

    # put everything into a single-row dataframe matching training columns
    sample = {
        "num_blocks":        len(blocks),
        "total_memory":      tot_mem,
        "num_processes":     len(processes),
        "total_requested":   tot_req,
        "mean_process_size": round(avg_p, 2),
        "std_process_size":  round(sd_p, 2),
        "memory_pressure":   round(pressure, 4),
        "max_block_size":    max_b,
        "max_process_size":  max_p,
        "min_process_size":  min_p,
        "std_block_size":    round(sd_b, 2),
        "mean_block_size":   round(avg_b, 2),
        "max_size_ratio":    round(ratio, 4)
    }
    df_input = pd.DataFrame([sample])[cols]

    result    = model.predict(df_input)[0]
    conf      = model.predict_proba(df_input)[0]
    labels    = model.classes_

    # simulate all three and compare with what the model predicted
    sim_out = simulate(blocks, processes)

    print()
    print("--------------------------------------------------------------")
    print("  Memory Allocation Recommender (AI)")
    print("--------------------------------------------------------------")
    print(f"  Input blocks    : {blocks}")
    print(f"  Input processes : {processes}")
    print(f"  Total memory    : {tot_mem} KB   Total requested : {tot_req} KB")
    print(f"  Memory pressure : {pressure:.2f}   Process std dev : {sd_p:.1f}")
    print()
    print(f"  Recommended strategy  =>  {result}")
    print()
    print("  Model confidence:")
    for lbl, c in zip(labels, conf):
        filled = int(c * 30)
        print(f"    {lbl:<12}  {c*100:.1f}%  {'|' * filled}")
    print()
    print("  Simulation results (all three algorithms):")
    print(f"    {'Strategy':<13}  Allocated  Free memory")
    for name, r in sim_out.items():
        marker = "  <-- AI pick" if name == result else ""
        print(f"    {name:<13}  {r['alloc']}/{len(processes)}        {r['free']} KB{marker}")
    print("--------------------------------------------------------------")
    print()


if __name__ == "__main__":

    print("Memory Allocation Strategy Predictor")
    print("1. Use default test workload")
    print("2. Enter custom workload")
    ch = input("Choice (1/2): ").strip()

    if ch == "2":
        blocks    = [int(x) for x in input("Block sizes (space separated): ").split()]
        processes = [int(x) for x in input("Process sizes (space separated): ").split()]
    else:
        blocks    = [100, 500, 200, 300, 600]
        processes = [212, 417, 112, 426]

    predict_best_strategy(blocks, processes)
