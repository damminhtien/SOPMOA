#ifndef ALGORITHM_ABSTRACT_SOLVER
#define ALGORITHM_ABSTRACT_SOLVER

#include"../common_inc.h"
#include"../problem/graph.h"
#include"../problem/heuristic.h"
#include"../utils/cost.h"
#include"../utils/label.h"

struct Solution {
    std::vector<cost_t> cost;
    double time_found;

    Solution(std::vector<cost_t> _cost = {}, double time=-1)
    : cost(_cost), time_found(time) {}
};

class AbstractSolver {
public:
    std::vector<Solution> solutions;
    
    AbstractSolver(AdjacencyMatrix &adj_matrix, size_t start_node, size_t target_node)
    : graph(adj_matrix), start_node(start_node), target_node(target_node) {
        solutions.clear();
    }
    virtual ~AbstractSolver(){}

    virtual std::string get_name() {return "AbstractSolver"; }

    size_t get_num_expansion(){ return num_expansion; }
    size_t get_num_generation(){ return num_generation; }

    virtual void solve(unsigned int time_limit = UINT_MAX){
        perror("Solver not implemented");
        exit(EXIT_FAILURE);
        return; 
    };

    std::string get_result_str(){
        std::stringstream ss;
        ss << get_name() << ","
        << start_node << "," << target_node << "," 
        << get_num_generation() << "," 
        << get_num_expansion() << "," 
        << solutions.size(); 
        std::string result = ss.str();
        return result;
    }

    std::string get_all_sols_str() {
        std::stringstream ss;
        for(auto sol : solutions) {
            ss << "[" << sol.cost[0];
            for (int i = 1; i < sol.cost.size(); i++) {
                ss << "," << sol.cost[i];
            }
            ss << "]" << std::endl;
        }
        std::string result = ss.str();
        return result;
    }

protected:
    AdjacencyMatrix & graph;

    size_t num_expansion = 0;
    size_t num_generation = 0;

    size_t start_node;
    size_t target_node;
};

#endif