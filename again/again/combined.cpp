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

/////////////GRAPH////////////////
#ifndef GRAPH_H
#define GRAPH_H



class Graph {
public:
    Graph() : vertices_(), graph_(), number_vertices_(0) {}

    void add_vertex(int vertex) {
        if (vertices_.find(vertex) == vertices_.end()) {
            vertices_.insert(vertex);
            orderable_vertices_.push_back(vertex);

            graph_.push_back(std::vector<int>());
            has_out_edges_.push_back(false);
            number_vertices_++;
        }
    }

    void add_edge(int from, int to) {
        graph_[mapped_to_[from]].push_back(mapped_to_[to]);
        has_out_edges_[mapped_to_[from]] = true;
        has_out_edges_[mapped_to_[to]] = true;
    }

    void done_with_vertices() {
        sort_vertices_();
        map_vertices_();
    }

    int get_number_vertices() const {
        return number_vertices_;
    }

    const std::vector<int>& get_neighbors(int vertex) const {
        return graph_[vertex];
    }

    bool has_out_edges(int vertex) const {
        return has_out_edges_[vertex];
    }

    int get_real_vertex(int vertex) const {
        return orderable_vertices_[vertex];
    }
private:
    std::unordered_set<int> vertices_;
    std::vector<int> orderable_vertices_;
    std::vector<std::vector<int>> graph_;
    std::vector<bool> has_out_edges_;
    std::unordered_map<int, int> mapped_to_;
    int number_vertices_;

    void sort_vertices_() {
        std::sort(orderable_vertices_.begin(), orderable_vertices_.end());
    }

    void map_vertices_() {
        for (int i = 0; i < number_vertices_; i++) {
            mapped_to_[orderable_vertices_[i]] = i;
        }
    }
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

class Parser {
public:
    Parser() : graph_() {
        int u, v;
        while(inputchar() != ']') {
            read(u, v);
            // cerr << "input: " << u << " " << v << endl;
            edges_.push_back(std::pair<int, int>(u, v));
        }
        add_vertices_();
        add_edges_();
    }

    const Graph& get_graph() {
        return graph_;
    }
private:
    Graph graph_;
    std::vector<std::pair<int, int>> edges_;

    void parse_edge_(std::string edge) {
        stringstream sstream(edge);

        string s;
        cin >> s;

        int from, to;
        sstream >> from >> to;

        edges_.push_back(std::pair<int, int>(from, to));
    }

    void add_vertices_() {
        for (auto edge : edges_) {
            graph_.add_vertex(edge.first);
            graph_.add_vertex(edge.second);
        }

        graph_.done_with_vertices();
    }

    void add_edges_() {
        for (auto edge : edges_) {
            graph_.add_edge(edge.first, edge.second);
        }
    }
};

#endif

////////////?DC////////////////
#ifndef DEPENDENCY_CALCULATOR_H
#define DEPENDENCY_CALCULATOR_H

class DependencyCalculator {
public:
    DependencyCalculator(const Graph& graph, int vertex) : graph_(graph),
            vertex_(vertex) {
        init_();
        find_shortest_paths_();
        calculate_dependencies_();
    }

    double get_dependency(int vertex) const {
        return dependency_[vertex];
    }
private:
    const Graph& graph_; // (V, E)
    int vertex_; // s

    std::stack<int> stack_; // S
    std::vector<std::vector<int>> shortest_path_predecessors_; // P
    std::vector<int> shortest_paths_; // sigma
    std::vector<int> distance_; // d
    std::queue<int> queue_; // Q
    std::vector<double> dependency_; // delta

    void init_() {
        for (int vertex = 0; vertex < graph_.get_number_vertices(); vertex++) {
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

            for (int neighbor : graph_.get_neighbors(vertex)) {
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
        while (!stack_.empty()) {
            int vertex = stack_.top();
            stack_.pop();

            for (int predecessor : shortest_path_predecessors_[vertex]) {
                double shortest_path_ratio =
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

Graph graph;
std::vector<double> betweenness;
std::queue<int> vertices_to_process;

std::vector<std::thread> threads_list;
std::mutex queue_mutex;
std::mutex betweenness_mutex;

void parse_args(int argc, char *argv[]) {
    // cerr << "have the numbers: " << argv[1] << endl;
    threads = std::stoi(argv[1]);
}

void parse_input() {
    Parser parser;
    graph = parser.get_graph();
}

void init() {
    for (int vertex = 0; vertex < graph.get_number_vertices(); vertex++) {
        betweenness.push_back(0);
        vertices_to_process.push(vertex);
    }
}

int next_vertex() {
    std::lock_guard<std::mutex> lock(queue_mutex);

    if (vertices_to_process.empty()) {
        return -1;
    }

    int vertex = vertices_to_process.front();
    vertices_to_process.pop();

    return vertex;
}

void update_betweenness(int vertex, DependencyCalculator& dc) {
    std::lock_guard<std::mutex> lock(betweenness_mutex);

    for (int v = 0; v < graph.get_number_vertices(); v++) {
        if (v != vertex) {
            double pans = dc.get_dependency(v);
            betweenness[v] += pans;
        }
    }
}

void launch_threads() {
    for (int i = 0; i < threads; i++) {
        threads_list.emplace_back([&] () {
            int vertex = next_vertex();
            while(vertex != -1) {
                DependencyCalculator dc(graph, vertex);
                update_betweenness(vertex, dc);

                vertex = next_vertex();
            }
        });
    }
}

void join_threads() {
    for (std::thread& thread : threads_list) {
        thread.join();
    }
}

void print_betweenness() {

    double mn = 2e9, mx = 0;
    for (int vertex = 0; vertex < graph.get_number_vertices(); vertex++) {
            mn = min(mn, betweenness[vertex]);
            mx = max(mx, betweenness[vertex]);
    }
    cout << fixed << setprecision(2);
    cout << '[';
    for (int vertex = 0; vertex < graph.get_number_vertices(); vertex++) {
        if (graph.has_out_edges(vertex)) {
            cout << '(' << graph.get_real_vertex(vertex) << ',' << (double)(betweenness[vertex] - mn) / (mx- mn) << ')';
            if (vertex + 1 != graph.get_number_vertices())
                cout << ',';
        }
    }
    cout << ']';
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
