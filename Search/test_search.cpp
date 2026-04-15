//
//  main.cpp
//  LES3
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
#include <filesystem>
#include "LES3.h"
#include "BRUTEFORCE.h"
#include "DualTrans.h"
#include "PARAM.h"
#include "InvertedIndex.h"
using namespace std;

// void check_les3(string path_to_sets, string path_to_groups, vector<int> ks, vector<float> ds) {
//     LES3 les3(path_to_sets, path_to_groups);
//     cout<<"LES3"<<endl;
//     cout<<"LES3 size: "<<les3.get_size_in_MB()<<endl;
//     // for(int k : ks) {
//     //     cout<<k<<endl;
//     //     les3.testKNN(k);
//     // }
//     // sample 1000 query sets once, reuse for all delta values
//     vector<multiset<int>> query_sets = les3.sampleQuerySets(1000);
//     cout << "Query set size: " << query_sets.size() << endl;
    
//     for(float d : ds) {
//         cout << "delta=" << d << endl;
//         les3.testDeltaNN(d, query_sets);
//     }
// }

// void check_dt(string path_to_sets, vector<int> ks, vector<float> ds, vector<multiset<int>>& query_sets) {
//     DualTrans dt(path_to_sets);
//     cout<<"DualTrans"<<endl;
//     cout<<"DT size: "<<dt.get_size_in_MB()<<endl;
//     // for(int k : ks) {
//     //     cout<<k<<endl;
//     //     dt.testKNN(k);
//     // }
//     for(float d : ds) {
//         cout<<d<<endl;
//         dt.testDeltaNN(d, query_sets);
//     }
// }

void check_ii(string path_to_sets, vector<int> ks, vector<float> ds) {
    InvertedIndex invidx(path_to_sets);
    
    cout<<"Inverted index"<<endl;
    cout<<"invidx size: "<<invidx.get_size_in_MB()<<endl;
    for(int k : ks) {
        cout<<k<<endl;
        invidx.testKNN(k);
    }
    for(float d : ds) {
        cout<<d<<endl;
        invidx.testDeltaNN(d);
    }
}

void check_bf(string path_to_sets, vector<int> ks, vector<float> ds) {
    cout<<"Brute force"<<endl;
    BRUTEFORCE bf(path_to_sets);
    bf.testKNN(2);
}

int main(int argc, const char * argv[]) {
    
    string path_to_sets = "../datasets/kosarak/all.dat";
    
    string path_to_groups = "../datasets/kosarak/LES3";
    
    vector<float> ds({0.9, 0.8, 0.7, 0.6, 0.5});
    
    // run LES3 first and get query sets
    LES3 les3(path_to_sets, path_to_groups);
    cout << "LES3 size: " << les3.get_size_in_MB() << " MB" << endl;
    vector<multiset<int>> query_sets = les3.sampleQuerySets(1000);
    cout << "Query set size: " << query_sets.size() << endl;
    for(float d : ds) {
        cout << "delta=" << d << endl;
        les3.testDeltaNN(d, query_sets);
    }
    
    // reuse same query sets for DualTrans
    DualTrans dt(path_to_sets);
    cout << "DualTrans size: " << dt.get_size_in_MB() << " MB" << endl;
    for(float d : ds) {
        cout << "delta=" << d << endl;
        dt.testDeltaNN(d, query_sets);
    }

}