# ai_recommender.py
# saved model load chesi which memory allocation strategy best o cheptundi
# anni 3 algos manually run chesi verify kooda chesamu

import os
import pickle
import numpy as np
import pandas as pd


# ee function anni 3 algorithms run chesi results return chesamu
# evvaro processes allocate ayyayo, evvaro memory waste ayyindo cheppadaniki
def simulate(blocks, processes):

    # first fit - list start nunchi scan, first fit ayye block ki allocate
    temp = blocks.copy()  # original blocks modify avvakunda copy use chesamu
    ff = 0
    for p in processes:
        for i in range(len(temp)):
            if temp[i] >= p:
                temp[i] = temp[i] - p
                ff += 1
                break  # first fit - dorikithe stop
    leftover_ff = sum(temp)

    # best fit - anni blocks scan chesi smallest leftover isthe block choose chesamu
    temp2 = blocks.copy()
    bf = 0
    for p in processes:
        chosen = -1
        smallest = float('inf')  # initially chala pedda number
        for i in range(len(temp2)):
            rem = temp2[i] - p
            if rem >= 0 and rem < smallest:  # ee block better unte update chesamu
                smallest = rem
                chosen = i
        if chosen >= 0:
            temp2[chosen] -= p
            bf += 1
    leftover_bf = sum(temp2)

    # worst fit - biggest available block ni pick chesamu
    # idea: peddha hole use chesthe leftover kooda peddha ga untundi - future ki useful
    temp3 = blocks.copy()
    wf = 0
    for p in processes:
        chosen = -1
        biggest = -1
        for i in range(len(temp3)):
            rem = temp3[i] - p
            if rem >= 0 and rem > biggest:  # best fit ki opposite - max kosam chusthamu
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

    # model file unda leeda chekc chesamu - lekapothe training run chesamu
    if not os.path.exists("memory_ai_model.pkl"):
        print("model file missing - running training first")
        import train_ai
        train_ai.train_model()

    # pkl file open chesi model load chesamu
    f = open("memory_ai_model.pkl", "rb")
    data = pickle.load(f)
    f.close()

    model = data["model"]
    cols  = data["features"]  # training lo use chesina same column order kavali

    # workload stats calculate chesamu - ivi model ki input avutayi
    tot_mem  = sum(blocks)
    tot_req  = sum(processes)
    avg_p    = float(np.mean(processes))
    avg_b    = float(np.mean(blocks))
    max_b    = max(blocks)
    max_p    = max(processes)
    min_p    = min(processes)
    # memory pressure - demand vs supply ratio
    # high unte memory tight ga undi ani artham - best fit suggest avutundi
    pressure = tot_req / tot_mem if tot_mem > 0 else 0.0

    # std dev calculate chesamu - oka element unte std dev 0 avutundi so check chesamu
    sd_p = float(np.std(processes)) if len(processes) > 1 else 0.0
    sd_b = float(np.std(blocks))    if len(blocks) > 1    else 0.0

    # biggest process vs biggest block ratio - fit avutunda leeda ani telustundi
    ratio = max_p / max_b if max_b > 0 else 0.0

    # anni features oka dictionary lo pettamu - model ki feed cheyyadaniki
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
    # column order training tho exactly match avvaali - lekapothe wrong prediction vastundi
    df_input = pd.DataFrame([sample])[cols]

    result    = model.predict(df_input)[0]       # predicted strategy
    conf      = model.predict_proba(df_input)[0]  # each strategy ki confidence percentage
    labels    = model.classes_                    # class names alphabetical order lo untayi

    # actual ga 3 algorithms run chesi model prediction verify chesamu
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
        filled = int(c * 30)  # percentage ni bar length ga convert chesamu
        print(f"    {lbl:<12}  {c*100:.1f}%  {'|' * filled}")
    print()
    print("  Simulation results (all three algorithms):")
    print(f"    {'Strategy':<13}  Allocated  Free memory")
    for name, r in sim_out.items():
        marker = "  <-- AI pick" if name == result else ""  # AI pick ni highlight chesamu
        print(f"    {name:<13}  {r['alloc']}/{len(processes)}        {r['free']} KB{marker}")
    print("--------------------------------------------------------------")
    print()


if __name__ == "__main__":

    print("Memory Allocation Strategy Predictor")
    print("1. Use default test workload")
    print("2. Enter custom workload")
    ch = input("Choice (1/2): ").strip()

    if ch == "2":
        # user custom values enter chesthe split chesi list lo store chesamu
        blocks    = [int(x) for x in input("Block sizes (space separated): ").split()]
        processes = [int(x) for x in input("Process sizes (space separated): ").split()]
    else:
        # default test values - C simulator tho same workload use chesamu
        blocks    = [100, 500, 200, 300, 600]
        processes = [212, 417, 112, 426]

    predict_best_strategy(blocks, processes)
