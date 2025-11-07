#include "boost/heap/pairing_heap.hpp"
#include "boost/heap/priority_queue.hpp"

#include"problem/heuristic.h"

template<int N>
Heuristic<N>::Heuristic(size_t source, AdjacencyMatrix &adj_matrix, bool reverse)
: data(adj_matrix.get_num_node()) {
    for (auto & entry: data){ entry.fill(MAX_COST); }

    for (int i = 0; i < N; i++){
        dijkstra_ota(source, i, adj_matrix, reverse);
    }
}


template<int N>
void Heuristic<N>::dijkstra_ota(size_t source, size_t cost_idx, AdjacencyMatrix &adj_matrix, bool reverse) {
    struct Node{
        size_t idx;
        cost_t value;
        Node() {}
        Node(size_t idx, cost_t value) : idx(idx), value(value) {}

        struct compare_node {
            // returns true if n1 > n2 (note -- this gives us *min*-heap).
            bool operator()(const Node& n1, const Node& n2) const {
                return n1.value >= n2.value;
            }
        };
    };

    data[source][cost_idx] = 0;
    Node root(source, 0);
    boost::heap::pairing_heap<Node, boost::heap::compare<typename Node::compare_node>> heap;

    heap.push(root);  // add root to heap
    while (!heap.empty()){
        Node curr = heap.top();
        heap.pop();

        if (data[curr.idx][cost_idx] < curr.value){
            continue;
        }

        const std::vector<Edge> &outgoing_edges = adj_matrix[curr.idx];
        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {
            size_t next_state = p_edge->tail;

            cost_t next_cost = reverse ?
                curr.value + p_edge->cost[N - cost_idx - 1]
                : curr.value + p_edge->cost[cost_idx];
            if (data[next_state][cost_idx] > next_cost){
                data[next_state][cost_idx] = next_cost;
                heap.push({next_state, next_cost});
            }
        }
    }
}

template class Heuristic<2>;
template class Heuristic<3>;
template class Heuristic<4>;
template class Heuristic<5>;