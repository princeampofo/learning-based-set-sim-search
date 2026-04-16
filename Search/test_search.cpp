//
//  test_search.cpp
//  LES3 / DualTrans benchmark runner
//
//  Usage:
//    ./test_search <dataset_name> <path_to_sets> <path_to_groups> <output_csv>
//

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <chrono>
#include <ctime>
#include <set>
#include <map>
#include <sstream>
<<<<<<< HEAD
#include <numeric>
#include <random>
=======
>>>>>>> a67531c (chore: add per-dataset results)
#include "LES3.h"
#include "BRUTEFORCE.h"
#include "DualTrans.h"
#include "PARAM.h"
#include "InvertedIndex.h"
using namespace std;


<<<<<<< HEAD
// Load all sets from file and sample num_queries with a fixed seed
vector<multiset<int>> loadAndSampleSets(const string& path_to_sets, int num_queries, unsigned seed = 42) {
    vector<multiset<int>> all_sets;
    ifstream in(path_to_sets);
    string line;
    while(getline(in, line)) {
        istringstream iss(line);
        string token;
        multiset<int> s;
        while(iss >> token) s.insert(stoi(token));
        if(!s.empty()) all_sets.push_back(s);
    }
    vector<int> indices(all_sets.size());
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), mt19937(seed));
    vector<multiset<int>> sampled;
    for(int i = 0; i < num_queries && i < (int)indices.size(); i++)
        sampled.push_back(all_sets[indices[i]]);
    return sampled;
}

// CSV helper
struct ResultRow {
    string method;
    string dataset;
    float  delta;
    double avg_time_ms;
    long long avg_candidates;
    double construction_time_min;
    float  index_size_mb;
};

void writeCSV(const string& path, const vector<ResultRow>& rows, bool append) {
    bool write_header = !append;
    if(append) {
        ifstream check(path);
        if(!check.good()) write_header = true;
    }

    ofstream fout;
    if(append) fout.open(path, ios::app);
    else       fout.open(path);

    if(!fout.is_open()) {
        cerr << "ERROR: cannot open result file: " << path << endl;
        cerr << "       Check that the directory exists and is writable." << endl;
        abort();
    }

    if(write_header) {
        fout << "method,dataset,delta,avg_time_ms,avg_candidates,"
                "construction_time_min,index_size_mb\n";
    }
    for(const auto& r : rows) {
        fout << r.method               << ","
             << r.dataset              << ","
             << r.delta                << ","
             << r.avg_time_ms          << ","
             << r.avg_candidates       << ","
             << r.construction_time_min << ","
             << r.index_size_mb        << "\n";
    }
    fout.flush();
    fout.close();
    cout << "Results written to: " << path << endl;
}


=======
// CSV helper
struct ResultRow {
    string method;
    string dataset;
    float  delta;
    double avg_time_ms;
    long long avg_candidates;
    double construction_time_min;
    float  index_size_mb;
};

void writeCSV(const string& path, const vector<ResultRow>& rows, bool append) {
    bool write_header = !append;
    if(append) {
        ifstream check(path);
        if(!check.good()) write_header = true;
    }

    ofstream fout;
    if(append) fout.open(path, ios::app);
    else       fout.open(path);

    if(!fout.is_open()) {
        cerr << "ERROR: cannot open result file: " << path << endl;
        cerr << "       Check that the directory exists and is writable." << endl;
        abort();
    }

    if(write_header) {
        fout << "method,dataset,delta,avg_time_ms,avg_candidates,"
                "construction_time_min,index_size_mb\n";
    }
    for(const auto& r : rows) {
        fout << r.method               << ","
             << r.dataset              << ","
             << r.delta                << ","
             << r.avg_time_ms          << ","
             << r.avg_candidates       << ","
             << r.construction_time_min << ","
             << r.index_size_mb        << "\n";
    }
    fout.flush();
    fout.close();
    cout << "Results written to: " << path << endl;
}


>>>>>>> a67531c (chore: add per-dataset results)
// Per-method runners
vector<ResultRow> run_les3(const string& dataset,
                           const string& path_to_sets,
                           const string& path_to_groups,
                           const vector<float>& ds,
                           vector<multiset<int>>& shared_query_sets) {
    vector<ResultRow> rows;

    LES3 les3(path_to_sets, path_to_groups);
<<<<<<< HEAD
    float    size_mb   = les3.get_size_in_MB();
    // Round to 2 decimal places
    size_mb = round(size_mb * 100.0f) / 100.0f;
    double build_min = les3.construction_time_minutes;
    cout << "LES3 size: " << size_mb << " MB" << endl;

    for(float d : ds) {
        cout << "\n[LES3] delta=" << d << endl;
        auto result = les3.testDeltaNN(d, shared_query_sets);
        double avg_ms = result.first;
        long long avg_cands = result.second;
        rows.push_back({"LES3", dataset, d, avg_ms, avg_cands, build_min, size_mb});
    }
    return rows;
}

vector<ResultRow> run_dualtrans(const string& dataset,
                                const string& path_to_sets,
                                const vector<float>& ds,
                                vector<multiset<int>>& shared_query_sets) {
    vector<ResultRow> rows;

    DualTrans dt(path_to_sets);
    float  size_mb = dt.get_size_in_MB();
    size_mb = round(size_mb * 100.0f) / 100.0f;
    double build_min = dt.construction_time_minutes;
    cout << "DualTrans size: " << size_mb << " MB" << endl;

    for(float d : ds) {
        cout << "\n[DualTrans] delta=" << d << endl;
        auto [avg_ms, avg_cands] = dt.testDeltaNN(d, shared_query_sets);
        rows.push_back({"DualTrans", dataset, d, avg_ms, avg_cands, build_min, size_mb});
    }
    return rows;
}

// Main
int main(int argc, const char* argv[]) {

    string dataset        = "kosarak";
    string path_to_sets   = "../datasets/kosarak/all.dat";
    string path_to_groups = "../datasets/kosarak/LES3";
    string result_path    = "./results/kosarak_results.csv";

    if(argc >= 5) {
        dataset        = argv[1];
        path_to_sets   = argv[2];
        path_to_groups = argv[3];
        result_path    = argv[4];  
    } else {
        cout << "Usage: " << argv[0]
             << " <dataset> <path_to_sets> <path_to_groups> <output_csv>\n"
             << "Falling back to defaults (kosarak).\n\n";
    }

    cout << "Output CSV: " << result_path << endl;

    vector<float> ds = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};

    cout << "Sampling 1000 query sets from: " << path_to_sets << endl;
    vector<multiset<int>> shared_query_sets = loadAndSampleSets(path_to_sets, 1000, 42);
    cout << "Sampled " << shared_query_sets.size() << " query sets" << endl;
=======
    int    size_mb   = les3.get_size_in_MB();
    double build_min = les3.construction_time_minutes;
    cout << "LES3 size: " << size_mb << " MB" << endl;

    if(shared_query_sets.empty()) {
        shared_query_sets = les3.sampleQuerySets(1000);
        cout << "Sampled " << shared_query_sets.size() << " query sets" << endl;
    }

    for(float d : ds) {
        cout << "\n[LES3] delta=" << d << endl;
        auto result = les3.testDeltaNN(d, shared_query_sets);
        double avg_ms = result.first;
        long long avg_cands = result.second;
        ResultRow row;
        row.method = "LES3";
        row.dataset = dataset;
        row.delta = d;
        row.avg_time_ms = avg_ms;
        row.avg_candidates = avg_cands;
        row.construction_time_min = build_min;
        row.index_size_mb = static_cast<float>(size_mb);
        rows.push_back(row);
    }
    return rows;
}

vector<ResultRow> run_dualtrans(const string& dataset,
                                const string& path_to_sets,
                                const vector<float>& ds,
                                vector<multiset<int>>& shared_query_sets) {
    vector<ResultRow> rows;

    DualTrans dt(path_to_sets);
    float  size_mb = dt.get_size_in_MB();
    size_mb = round(size_mb * 100.0f) / 100.0f;
    double build_min = dt.construction_time_minutes;
    cout << "DualTrans size: " << size_mb << " MB" << endl;

    for(float d : ds) {
        cout << "\n[DualTrans] delta=" << d << endl;
        auto [avg_ms, avg_cands] = dt.testDeltaNN(d, shared_query_sets);
        rows.push_back({"DualTrans", dataset, d, avg_ms, avg_cands, build_min, size_mb});
    }
    return rows;
}

// Main
int main(int argc, const char* argv[]) {

    string dataset        = "kosarak";
    string path_to_sets   = "../datasets/kosarak/all.dat";
    string path_to_groups = "../datasets/kosarak/LES3";
    string result_path    = "./results/kosarak_results.csv";

    if(argc >= 5) {
        dataset        = argv[1];
        path_to_sets   = argv[2];
        path_to_groups = argv[3];
        result_path    = argv[4];  
    } else {
        cout << "Usage: " << argv[0]
             << " <dataset> <path_to_sets> <path_to_groups> <output_csv>\n"
             << "Falling back to defaults (kosarak).\n\n";
    }

    cout << "Output CSV: " << result_path << endl;

    vector<float> ds = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    vector<multiset<int>> shared_query_sets;
>>>>>>> a67531c (chore: add per-dataset results)

    // Run LES3
    cout << "\n========== LES3 | " << dataset << " ==========\n";
    auto les3_rows = run_les3(dataset, path_to_sets, path_to_groups,
                              ds, shared_query_sets);
    writeCSV(result_path, les3_rows, /*append=*/false);

    // Run DualTrans
    cout << "\n========== DualTrans | " << dataset << " ==========\n";
    auto dt_rows = run_dualtrans(dataset, path_to_sets, ds, shared_query_sets);
    writeCSV(result_path, dt_rows, /*append=*/true);

    cout << "\nAll results saved to: " << result_path << endl;
    return 0;
}