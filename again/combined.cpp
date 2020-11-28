#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <ctime>
#define MAXN 12500
#define TIME (double)clock()/CLOCKS_PER_SEC
using namespace std;

int ind[MAXN], outd[MAXN];

    unordered_set<int> vertices_;
    vector<int> orderable_vertices_;
    vector<vector<int>> graph_;
    vector<bool> has_out_edges_;
    unordered_map<int, int> mapped_to_;
    int number_vertices_ = 0;

    void add_vertex(int vertex) {
        if (vertices_.find(vertex) == vertices_.end()) {
            vertices_.insert(vertex);
            orderable_vertices_.push_back(vertex);

            graph_.push_back(vector<int>());
            has_out_edges_.push_back(false);
            number_vertices_++;
        }
    }

    void add_edge(int from, int to) {
        graph_[mapped_to_[from]].push_back(mapped_to_[to]);
        has_out_edges_[mapped_to_[from]] = true;
        has_out_edges_[mapped_to_[to]] = true;
        outd[mapped_to_[from]] ++;
        ind[mapped_to_[to]] ++;
    }
    void sort_vertices_() {
        sort(orderable_vertices_.begin(), orderable_vertices_.end());
    }

    void map_vertices_() {
        for (int i = 0; i < number_vertices_; i++) {
            mapped_to_[orderable_vertices_[i]] = i;
        }
    }
    void done_with_vertices() {
        sort_vertices_();
        map_vertices_();
    }

    // int get_number_vertices()  {
    //     return number_vertices_;
    // }

    // const vector<int>& get_neighbors(int vertex)  {
    //     return graph_[vertex];
    // }

    // bool has_out_edges(int vertex)  {
    //     return has_out_edges_[vertex];
    // }

    // int get_real_vertex(int vertex)  {
    //     return orderable_vertices_[vertex];
    // }


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
    Parser() {
        int u, v;
        while(inputchar() != ']') {
            read(u, v);
            edges_.push_back(pair<int, int>(u, v));
        }
        add_vertices_();
        add_edges_();
    }

private:
    vector<pair<int, int>> edges_;

    void add_vertices_() {
        for (auto edge : edges_) {
            add_vertex(edge.first);
            add_vertex(edge.second);
        }
        done_with_vertices();
    }

    void add_edges_() {
        for (auto edge : edges_)
            add_edge(edge.first, edge.second);
    }
};

#endif

////////////?DC////////////////
#ifndef DEPENDENCY_CALCULATOR_H
#define DEPENDENCY_CALCULATOR_H

const int MAXT = 50;
const int MAXD = 16;

int stack[MAXT][MAXN], tp[MAXT];
int dist[MAXT][MAXN];
int que[MAXT][MAXN], qst[MAXT], qed[MAXT];
int shortest_paths[MAXT][MAXN];
double dependency[MAXT][MAXN];
int shortest_path_predecessors[MAXT][MAXN][MAXD], sppst[MAXT][MAXN];

// int shortest_path_predecessors_[MAXT][MAXN]

class DependencyCalculator {
public:
    DependencyCalculator(int vertex, int tid) :
            vertex_(vertex), tid(tid){
        init_();
        find_shortest_paths_();
        calculate_dependencies_();
    }
    int vertex_, tid; // s
    // vector<double> dependency_; // delta
private:
    

    // stack<int> stack_; // S
    vector<vector<int>> shortest_path_predecessors_; // P
    // vector<int> shortest_paths_; // sigma
    // vector<int> distance_; // d
    // queue<int> queue_; // Q
    

    void init_() {
        memset(shortest_paths[tid], 0, sizeof (shortest_paths[tid]));
        memset(dist[tid], 0, sizeof (dist[tid]));
        memset(dependency[tid], 0, sizeof (dependency[tid]));
        memset(sppst[tid], 0, sizeof sppst[tid]);
        // for (int vertex = 0; vertex < graph.get_number_vertices(); vertex++) {
        //     shortest_paths[tid][vertex] = 0;
        //     dist[tid][vertex] = 0;
        //     dependency[tid][vertex] = 0;
        // }


        // shortest_path_predecessors_.resize(number_vertices_);

        shortest_paths[tid][vertex_] = 1;
        dist[tid][vertex_] = 1;

        qed[tid] = 0, qst[tid] = 1;
        que[tid][++ qed[tid]] = vertex_;
    }

    void find_shortest_paths_() {
        while (qst[tid] <= qed[tid]) {
            int vertex = que[tid][qst[tid] ++];
            stack[tid][++ tp[tid]] = vertex;

            for (int neighbor : graph_[vertex]) {
                if (dist[tid][neighbor] <= 0) {
                    que[tid][++ qed[tid]] = neighbor;
                    dist[tid][neighbor] = dist[tid][vertex] + 1;
                }

                if (dist[tid][neighbor] == dist[tid][vertex] + 1) {
                    shortest_paths[tid][neighbor] += shortest_paths[tid][vertex];
                    shortest_path_predecessors[tid][neighbor][++ sppst[tid][neighbor]] = vertex;
                    // shortest_path_predecessors_[neighbor].push_back(vertex);
                }
            }
        }
    }

    void calculate_dependencies_() {
        while (tp[tid]) {
            int vertex = stack[tid][tp[tid] --];
            for (int i = 1; i <= sppst[tid][vertex]; i ++) {
                int predecessor = shortest_path_predecessors[tid][vertex][i];
            // for (int predecessor : shortest_path_predecessors_[vertex]) {
                double shortest_path_ratio =
                    (double) shortest_paths[tid][predecessor] /
                    (double) shortest_paths[tid][vertex];
                dependency[tid][predecessor] +=
                    shortest_path_ratio * (1 + dependency[tid][vertex]);
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
    for (int vertex = 0; vertex < number_vertices_; vertex++) {
        betweenness.push_back(0);
        vertices_to_process.push(vertex);
    }
}

int tmp[1000000], tst = 0;
int v_belong_t[MAXN];

int next_vertex(int &tid) {
    lock_guard<mutex> lock(queue_mutex);

    int htid = hash<thread::id>{}(this_thread::get_id()) % 1000000;

    if (!tmp[htid])
        tmp[htid] = ++ tst;
    tid = tmp[htid];

    if (vertices_to_process.empty()) {
        return -1;
    }

    int vertex = vertices_to_process.front();
    vertices_to_process.pop();
    // cerr << "dealing with " << vertex << " and the thread is " << this_thread::get_id() << endl;
    v_belong_t[vertex] = tid;
    return vertex;
}

double ti = 0;
void update_betweenness(int vertex, DependencyCalculator& dc) {
    // ti -= TIME;
    lock_guard<mutex> lock(betweenness_mutex);

    for (int v = 0; v < number_vertices_; v++) {
        if (v != vertex)
            betweenness[v] += dependency[dc.tid][v];
    }
    // ti += TIME;
}



void launch_threads() {
    for (int i = 0; i < threads; i++) {
        threads_list.emplace_back([&] () {
            int tid;
            int vertex = next_vertex(tid);
            while(vertex != -1) {
                DependencyCalculator dc(vertex, tid);
                update_betweenness(vertex, dc);

                vertex = next_vertex(tid);
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

    double mn = 2e9, mx = 0;
    for (int vertex = 0; vertex < number_vertices_; vertex++) {
            mn = min(mn, betweenness[vertex]);
            mx = max(mx, betweenness[vertex]);
    }
    cout << fixed << setprecision(2);
    cout << '[';
    for (int vertex = 0; vertex < number_vertices_; vertex++) {
        if (has_out_edges_[vertex]) {
            cout << '(' << orderable_vertices_[vertex] << ',' << (double)(betweenness[vertex] - mn) / (mx- mn) << ')';
            if (vertex + 1 != number_vertices_)
                cout << ',';
        }
    }
    cout << ']';
}

int main(int argc, char *argv[]) {
    cerr << fixed << setprecision(10);
    parse_args(argc, argv);
    parse_input();
    init();
    cerr << "mid: " << TIME << endl;
    launch_threads();
    join_threads();

    print_betweenness();
    cerr << "time " << TIME << endl;

    // for (int vertex = 0; vertex < number_vertices_; vertex++)
    //     cerr << vertex << " : " << v_belong_t[vertex] << endl;

    int mxind = 0, mxoutd = 0;
    for (int vertex = 0; vertex < number_vertices_; vertex ++) {
        mxind = max (mxind, ind[vertex]);
        mxoutd = max (mxoutd, outd[vertex]);
    }

    cerr << "mx: " << mxind << " " << mxoutd << endl;

    return 0;
}