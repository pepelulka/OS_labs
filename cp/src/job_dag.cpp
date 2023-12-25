#include "job_dag.hpp"

namespace cp {

bool TJobDag::dfs(const std::string &v,
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

bool TJobDag::CheckCorectness(TJobDag &dag) {
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

TJobDag JSONParser::parse(const std::string &pathToFile) {
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

        std::vector<std::string> muts;
        std::map<std::string, std::vector<std::string>> mutOwn;
        if (jsn.contains("mutexes")) {
            for (auto mut : jsn["mutexes"]) {
                muts.push_back(mut["name"]);
                for (auto owner : mut["who_owns"]) {
                    mutOwn[owner].push_back(mut["name"]);
                }
            }
        }

        return TJobDag(jobs, deps, muts, mutOwn);
    }

}
