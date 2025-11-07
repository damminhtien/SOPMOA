#ifndef ALGORITHMS_GCL_SOPMOA
#define ALGORITHMS_GCL_SOPMOA

#include"../../common_inc.h"
#include"../../utils/cost.h"

#include <shared_mutex>

template <int N>
class Gcl_SOPMOA {
public:
    Gcl_SOPMOA(size_t num_node) {
        for (size_t i = 0; i <= num_node; i++) {
            gcl.push_back(std::list<CostVec<N>*>{});
            locks.push_back(new std::shared_mutex);
        }
    };
    ~Gcl_SOPMOA() {
        for (auto cost : all_costs) { delete cost; }
        for (auto lock : locks) { delete lock; }
        gcl.clear();
    }
    std::string get_name(){ return "custom_list"; }

    inline bool frontier_check(size_t node, const CostVec<N> & cost) {
        if (node >= gcl.size()) { return false; }
        
        locks[node]->lock_shared();
        auto temp_gcl = gcl[node];
        locks[node]->unlock_shared();

        auto it_ub = std::lower_bound(temp_gcl.rbegin(), temp_gcl.rend(), 
            cost, [](CostVec<N>* ele, CostVec<N> vec) { 
                return lex_smaller<N>(vec, *ele);
            }
        ).base();

        for (auto it = temp_gcl.begin(); it != it_ub; it++) {
            if (weakly_dominate_dr<N>(*(*it), cost)){ return true; }
        }
        return false;
    }

    inline void frontier_update(size_t node, const CostVec<N> & cost) {
        if (node >= gcl.size()) { 
            perror(("Invalid Gcl index " + std::to_string(node)).c_str());
            exit(EXIT_FAILURE);
            return; 
        }
    
        locks[node]->lock();

        auto it = gcl[node].begin();
        while (it != gcl[node].end()) {
            if (weakly_dominate_dr<N>(cost, *(*it))){
                it = gcl[node].erase(it); 
            } else { it++; }
        }

        auto it_insert = std::find_if(gcl[node].rbegin(), gcl[node].rend(), 
            [&cost](CostVec<N>* ele) { 
                return lex_smaller<N>(*ele, cost);
            }
        ).base();

        auto new_cost = new CostVec<N>(cost);
        all_costs.push_back(new_cost);
        gcl[node].insert(it_insert, new_cost);

        locks[node]->unlock();
    }

private:
    std::list<CostVec<N>*> all_costs;
    std::vector<std::list<CostVec<N>*>> gcl;
    std::vector<std::shared_mutex*> locks;
};

template class Gcl_SOPMOA<2>;
template class Gcl_SOPMOA<3>;
template class Gcl_SOPMOA<4>;
template class Gcl_SOPMOA<5>;

#endif