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

    // 0 - not-visited, 1 - in current route, 2 - visited and not in current route
    // returns true if have loops, false - otherwise
    static bool dfs(const std::string &v,
                    std::map<std::string, int>& visited,
                    TMapStringToStrings& dep) {
        visited[v] = 1;
        for (const auto& to : dep[v]) {
            if (visited[to] == 1) {
                return true;
            } else if (visited[to] == 0) {
                bool result = dfs(to, visited, dep);
                if (result) {
                    return result;
                }
            }
        }
        visited[v] = 2;
        return false;
    }

    static bool CheckCorectness(TJobDag &dag) {
        // First step (map keys is job.name)
        for (auto p : dag.jobs) {
            auto key = p.first;
            auto job = p.second;
            if (key != job.name) {
                return false;
            }
        }
        // Second step
        for (auto p : dag.dep) {
            if (dag.jobs.find(p.first) == dag.jobs.end()) {
                return false;
            }
            for (auto i : p.second) {
                if (dag.jobs.find(i) == dag.jobs.end()) {
                    return false;
                }
            }
        }
        // Third step (Absence of loops)
        std::map<std::string, std::vector<std::string>> dep = dag.dep;
        std::map<std::string, int> visited;
        for (auto p : dep) {
            visited[p.first] = 0;
        }
        for (auto p : dep) {
            if (visited[p.first] == 0) {
                if (dfs(p.first, visited, dep)) {
                    return false;
                }
            }
        }
        return true;
    }

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
            const std::vector<std::pair<std::string, std::string>>& deps) {
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
}

*/
class JSONParser {
public:

    friend class TJobDag;

    static TJobDag parse(const std::string &pathToFile) {
        std::ifstream f(pathToFile);
        nlohmann::json jsn = nlohmann::json::parse(f);

        std::string path_to_bins = jsn["path_to_bins"];

        std::vector<TJob> jobs;
        for (auto job : jsn["jobs"]) {
            std::string path = path_to_bins + std::string(job["path"]);
            jobs.push_back({job["name"], path});
        }

        std::vector<std::pair<std::string, std::string> > deps;
        for (auto dep : jsn["dependencies"]) {
            deps.push_back({dep["required"], dep["target"]});
        }

        return TJobDag(jobs, deps);
    }

};

} // namespace cp
