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
#define MAXT 50
#define MAXD 16
#define TIME (double)clock()/CLOCKS_PER_SEC
using namespace std;


short graph_[MAXN][50], gst[MAXN];
bool exist[MAXN];

    void add_vertex(short vertex) {
        exist[vertex] = true;
    }

    void add_edge(short from, short to) {
        graph_[from][gst[from] ++] = to;
    }

//////////////PARSE/////////////
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
        short u, v;
        while(inputchar() != ']') {
            read(u, v);
            add_vertex(u);
            add_vertex(v);
            add_edge(u, v);
        }
    }
};

////////////DC////////////////

long long num = 0;

short stack[MAXT][MAXN], tp[MAXT];
short dist[MAXT][MAXN];
short que[MAXT][MAXN], qst[MAXT], qed[MAXT];
int shortest_paths[MAXT][MAXN];
double dependency[MAXT][MAXN];
short shortest_path_predecessors[MAXT][MAXN][MAXD], sppst[MAXT][MAXN];
bool if_vis[MAXT][MAXN];

class DependencyCalculator {
public:
    DependencyCalculator(short vertex, short tid): vertex_(vertex), tid(tid){
        init_();
        find_shortest_paths_();
        calculate_dependencies_();
    }
    short vertex_, tid;
private:
    void init_() {
        for (short vertex = 0; vertex < MAXN; vertex++) {
            shortest_paths[tid][vertex] = 0;
            dist[tid][vertex] = 0;
            sppst[tid][vertex] = 0;
            dependency[tid][vertex] = 0;
            if_vis[tid][vertex] = false;
        }

        shortest_paths[tid][vertex_] = 1;
        dist[tid][vertex_] = 1;
        if_vis[tid][vertex_] = true;
        qed[tid] = 0, qst[tid] = 1;
        que[tid][++ qed[tid]] = vertex_;
    }

    void find_shortest_paths_() {
        short vertex, i, neighbor;
        while (qst[tid] <= qed[tid]) {
            vertex = que[tid][qst[tid] ++];
            
            stack[tid][++ tp[tid]] = vertex;

            for (i = 0; i < gst[vertex]; i ++) {
                neighbor = graph_[vertex][i];
                if (if_vis[tid][neighbor] == false) {
                    que[tid][++ qed[tid]] = neighbor;
                    if_vis[tid][neighbor] = true;
                    dist[tid][neighbor] = dist[tid][vertex] + 1;
                    shortest_paths[tid][neighbor] += shortest_paths[tid][vertex];
                    shortest_path_predecessors[tid][neighbor][++ sppst[tid][neighbor]] = vertex;
                }

                else if (dist[tid][neighbor] == dist[tid][vertex] + 1) {
                    shortest_paths[tid][neighbor] += shortest_paths[tid][vertex];
                    shortest_path_predecessors[tid][neighbor][++ sppst[tid][neighbor]] = vertex;
                }
            }
        }
    }

    void calculate_dependencies_() {
        short vertex, i, predecessor;
        while (tp[tid]) {
            vertex = stack[tid][tp[tid] --];
            for (i = 1; i <= sppst[tid][vertex]; i ++) {
                predecessor = shortest_path_predecessors[tid][vertex][i];
                dependency[tid][predecessor] +=
                    shortest_paths[tid][predecessor] * (1 + dependency[tid][vertex]) / shortest_paths[tid][vertex];
            }
        }
    }
};



int threads;
vector<double> betweenness;
int vertices_to_process[MAXN], vtpst = 0;

vector<thread> threads_list;
mutex queue_mutex;
mutex betweenness_mutex;

void parse_args(int argc, char *argv[]) {
    threads = stoi(argv[1]);
}

void parse_input() {
    Parser parser;
}

void init() {
    for (int vertex = 0; vertex < MAXN; vertex++) 
        if (exist[vertex]) {
            betweenness.push_back(0);
            vertices_to_process[++ vtpst] = vertex;
        }
}

short tmp[100000], tst = 0;
short v_belong_t[MAXN];

short next_vertex(short &tid) {
    lock_guard<mutex> lock(queue_mutex);

    int htid = hash<thread::id>{}(this_thread::get_id()) % 100000;

    if (!tmp[htid])
        tmp[htid] = ++ tst;
    tid = tmp[htid];

    if (vtpst == 0) {
        return -1;
    }

    short vertex = vertices_to_process[vtpst --];
    v_belong_t[vertex] = tid;
    return vertex;
}

void update_betweenness(int vertex, DependencyCalculator& dc) {
    lock_guard<mutex> lock(betweenness_mutex);

    for (short v = 0; v < MAXN; v++)
        if (exist[v] && v != vertex)
            betweenness[v] += dependency[dc.tid][v];
}

void launch_threads() {
    for (int i = 0; i < threads; i++) {
        threads_list.emplace_back([&] () {
            short tid;
            short vertex = next_vertex(tid);
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

    double mn = 0, mx = 0;
    for (int vertex = 0; vertex < MAXN; vertex++)
        if (exist[vertex])
            mx = max(mx, betweenness[vertex]);
    cout << fixed << setprecision(2);
    cout << '[';
    bool ini = true;
    for (int vertex = 0; vertex < MAXN; vertex++) {
        if (exist[vertex]) {
            if (!ini)
                cout << ',';
            cout << '(' << vertex << ',' << (double)(betweenness[vertex] - mn) / (mx- mn) << ')';
            ini = false;
        }
    }
    cout << ']';

    // for (int vertex = 0; vertex < MAXN; vertex++)
    //     if (exist[vertex])
    //         cerr << vertex << " : " << betweenness[vertex] << endl;
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
    // cerr << "num: " << num << endl;
    return 0;
}