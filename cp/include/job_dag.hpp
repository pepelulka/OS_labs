#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "nlohmann/json.hpp"

namespace cp {

class TDagJobExecutor;

struct TJob {
    std::string name, path;
};

class TJobDag {
private:
    using TMapStringToStrings = std::map<std::string, std::vector<std::string>>;

    // Common rule :
    // if the pair represents dependency then first element is what REQUIRED and the second one is TARGET
    // Map of jobs
    std::map<std::string, TJob> jobs;

    // Map of dependencies. dep["name"] gives us vector of jobs REQUIRED to do job "name"
    TMapStringToStrings dep;

    // Inversed dep. dep^-1
    TMapStringToStrings rdep;

    std::vector<std::string> mutexes;
    // Map of mutexes, each job own : mutOwn[j] is vector of mutexes, which j own
    TMapStringToStrings mutOwn;

    // 0 - not-visited, 1 - in current route, 2 - visited and not in current route
    // returns true if have loops, false - otherwise
    static bool dfs(const std::string &v,
                    std::map<std::string, int>& visited,
                    TMapStringToStrings& dep);

    static bool CheckCorectness(TJobDag &dag);

    static TMapStringToStrings inverse(TMapStringToStrings &map) {
        TMapStringToStrings result;
        for (auto p : map) {
            for (auto target : p.second) {
                result[target].push_back(p.first);
            }
        }
        return result;
    }

public:

    friend class TDagJobExecutor;

    TJobDag() = default;
    TJobDag(const std::vector<TJob>& jobs,
            const std::vector<std::pair<std::string, std::string>>& deps,
            const std::vector<std::string>& _mutexes = {},
            const TMapStringToStrings& _mutOwn = {}) {
        TJobDag temp;
        for (auto i : jobs) {
            temp.jobs[i.name] = i;
        }
        for (auto p : deps) {
            temp.dep[p.second].push_back(p.first);
        }
        if (CheckCorectness(temp)) {
            *this = temp;
        } else {
            throw std::logic_error("Bad DAG");
        }
        mutexes = _mutexes;
        mutOwn = _mutOwn;
        rdep = inverse(dep);
    }

    void print() {
        std::cout << "JOB DAG : ============\n";
        for (auto p : jobs) {
            std::cout << "JOB{" << p.second.name << ", " << p.second.path << "} ";
        }
        std::cout << std::endl;
        for (auto p : dep) {
            std::cout << p.first << " <- ";
            for (auto to : p.second) {
                std::cout << to << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "======================\n";
    }

};

/*

    Example of CORRECT json file:
    (*mutexes is optional)

{
    "path_to_bins": "/home/meow/bin/",
    "jobs": [
        {
            "name": "job1",
            "path": "bin/job1"
        },
        {
            "name": "job2",
            "path": "bin/job2"
        }
    ]
    "dependencies": [
        {
            "required": "job1",
            "target": "job2"
        }
    ]
    "mutexes": [
        {
            "name": "mutex1",
            "who_owns": ["job1", "job2"]
        }
    ]

}

*/
class JSONParser {
public:

    static TJobDag parse(const std::string &pathToFile);

};

} // namespace cp
