#include <iostream>
#include <sstream>
#include <mutex>
#include <string>
#include <thread>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <stack>
#include <ctime>
using namespace std;

#define MAXN 12500
const double eps = 1e-8;
/////////////GRAPH////////////////
#ifndef GRAPH_H
#define GRAPH_H
class Graph {
public:
    Graph() {
        for (int i = 0; i <= MAXN; i ++) {
            vertices_.insert(i);
            graph_.push_back(vector<int>());
            has_edges_.push_back(false);
        }
    }

    void add_edge(int from, int to) {
        graph_[from].push_back(to);
        has_edges_[from] = has_edges_[to] = true;
    }

    const vector<int>& get_neighbors(int vertex) const {
        return graph_[vertex];
    }

    bool has_edges(int vertex) const {
        return has_edges_[vertex];
    }
private:
    unordered_set<int> vertices_;
    vector<vector<int>> graph_;
    vector<bool> has_edges_;
};

#endif


//////////////PARSE/////////////
#ifndef PARSE_H
#define PARSE_H

namespace IO_opt {
    const int BUF = 65536;
    char buf[BUF + 1];
    char *head = buf, *tail = buf;

    inline char inputchar()
    {
        if(head == tail)
            *(tail = (head = buf) + fread(buf, 1, BUF, stdin)) = 0;
        return *head++;
    }
    template <typename T> inline void read(T &x, T &y) {
        char ch = inputchar(); int f = 1; x = 0;
        while(ch < '0' || ch > '9') {if(ch == '-') f = -1; ch = inputchar();}
        while(ch >= '0' && ch <= '9') x = x*10+ch-'0', ch = inputchar();
        x *= f;

        ch = inputchar(); f = 1; y = 0;
        while(ch < '0' || ch > '9') {if(ch == '-') f = -1; ch = inputchar();}
        while(ch >= '0' && ch <= '9') y = y*10+ch-'0', ch = inputchar();
        y *= f;
    }

    #define I_int int
    inline void write(I_int x) {
        if(x<0) putchar('-'),x=-x;
        if(x>9) write(x/10);
        putchar(x%10+'0');
    }
    #undef I_int
} using namespace IO_opt;

Graph graph;

class Parser {
public:
    Parser() {
        int u, v;
        while(inputchar() != ']') {
            read(u, v);
            graph.add_edge(u, v);
        }
    }
};
#endif

////////////?DC////////////////
#ifndef DEPENDENCY_CALCULATOR_H
#define DEPENDENCY_CALCULATOR_H

class DependencyCalculator {
public:
    DependencyCalculator(int vertex) : vertex_(vertex) {
        init_();
        find_shortest_paths_();
        calculate_dependencies_();
    }

    double get_dependency(int vertex) const {
        return dependency_[vertex];
    }
private:
    int vertex_; // s

    stack<int> stack_; // S
    vector<vector<int> > shortest_path_predecessors_; // P
    vector<int> shortest_paths_; // sigma
    vector<int> distance_; // d
    queue<int> queue_; // Q
    vector<double> dependency_; // delta

    void init_() {
        for (int vertex = 0; vertex <= MAXN; vertex++) {
            shortest_path_predecessors_.emplace_back();
            shortest_paths_.push_back(0);
            distance_.push_back(-1);
            dependency_.push_back(0);
        }

        shortest_paths_[vertex_] = 1;
        distance_[vertex_] = 0;
        queue_.push(vertex_);
    }

    void find_shortest_paths_() {
        while (!queue_.empty()) {
            int vertex = queue_.front();
            queue_.pop();
            stack_.push(vertex);

            for (int neighbor : graph.get_neighbors(vertex)) {
                if (distance_[neighbor] < 0) {
                    queue_.push(neighbor);
                    distance_[neighbor] = distance_[vertex] + 1;
                }

                if (distance_[neighbor] == distance_[vertex] + 1) {
                    shortest_paths_[neighbor] += shortest_paths_[vertex];
                    shortest_path_predecessors_[neighbor].push_back(vertex);
                }
            }
        }
    }

    void calculate_dependencies_() {
        double shortest_path_ratio;
        while (!stack_.empty()) {
            int vertex = stack_.top();
            stack_.pop();

            for (int predecessor : shortest_path_predecessors_[vertex]) {
                shortest_path_ratio =
                    (double) shortest_paths_[predecessor] /
                    (double) shortest_paths_[vertex];
                dependency_[predecessor] +=
                    shortest_path_ratio * (1 + dependency_[vertex]);
            }
        }
    }
};

#endif

int threads;

vector<double> betweenness;
queue<int> vertices_to_process;

vector<thread> threads_list;
mutex queue_mutex;
mutex betweenness_mutex;

void parse_args(int argc, char *argv[]) {
    // cerr << "have the numbers: " << argv[1] << endl;
    threads = stoi(argv[1]);
}

void parse_input() {
    Parser parser;
}

void init() {
    for (int vertex = 0; vertex <= MAXN; vertex++)
        betweenness.push_back(0);
    for (int vertex = 0; vertex <= MAXN; vertex++)
        if (graph.has_edges(vertex))
            vertices_to_process.push(vertex);
}

int next_vertex() {
    lock_guard<mutex> lock(queue_mutex);

    if (vertices_to_process.empty()) {
        return -1;
    }

    int vertex = vertices_to_process.front();
    vertices_to_process.pop();

    return vertex;
}

void update_betweenness(int vertex, DependencyCalculator& dc) {
    lock_guard<mutex> lock(betweenness_mutex);

    for (int v = 0; v <= MAXN; v++)
        if (v != vertex)
            betweenness[v] += dc.get_dependency(v);
}

void launch_threads() {
    for (int i = 0; i < threads; i++) {
        threads_list.emplace_back([&] () {
            int vertex = next_vertex();
            while(vertex != -1) {
                DependencyCalculator dc(vertex);
                update_betweenness(vertex, dc);
                vertex = next_vertex();
            }
        });
    }
}

void join_threads() {
    for (thread& thread : threads_list) {
        thread.join();
    }
}

void print_betweenness() {

    double mn = 0, mx = 0;
    for (int vertex = 0; vertex <= MAXN; vertex++)
        if (graph.has_edges(vertex))
            mx = max(mx, betweenness[vertex]);

    printf("[");

    bool if_first = true;

    for (int vertex = 0; vertex <= MAXN; vertex++) 
        if (graph.has_edges(vertex)) {
            double ans = (betweenness[vertex] - mn) / (mx - mn) + eps;
            if (if_first)
                if_first = false;
            else
                printf(",");
            printf("(%d,%.2f)", vertex, 0.01 * round(ans * 100));
        }
    printf("]");
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    parse_input();

    init();

    launch_threads();
    join_threads();

    print_betweenness();
    return 0;
}
