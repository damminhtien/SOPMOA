#ifndef ALGORITHMS_GCL
#define ALGORITHMS_GCL

#include"../../common_inc.h"
#include"../../utils/cost.h"

template<int N>
class Gcl {
public:
    Gcl(size_t num_node) : gcl(num_node + 1) {};
    std::string get_name(){ return "list"; }
    
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

        auto it = gcl[node].begin();
        while (it != gcl[node].end()) {
            if (weakly_dominate<N>(cost, *it)) {
                it = gcl[node].erase(it);
            } else { it++; }
        }

        gcl[node].push_front(cost);
    }

private:
    std::vector<std::list<CostVec<N>>> gcl;
};

template class Gcl<1>;
template class Gcl<2>;
template class Gcl<3>;
template class Gcl<4>;
template class Gcl<5>;

#endif