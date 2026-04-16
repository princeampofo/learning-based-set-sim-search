#!/usr/bin/env bash
# run_experiments.sh

set -euo pipefail

# Config
BINARY="./Search/test_search"          # path to compiled binary
DATASETS_ROOT="./datasets"             # root folder for all datasets
RESULTS_DIR="$(pwd)/results"                # output folder for CSV files
LOG_DIR="$(pwd)/results/logs"               # per-run stdout logs

# List of dataset names (must match subfolder names under $DATASETS_ROOT)
DATASETS=(
<<<<<<< HEAD
    'kosarak'
    'lastfm'
    'retail'
=======
    "retail"
    "kosarak"
    "lastfm"
>>>>>>> a67531c (chore: add per-dataset results)
)

# File names within each dataset folder
SETS_FILE="all.dat"
GROUPS_FILE="LES3"

# Setup
mkdir -p "$RESULTS_DIR" "$LOG_DIR"

# Compile 
echo "Compiling test_search ..."
g++ -std=c++17 -O2 \
    -o "$BINARY" \
    ./Search/test_search.cpp \
    -I./Search \
    2>&1
if [[ $? -ne 0 ]]; then
    echo "ERROR: compilation failed. Fix errors above and re-run."
    exit 1
fi
echo "Compilation successful."
echo ""

echo "============================================================"
echo "  Experiment runner"
echo "  Binary   : $BINARY"
echo "  Datasets : ${DATASETS[*]}"
echo "  Results  → $RESULTS_DIR"
echo "============================================================"
echo ""

TOTAL=${#DATASETS[@]}
IDX=0

for DATASET in "${DATASETS[@]}"; do
    IDX=$((IDX + 1))
    SETS_PATH="$DATASETS_ROOT/$DATASET/$SETS_FILE"
    GROUPS_PATH="$DATASETS_ROOT/$DATASET/$GROUPS_FILE"
    LOG_FILE="$LOG_DIR/${DATASET}.log"

    echo "[$IDX/$TOTAL] Dataset: $DATASET"
    echo "  Sets   : $SETS_PATH"
    echo "  Groups : $GROUPS_PATH"

    # Validate dataset files exist 
    if [[ ! -f "$SETS_PATH" ]]; then
        echo "  SKIP: sets file not found → $SETS_PATH"
        echo ""
        continue
    fi
    if [[ ! -f "$GROUPS_PATH" ]]; then
        echo "  SKIP: groups file not found → $GROUPS_PATH"
        echo ""
        continue
    fi

    echo "  Running... (log → $LOG_FILE)"
    START_TS=$(date +%s)

    # Run benchmark; tee output to log and stdout
    DATASET_CSV="$RESULTS_DIR/${DATASET}_results.csv"
    "$BINARY" "$DATASET" "$SETS_PATH" "$GROUPS_PATH" "$DATASET_CSV" 2>&1 | tee "$LOG_FILE"

    END_TS=$(date +%s)
    ELAPSED=$((END_TS - START_TS))
    echo "  Finished in ${ELAPSED}s"
    echo ""
done

<<<<<<< HEAD
# # Merge all per-dataset CSVs into one combined CSV 
# COMBINED="$RESULTS_DIR/all_results.csv"
# echo "Merging results into: $COMBINED"

# # Write header once
# echo "method,dataset,delta,avg_time_ms,avg_candidates,construction_time_min,index_size_mb" \
#     > "$COMBINED"

# for DATASET in "${DATASETS[@]}"; do
#     DATASET_CSV="$RESULTS_DIR/${DATASET}_results.csv"
#     if [[ -f "$DATASET_CSV" ]]; then
#         # Skip header row (line 1) when appending
#         tail -n +2 "$DATASET_CSV" >> "$COMBINED"
#     fi
# done

echo ""
echo "============================================================"
echo "  Done. Results are in: $RESULTS_DIR"
=======
# Merge all per-dataset CSVs into one combined CSV 
COMBINED="$RESULTS_DIR/all_results.csv"
echo "Merging results into: $COMBINED"

# Write header once
echo "method,dataset,delta,avg_time_ms,avg_candidates,construction_time_min,index_size_mb" \
    > "$COMBINED"

for DATASET in "${DATASETS[@]}"; do
    DATASET_CSV="$RESULTS_DIR/${DATASET}_results.csv"
    if [[ -f "$DATASET_CSV" ]]; then
        # Skip header row (line 1) when appending
        tail -n +2 "$DATASET_CSV" >> "$COMBINED"
    fi
done

echo ""
echo "============================================================"
echo "  Done. Combined results: $COMBINED"
>>>>>>> a67531c (chore: add per-dataset results)
echo "============================================================"