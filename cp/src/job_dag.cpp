#include "job_dag.hpp"

namespace cp {

bool TJobDag::Dfs(const std::string &v,
                  std::map<std::string, int>& visited,
                  TMapStringToStrings& dep) {
    visited[v] = 1;
    for (const auto& to : dep[v]) {
        if (visited[to] == 1) {
            return true;
        } else if (visited[to] == 0) {
            bool result = Dfs(to, visited, dep);
            if (result) {
                return result;
            }
        }
    }
    visited[v] = 2;
    return false;
}

bool TJobDag::CheckCorrectness(TJobDag &dag) {
    // First step (map keys is job.name)
    for (const auto& p : dag.jobs) {
        const auto& key = p.first;
        const auto& job = p.second;
        if (key != job.name) {
            return false;
        }
    }
    // Second step
    for (const auto& p : dag.dep) {
        if (dag.jobs.find(p.first) == dag.jobs.end()) {
            return false;
        }
        for (const auto& i : p.second) {
            if (dag.jobs.find(i) == dag.jobs.end()) {
                return false;
            }
        }
    }
    // Third step (Absence of loops)
    std::map<std::string, std::vector<std::string>> dep = dag.dep;
    std::map<std::string, int> visited;
    for (const auto& p : dep) {
        visited[p.first] = 0;
    }
    for (const auto& p : dep) {
        if (visited[p.first] == 0) {
            if (Dfs(p.first, visited, dep)) {
                return false;
            }
        }
    }
    return true;
}

TJobDag::TMapStringToStrings TJobDag::Inverse(TMapStringToStrings &map) {
    TMapStringToStrings result;
    for (const auto& p : map) {
        for (const auto& target : p.second) {
            result[target].push_back(p.first);
        }
    }
    return result;
}

TJobDag::TJobDag(const std::vector<TJob>& jobs,
                 const std::vector<std::pair<std::string, std::string>>& deps,
                 const std::vector<std::string>& _mutexes,
                 const TMapStringToStrings& _mutOwn) {
    for (const auto& i : jobs) {
        this->jobs[i.name] = i;
    }
    for (const auto& p : deps) {
        dep[p.second].push_back(p.first);
    }
    if (!CheckCorrectness(*this)) {
        throw std::logic_error("Bad DAG");
    }
    mutexes = _mutexes;
    mutOwn = _mutOwn;
    rdep = Inverse(dep);
}

TJobDag JSONParser::Parse(const std::string &pathToFile) {
        std::ifstream f(pathToFile);
        nlohmann::json jsn = nlohmann::json::parse(f);

        std::string path_to_bins = jsn["path_to_bins"];

        std::vector<TJob> jobs;
        for (const auto& job : jsn["jobs"]) {
            std::string path = path_to_bins + std::string(job["path"]);
            jobs.push_back({job["name"], path});
        }

        std::vector<std::pair<std::string, std::string> > deps;
        for (const auto& dep : jsn["dependencies"]) {
            deps.push_back({dep["required"], dep["target"]});
        }

        std::vector<std::string> muts;
        std::map<std::string, std::vector<std::string>> mutOwn;
        if (jsn.contains("mutexes")) {
            for (const auto& mut : jsn["mutexes"]) {
                muts.push_back(mut["name"]);
                for (const auto& owner : mut["who_owns"]) {
                    mutOwn[owner].push_back(mut["name"]);
                }
            }
        }

        return TJobDag(jobs, deps, muts, mutOwn);
    }

}
