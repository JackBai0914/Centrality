#include <bits/stdc++.h>
#define il inline
#define pb push_back
#define mp make_pair
#define fir first
#define sec second
#define mid ((l+r)>>1)
#define MAXN 200050
#define MAXM 
#define mod 
#define inf (1ll << 30)
#define eps (1e-6)
#define alpha 0.75
#define rep(i, x, y) for( int i = x; i <= y; ++i)
#define repd(i, x, y) for( int i = x; i >= y; --i)
#define file(s) freopen(s".in", "r", stdin), freopen(s".out", "w", stdout)
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;
typedef pair <int, int> pii;
typedef pair <ll, int> pli;
typedef pair <int, ll> pil;
typedef pair <ll, ll> pll;
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

    template <typename T> inline void read(T &x) {
    	char ch = inputchar(); int f = 1; x = 0;
    	while(ch < '0' || ch > '9') {if(ch == '-') f = -1; ch = inputchar();}
    	while(ch >= '0' && ch <= '9') x = x*10+ch-'0', ch = inputchar();
    	x *= f;
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
double res[MAXN], _d[MAXN], path[MAXN];
int st[MAXN], q[MAXN], dis[MAXN];
vector<int> pred[MAXN];
vector<int> adj[MAXN];
void bfs(int s) {
	rep(i, 1, n) _d[i] = path[i] = 0, dis[i] = n + 1, pred[i].clear();
	int fr = 1, re = 0, tp = 0;
	q[++re] = s, dis[s] = 0, path[s] = 1;
	while(fr <= re) {
		int u = q[fr++];
		st[++tp] = u;
		for(int v: adj[u]) {
			if(dis[v] > n) dis[v] = dis[u] + 1, q[++re] = v;
			if(dis[v] == dis[u] + 1) path[v] += path[u], pred[v].pb(u);
		}
	}
	while(tp) {
		int u = st[tp--];
		for(int v: pred[u]) _d[v] += path[v] / path[u] * (1 + _d[u]);
		if(u != s) res[u] += _d[u];
	}
}
bool exi[MAXN];
int main() {
	int u, v;
	// freopen("1.in", "r", stdin);
	while(inputchar() != ']') {
		read(u, v), ++u, ++v;
        exi[u] = exi[v] = true;
		adj[u].pb(v);
		chkmax(n, u), chkmax(n, v);
	}
	// rep(i, 1, n) read(u, v), adj[u].pb(v);
	rep(i, 1, n) {
        bfs(i);
        // if (i % 100 == 0)
        //     cerr << "finish " << i << endl;
    }
	double bg = 0;
	rep(i, 1, n) chkmax(bg, res[i]);
    printf("[");
	rep(i, 1, n) if (exi[i]) {
       printf("(%d,%.2lf)", i - 1, res[i] / bg);
       if (i != n)
            printf(",");
    }
    printf("]");
	return 0;
}