# train_ai.py
# Generates training data and trains the random forest model.
# Saves the model to memory_ai_model.pkl and the dataset to memory_dataset.csv
# DSA Mini Project - Topic 19

import random
import pickle
import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report


# ---------- simulation helpers ----------

def simulate_first_fit(blocks, processes):
    b = blocks.copy()
    count = 0
    for p in processes:
        for i in range(len(b)):
            if b[i] >= p:
                b[i] -= p
                count += 1
                break
    return count, sum(b)


def simulate_best_fit(blocks, processes):
    b = blocks.copy()
    count = 0
    for p in processes:
        pick = -1
        smallest_gap = float('inf')
        for i in range(len(b)):
            gap = b[i] - p
            if gap >= 0 and gap < smallest_gap:
                smallest_gap = gap
                pick = i
        if pick != -1:
            b[pick] -= p
            count += 1
    return count, sum(b)


def simulate_worst_fit(blocks, processes):
    b = blocks.copy()
    count = 0
    for p in processes:
        pick = -1
        biggest_gap = -1
        for i in range(len(b)):
            gap = b[i] - p
            if gap >= 0 and gap > biggest_gap:
                biggest_gap = gap
                pick = i
        if pick != -1:
            b[pick] -= p
            count += 1
    return count, sum(b)


# ---------- dataset generation ----------

def generate_dataset(num_samples=5000):
    print(f"Building dataset with {num_samples} workload samples...")

    # fix seeds so results are reproducible
    random.seed(123)
    np.random.seed(123)

    rows = []

    for i in range(num_samples):
        nb = random.randint(3, 8)
        np_ = random.randint(4, 12)

        # cycle through 3 workload types to get balanced classes
        wtype = i % 3

        if wtype == 0:
            # tight partitions - Best Fit tends to do well here
            base = random.randint(150, 450)
            blocks    = [base + random.randint(-30, 50) for _ in range(nb)]
            processes = [base - random.randint(5, 40)   for _ in range(np_)]

        elif wtype == 1:
            # big blocks, mixed request sizes - Worst Fit keeps large holes open
            blocks    = [random.randint(400, 1000) for _ in range(nb)]
            processes = [
                random.randint(25, 75) if j % 2 == 0 else random.randint(250, 400)
                for j in range(np_)
            ]

        else:
            # general balanced workload - First Fit is usually fine
            blocks    = [random.randint(150, 650) for _ in range(nb)]
            processes = [random.randint(50, 300)  for _ in range(np_)]

        ff_alloc, ff_free = simulate_first_fit(blocks, processes)
        bf_alloc, bf_free = simulate_best_fit(blocks, processes)
        wf_alloc, wf_free = simulate_worst_fit(blocks, processes)

        # feature extraction
        total_mem  = sum(blocks)
        total_req  = sum(processes)
        avg_proc   = float(np.mean(processes))
        sd_proc    = float(np.std(processes))
        pressure   = total_req / total_mem if total_mem > 0 else 0
        max_b      = max(blocks)
        max_p      = max(processes)
        min_p      = min(processes)
        sd_b       = float(np.std(blocks))
        avg_b      = float(np.mean(blocks))
        ratio      = max_p / max_b if max_b > 0 else 0

        # score: more allocations = better; less wasted memory = better
        ff_score = ff_alloc * 1000 - ff_free
        bf_score = bf_alloc * 1000 - bf_free
        wf_score = wf_alloc * 1000 - wf_free

        # when all three tie on allocations, use memory pressure to break ties
        if ff_alloc == bf_alloc == wf_alloc:
            if pressure > 0.65:
                bf_score += 200       # high pressure → minimize fragmentation
            elif sd_proc > 80 and pressure < 0.55:
                wf_score += 200       # varied sizes + low pressure → keep big holes
            else:
                ff_score += 200       # default: first fit is fastest

        scores = {"First Fit": ff_score, "Best Fit": bf_score, "Worst Fit": wf_score}
        winner = max(scores, key=scores.get)

        rows.append({
            "num_blocks":        nb,
            "total_memory":      total_mem,
            "num_processes":     np_,
            "total_requested":   total_req,
            "mean_process_size": round(avg_proc, 2),
            "std_process_size":  round(sd_proc, 2),
            "memory_pressure":   round(pressure, 4),
            "max_block_size":    max_b,
            "max_process_size":  max_p,
            "min_process_size":  min_p,
            "std_block_size":    round(sd_b, 2),
            "mean_block_size":   round(avg_b, 2),
            "max_size_ratio":    round(ratio, 4),
            "best_algorithm":    winner
        })

    df = pd.DataFrame(rows)
    df.to_csv("memory_dataset.csv", index=False)
    print(f"Saved dataset → memory_dataset.csv  (shape: {df.shape})")
    print("Class counts:")
    print(df["best_algorithm"].value_counts())
    return df


# ---------- training ----------

def train_model():

    df = generate_dataset(5000)

    features = [
        "num_blocks", "total_memory", "num_processes", "total_requested",
        "mean_process_size", "std_process_size", "memory_pressure",
        "max_block_size", "max_process_size", "min_process_size",
        "std_block_size", "mean_block_size", "max_size_ratio"
    ]

    X = df[features]
    y = df["best_algorithm"]

    # 80/20 split, stratified so each class is represented in test set
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )

    print("\nTraining Random Forest...")
    clf = RandomForestClassifier(
        n_estimators=180,
        max_depth=14,
        min_samples_split=4,
        random_state=42
    )
    clf.fit(X_train, y_train)

    train_pred = clf.predict(X_train)
    test_pred  = clf.predict(X_test)

    train_acc = accuracy_score(y_train, train_pred)
    test_acc  = accuracy_score(y_test,  test_pred)

    print(f"\n  Train accuracy : {train_acc*100:.2f}%")
    print(f"  Test  accuracy : {test_acc*100:.2f}%")

    print("\nPer-class report:")
    print(classification_report(y_test, test_pred))

    # feature importance - useful to understand what drives predictions
    print("Feature importances:")
    imp = pd.Series(clf.feature_importances_, index=features).sort_values(ascending=False)
    for feat, val in imp.items():
        print(f"  {feat:<22}: {val*100:.1f}%")

    # save model + feature list together so the recommender knows the column order
    with open("memory_ai_model.pkl", "wb") as f:
        pickle.dump({"model": clf, "features": features}, f)

    print("\nModel saved to memory_ai_model.pkl")


if __name__ == "__main__":
    train_model()
