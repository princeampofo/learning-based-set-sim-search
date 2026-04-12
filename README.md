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

`trainL2P.py` runs the cascade Siamese network training. This is the most
time-consuming step and constitutes the majority of LES3's construction time.

**Time this step — it is part of LES3's reported construction time:**
```bash
time python3 Partition/trainL2P.py
```

Before running, update these two variables at the top of the script:

```python
# For kosarak:
path = "datasets/kosarak/all.dat"
num_sets = 990002   # from: wc -l datasets/kosarak/all.dat to get line count
```

This produces group files: `all.dat-group-0` through `all.dat-group-5`.

### Choosing the Right Partition File

Copy the chosen file group to `LES3`:
```bash
# eg. For RETAIL:
cp datasets/retail/all.dat-group-2 datasets/retail/LES3
```

## Step 6: Configure `test_search.cpp`

Update the dataset paths in `test_search.cpp` to point to your `all.dat` and `LES3` files. These paths are used by both LES3 and DualTrans for loading the dataset and the LES3 partitions, respectively.

## Step 7: Compile
```bash
cd Search
g++ -o test_search test_search.cpp
```

---

## Step 8: Run

From inside the `Search/` directory, run and save output:

```bash
# Save to file and print to terminal simultaneously
./test_search | tee results_retail.txt

# Repeat for each dataset after updating test_search.cpp and recompiling
./test_search | tee results_kosarak.txt
./test_search | tee results_lastfm.txt
```
