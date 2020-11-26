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
#define pb push_back
#define il inline
#define MAXN 10010
#define eps (1e-6)
#define TIME (double)clock()/CLOCKS_PER_SEC
using namespace std;
template <typename T> il bool chkmin(T &x, T y) {return x > y ? x = y, 1 : 0;}
template <typename T> il bool chkmax(T &x, T y) {return x < y ? x = y, 1 : 0;}

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

int n, m;
bool exi[MAXN];
vector<int> adj[MAXN];
double betweenness[MAXN];

struct Dependency_Calc {

    vector <double> _d, path;
    vector <int> st, q, dis;
    vector <int > pred[MAXN];    
    
    Dependency_Calc() {}
    Dependency_Calc(int _s) {bfs(_s);}

    void bfs(int s) {
        _d.resize(n+1);
        path.resize(n+1);
        st.resize(n+1);
        q.resize(n+1);
        dis.resize(n+1);
        for (int i = 0; i <= n; i ++) {
            dis[i] = n + 1;
            // pred.emplace_back();
        }
        int fr = 1, re = 0, tp = 0, u;
        q[++re] = s, dis[s] = 0, path[s] = 1;
        while(fr <= re) {
            u = q[fr++];
            st[++tp] = u;
            for(int v: adj[u]) {
                if(dis[v] > n) dis[v] = dis[u] + 1, q[++re] = v;
                if(dis[v] == dis[u] + 1) path[v] += path[u], pred[v].pb(u);
            }
        }
        while(tp) {
            u = st[tp--];
            for(int v: pred[u])
                _d[v] += path[v] / path[u] * (1 + _d[u]);
        }
    }
};

int thread_num;

vector<thread> threads_list;
mutex queue_mutex;
mutex betweenness_mutex;

vector <int> allpt;

int next_vertex() {
    lock_guard<mutex> lock(queue_mutex);
    if (allpt.empty())
        return -1;
    int vertex = allpt[allpt.size() - 1];
    allpt.pop_back();
    return vertex;
}

void update_betweenness(Dependency_Calc& dc, int s) {
    lock_guard<mutex> lock(betweenness_mutex);

    for (int v = 1; v <= n; v ++) {
        if (v != s) {
            betweenness[v] += dc._d[v];
        }
    }
}

void launch_threads() {
    for (int i = 1; i <= n; i ++)
        if (exi[i])
            allpt.push_back(i);
    for (int i = 0; i < thread_num; i++) {
        threads_list.emplace_back([&] () {
            int vertex = next_vertex();
            while(vertex != -1) {
                Dependency_Calc dc(vertex);
                update_betweenness(dc, vertex);
                vertex = next_vertex();
            }
        });
    }
}


int main(int argc, char *argv[]) {
    thread_num = stoi(argv[1]);

    int u, v;
    while(inputchar() != ']') {
        read(u, v), ++u, ++v;
        exi[u] = exi[v] = true;
        adj[u].pb(v);
        chkmax(n, u);
        chkmax(n, v);
    }

    // cerr << "N:" << n << endl;
    // cerr << TIME << endl;

    launch_threads();

    for (thread& th : threads_list)
        th.join();


    double bg = 0;
    for (int i = 1; i <= n; i ++)
        chkmax(bg, betweenness[i]);
    
    // cerr << TIME << endl;
    printf("[");
    for (int i = 1; i <= n; i ++)
        if (exi[i]) {
            printf("(%d,%.2lf)", i - 1, betweenness[i] / bg);
                if (i != n)
                    printf(",");
        }
    printf("]");
    return 0;
}