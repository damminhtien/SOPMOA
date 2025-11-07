#ifndef ALGORITHMS_GCL_ARRAY
#define ALGORITHMS_GCL_ARRAY

#include"../../common_inc.h"
#include"../../utils/cost.h"

const int ARRAY_SIZE=50000;

template<int N>
class Gcl_array {
public:
    Gcl_array(size_t num_node) : gcl(num_node + 1) {
        for (int i = 0; i < num_node; i++) {
            gcl[i].reserve(ARRAY_SIZE);
        }
    };
    std::string get_name(){ return "array"; }
    
    inline bool frontier_check(size_t node, const CostVec<N> & cost) {
        if (node >= gcl.size()) { return false; }

        for (auto & vec : gcl[node]) {
            if (weakly_dominate<N>(vec, cost)) { return true; }
        }
        return false;
    }

    inline void frontier_update(size_t node, const CostVec<N> & cost) {
        if (node >= gcl.size()) { 
            perror(("Invalid Gcl index " + std::to_string(node)).c_str());
            exit(EXIT_FAILURE);
            return; 
        }

        gcl[node].push_back(cost);
        for (int i = gcl[node].size() - 2; i >= 0; i--) {
            if (weakly_dominate<N>(cost, gcl[node][i])) {
                gcl[node][i] = gcl[node].back();
                gcl[node].pop_back();
            }
        }
    }

private:
    std::vector<std::vector<CostVec<N>>> gcl;
};

template class Gcl_array<1>;
template class Gcl_array<2>;
template class Gcl_array<3>;
template class Gcl_array<4>;
template class Gcl_array<5>;

#endif