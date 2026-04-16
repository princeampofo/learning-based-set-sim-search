//
//  LES3.h
//  LES3
//

#ifndef LES3_h
#define LES3_h

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <sstream>
#include <filesystem>
#include "MEASURE.h"
#include "TGM_C.h"
using namespace std;
#include <random>

struct LES3 {
    int num_groups;
    TGM_C tgm;
    Measure measure;
    int total_num_sets;
    vector<vector<multiset<int>>> grouped_database;
    vector<multiset<int>> database;
    vector<vector<int>> groups;
    double construction_time_minutes = 0.0;

    LES3() = default;
    LES3(string path_to_sets, string path_to_groups) {
        auto build_start = chrono::steady_clock::now();
        preprocess(path_to_sets, path_to_groups);
        tgm.construct(path_to_sets, path_to_groups);
        auto build_end = chrono::steady_clock::now();
        construction_time_minutes = chrono::duration<double, std::ratio<60>>(build_end - build_start).count();
        cout << "LES3 construction time: " << construction_time_minutes << " minutes" << endl;
    };

    int findKNN(multiset<int> &query_set, int k);
    int findDeltaNN(multiset<int> &query_set, float delta);
    void preprocess(string path_to_sets, string path_to_groups);
    void insert_helper(multiset<int> &set_to_insert);
    float get_size_in_MB();
    void insert(string path, float insert_ratio);
    double testKNN(int result_size);

    // Returns {avg_time_ms, avg_candidates_checked}
    pair<double, long long> testDeltaNN(float delta, vector<multiset<int>>& query_sets);

    vector<multiset<int>> sampleQuerySets(int num_queries) {
        vector<multiset<int>> query_sets;
        vector<multiset<int>*> all_sets;
        for(auto &group : grouped_database) {
            for(auto &s : group) {
                all_sets.push_back(&s);
            }
        }
        srand(42);
        vector<int> indices(all_sets.size());
        iota(indices.begin(), indices.end(), 0);
        shuffle(indices.begin(), indices.end(), default_random_engine(42));
        for(int i = 0; i < num_queries && i < (int)indices.size(); i++) {
            query_sets.push_back(*all_sets[indices[i]]);
        }
        return query_sets;
    }
};

void LES3::preprocess(string path_to_sets, string path_to_groups) {
    ifstream in(path_to_sets);
    string line;
    while(getline(in, line)) {
        istringstream line_stream(line);
        string temp;
        multiset<int> temp_set;
        while(line_stream>>temp) {
            temp_set.insert(stoi(temp));
        }
        database.push_back(temp_set);
    }
    in.close();
    total_num_sets = database.size();

    in.open(path_to_groups);
    int group_id = 0;
    while(getline(in, line)) {
        vector<int> current_group;
        istringstream line_stream(line);
        string temp;
        while(line_stream>>temp) {
            int set_id = stoi(temp);
            current_group.push_back(set_id);
        }
        groups.push_back(current_group);
        group_id++;
    }

    for(int i = 0; i < (int)groups.size(); i++) {
        vector<multiset<int>> this_group;
        for(int set_id : groups[i]) {
            this_group.push_back(database[set_id]);
        }
        grouped_database.push_back(this_group);
    }
    in.close();

    num_groups = groups.size();
    cout << "num groups: " << num_groups << endl;
}

float LES3::get_size_in_MB() {
    return tgm.get_size();
}

int LES3::findKNN(multiset<int> &query_set, int k) {
    int check_counts = 0;
    priority_queue<pair<float, int>> candidate_groups;
    for(unsigned j = 0; j < (unsigned)num_groups; j++) {
        float ub = tgm.getUB(query_set, j);
        candidate_groups.push({ub, j});
    }

    priority_queue<pair<float, multiset<int>>, vector<pair<float, multiset<int>>>,
        std::greater<pair<float, multiset<int>>>> result;

    while(!candidate_groups.empty()) {
        if(result.size() == (size_t)k && result.top().first >= candidate_groups.top().first)
            break;
        for(auto &candidate_set : grouped_database[candidate_groups.top().second]) {
            check_counts++;
            float sim = measure.computeSim(query_set, candidate_set);
            if(result.size() >= (size_t)k && sim < result.top().first)
                continue;
            result.push({sim, candidate_set});
            if(result.size() > (size_t)k)
                result.pop();
        }
        candidate_groups.pop();
    }
    return check_counts;
}

int LES3::findDeltaNN(multiset<int> &query_set, float delta) {
    int check_counts = 0;
    priority_queue<pair<float, int>> candidate_groups;
    for(unsigned j = 0; j < (unsigned)num_groups; j++) {
        float ub = tgm.getUB(query_set, j);
        candidate_groups.push({ub, j});
    }
    vector<multiset<int>> result;
    while(!candidate_groups.empty() && candidate_groups.top().first >= delta) {
        for(auto &candidate_set : grouped_database[candidate_groups.top().second]) {
            check_counts++;
            float sim = measure.computeSim(query_set, candidate_set);
            if(sim >= delta)
                result.push_back(candidate_set);
        }
        candidate_groups.pop();
    }
    return check_counts;
}

double LES3::testKNN(int result_size) {
    float query_ratio = 1000.0 / total_num_sets;
    int query_size = 0;
    unsigned total_checked_count = 0;

    auto start = chrono::system_clock::now();
    for(int i = 0; i < (int)groups.size(); i++) {
        for(auto &query_set : grouped_database[i]) {
            float rand_v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if(rand_v > query_ratio) continue;
            query_size++;
            total_checked_count += findKNN(query_set, result_size);
        }
    }
    auto end = chrono::system_clock::now();
    double total_time = (end - start).count();
    double avg_time_milliseconds = total_time / query_size / 1000;
    cout << "average search time: " << avg_time_milliseconds << endl;
    return avg_time_milliseconds;
}

// Returns {avg_time_ms, avg_candidates_checked}
pair<double, long long> LES3::testDeltaNN(float delta, vector<multiset<int>>& query_sets) {
    int query_size = query_sets.size();
    long long total_checked_count = 0;

    auto start = chrono::steady_clock::now();
    for(auto &query_set : query_sets) {
        total_checked_count += findDeltaNN(query_set, delta);
    }
    auto end = chrono::steady_clock::now();

    double total_time_milliseconds = chrono::duration<double, std::milli>(end - start).count();
    double avg_time_milliseconds = total_time_milliseconds / query_size;
    long long avg_checked_count = total_checked_count / query_size;

    cout << "LES3 avg candidates checked: " << avg_checked_count << endl;
    cout << "LES3 average search time: " << avg_time_milliseconds << " ms" << endl;

    return {avg_time_milliseconds, avg_checked_count};
}

#endif /* LES3_h */