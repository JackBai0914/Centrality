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
#include <bits/stdc++.h>
using namespace std;
#define pb push_back
#define il inline
#define MAXN 20020
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
	double _d[MAXN], bet[MAXN];
	int path[MAXN], st[MAXN], q[MAXN], dis[MAXN];
    vector <int> pred[MAXN], cadj[MAXN];
	int l, r;
	void solve() {
		for(int i = 1; i <= n; i++) bet[i] = 0;
		// cout << "SOSL" << endl;
		for(int i = l; i <= r; i++) bfs(i);
	}
    void bfs(int s) {
		// cout << "S" << endl;
        for (int i = 1; i <= n; i ++) {
            dis[i] = n + 1, _d[i] = 0;
        }
        int fr = 1, re = 0, tp = 0;
        q[++re] = s, dis[s] = 0, path[s] = 1;
        while(fr <= re) {
            int u = q[fr++];
            st[++tp] = u;
            for(int v: cadj[u]) {
                if(dis[v] > n) dis[v] = dis[u] + 1, path[v] = 0, q[++re] = v, pred[v].clear();
                if(dis[v] == dis[u] + 1) path[v] += path[u], pred[v].pb(u);
            }
        }
        while(tp) {
            int u = st[tp--];
            for(int v: pred[u]) 
                _d[v] += 1. * path[v] / path[u] * (1 + _d[u]);
        }
    }
} F[50];

int thread_num;

vector<thread> threads_list;

void solve(int id, int l, int r) {
	// cout << id << ' ' << l << ' ' << r << endl;
	Dependency_Calc cur = F[id];
	cur.l = l, cur.r = r;
	cur.solve();
	// cout << id << ' ' << endl;
	F[id] = cur;
}
int bel[MAXN], R[MAXN], L[MAXN];
void launch_threads() {
	int Sz = (n - 1) / thread_num + 1;
	for(int i = 1; i <= n; i ++) bel[i] = (i - 1) / Sz + 1;
	assert(bel[n] == thread_num);
	for(int i = 1; i <= n; i++) R[bel[i]] = i;
	for(int i = n; i >= 1; --i) L[bel[i]] = i;
	solve(1, 1, 1250);
    for (int i = 0; i < thread_num; i++) {
		cout << i << ' ' << L[i+1] << ' ' << R[i+1] << endl;
        threads_list.emplace_back(thread(solve, i + 1, L[i + 1], R[i + 1]));
    }
}


int main(int argc, char *argv[]) {
	freopen("1.in", "r", stdin);
    cerr << fixed << setprecision(6);
    // thread_num = stoi(argv[1]);
	thread_num = 4;
    int mxp = 0, nump = 0, cnt = 0;

    int u, v;
    while(inputchar() != ']') {
        read(u, v), ++u, ++v;
        cnt ++;
        nump += (exi[u] == false);
        nump += (exi[v] == false);
        exi[u] = exi[v] = true;
        adj[u].pb(v);
		for(int i = 0; i < thread_num; i++) F[i].cadj[u].pb(v);
        chkmax(n, u);
        chkmax(n, v);
    }
    mxp = n;

    cerr << "after input: " << TIME << endl;

    launch_threads();

    for (thread& th : threads_list)
        th.join();
    cerr << "after calculation: " << TIME << endl;

	for(int i = 1; i <= n; i++) 
		for(int j = 0; j < thread_num; j++)
			betweenness[i] += F[j].bet[i];

    double bg = 0;
    for (int i = 1; i <= n; i ++)
        chkmax(bg, betweenness[i]);
    
    cerr << "running time: " << TIME << endl;
    cerr << "data report: " << mxp << ", " << nump << ", " << cnt << endl;
    
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