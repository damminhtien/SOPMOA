#ifndef UTILS_LABEL
#define UTILS_LABEL

#include"../common_inc.h"
#include"cost.h"

template <int N> struct Label;
template <int N> using LabelPtr = Label<N> *;

template <int N>
struct Label {
    static const int num_obj = N;
    size_t node;
    CostVec<N> f;
    Label * parent = nullptr;
    bool should_check_sol = false;

    Label() {}
    Label(size_t node, CostVec<N> f): node(node), f(f) {}
    Label(size_t node, CostVec<N> g, CostVec<N> & h) : node(node) {
        f = g;
        for (int i = 0; i < N; i++) { f[i] += h[i]; }
    }

    Label * next_ = nullptr;
    inline Label* get_next() { return next_; }
    inline void set_next(Label* next) { next_ = next; }

    struct lex_larger_for_PQ {
        // HERE
        bool operator()(const LabelPtr<N> &a, const LabelPtr<N> &b) const {
            for (int i = 0; i < N - 1; i++) {
                if (a->f[i] != b->f[i]) { return (a->f[i] > b->f[i]); }
            }
            return (a->f.back() > b->f.back());
        };
    };

    struct lex_larger_tr_for_PQ {
        // HERE
        inline bool operator()(const LabelPtr<N> &a, const LabelPtr<N> &b) const {
            for (int i = 1; i < N - 1; i++) {
                if (a->f[i] != b->f[i]) { return (a->f[i] > b->f[i]); }
            }
            return (a->f.back() > b->f.back());
        };
    };
};

#endif