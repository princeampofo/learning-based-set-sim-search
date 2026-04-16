# Reproducing LES3 and DualTrans Baselines
## From: "A Length Enhanced B+-tree based index for Efficient Set Similarity Query" (ICDE 2025)

This guide documents the full steps to reproduce the LES3 and DualTrans baseline
results used in the LeB paper. The implementation is sourced from the original
LES3 repository: https://github.com/AwesomeYifan/learning-based-set-sim-search

### Notes on Reproducibility
- Results may differ from the paper due to hardware, compiler, and platform differences.

## Step 1: Process Raw Datasets

`process_database.py` tokenizes and sorts the raw data into `all.dat`. It also
prints the number of distinct tokens, which you need for Step 2.

Run from the repo root:
```bash
python3 Partition/process_database.py
```

## Step 2: Generate Set Representations (PTR)

`test_embedding.py` generates PTR representations used by the Siamese network.

Run:
```bash
python3 Partition/test_embedding.py
```

This produces `all.dat-rep-PTR` in your dataset folder.

---

## Step 3: Train LES3 Partitions (L2P)

`trainL2P.py` runs the cascade Siamese network training — the most time-consuming step of LES3 construction.

```bash
python3 Partition/trainL2P.py
```

Before running, update these variables at the top of the script:

```python
path     = "datasets/<dataset>/all.dat"
num_sets = <line count of all.dat>   # use: wc -l datasets/<dataset>/all.dat
```

The LES paper recommends setting the number of final groups to **~0.5% of |D|**. Given that `initialize()` starts with 128 groups, set the cascade range accordingly.

<<<<<<< HEAD
Adjust `range(0, n)` in the script to match your dataset size. Always use the **final** group file for TGM construction:
e.g. for retail we use `range(0, 2)` and for lastfm and kosarak we use `range(0, 5)`.

```bash
cp datasets/<dataset>/all.dat-group-<n> datasets/<dataset>/LES3
=======
### Choosing the Right Partition File

Copy the chosen file group to `LES3`:
```bash
# eg. For RETAIL:
cp datasets/retail/all.dat-group-2 datasets/retail/LES3
>>>>>>> a67531c (chore: add per-dataset results)
```

## Step 4: Run experiments
Run bash `run_experiments.sh` to execute LES3 and DualTrans on the test sets. This produces the final CSV results file.
```bash
bash run_experiments.sh
```
