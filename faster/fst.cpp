#include <bits/stdc++.h>
#include <thread>
#include <atomic>
#include <omp.h>
#define il inline
#define pb push_back
#define mp make_pair
#define fir first
#define sec second
#define mid ((l+r)>>1)
#define MAXN 50050
#define MAXM 
#define mod 
#define inf (1ll << 30)
#define eps (1e-6)
#define alpha 0.75
#define rep(i, x, y) for(register int i = x; i <= y; ++i)
#define repd(i, x, y) for(register int i = x; i >= y; --i)
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
int n, link[MAXN][30], deg[MAXN];
atomic<int> dis[MAXN], path[MAXN], suc_cnt[MAXN], q_cnt[MAXN];
int q[130][MAXN], suc[MAXN][30];
double _d[MAXN], bet[MAXN];
const size_t nthreads = std::thread::hardware_concurrency();
void solve(int s) {
	atomic<int> phase;
	// atomic<int> p, dv;
	phase = 0;
	for(int i = 1; i <= n; i++) _d[i] = path[i] = 0, dis[i] = -1, suc_cnt[i] = 0;
	for(int i = 0; i <= 130; i++) q_cnt[i] = 0;
	dis[s] = 0, path[s] = 1;
	q_cnt[phase] = 1, q[phase][0] = s;
	while(q_cnt[phase] > 0) {
		int *f = q[phase], *g = q[phase + 1];
		// cout << "Phase " << phase << endl;
    	std::vector<std::thread> threads(nthreads);
    	for(int t = 0; t < nthreads; t++) {
      		threads[t] = std::thread(std::bind(
		        [&](const int bg, const int en) {
					for(int i = bg; i < en; i++) {
						int u = f[i];
						for(int j = 1; j <= deg[u]; j++) {
							int v = link[u][j];
							int p, dv = -1;
							dis[v].compare_exchange_weak(dv, phase + 1);
							// if(u == 3) cerr << "solve: " << s << ' ' << v << ' ' << phase << ' ' << dv << endl;
							if(dv == -1) p = q_cnt[phase + 1].fetch_add(1), g[p] = v, dv = phase + 1;
							if(dv == phase + 1) p = suc_cnt[u].fetch_add(1), suc[u][p] = v, path[v].fetch_add(path[u]);
						}
					}
				}, t*q_cnt[phase]/nthreads, (t+1)==nthreads ? int(q_cnt[phase]):(t+1)*q_cnt[phase]/nthreads));
    	}
	    std::for_each(threads.begin(),threads.end(),[](std::thread& x){x.join();});
		++phase;
	}
	--phase;
	while(phase > 0) {
		--phase;
		// #pragma omp parallel for
    	std::vector<std::thread> threads(nthreads);
    	for(int t = 0; t < nthreads; t++) {
      		threads[t] = std::thread(std::bind(
		        [&](const int bg, const int en) {
					for(int i = bg; i < en; i++) {
						int u = q[phase][i];
							for(int j = 0; j < suc_cnt[u]; j++) {
								int v = suc[u][j];
								// cout << "Link: " << u << ' ' << v << endl;
								_d[u] += 1. * path[u] / path[v] * (1 + _d[v]);
							}
							if(u != s) bet[u] += _d[u];
					}
				}, t*q_cnt[phase]/nthreads, (t+1)==nthreads ? int(q_cnt[phase]):(t+1)*q_cnt[phase]/nthreads));
    	}
	    std::for_each(threads.begin(),threads.end(),[](std::thread& x){x.join();});
	}
}
int exi[MAXN];
int main() {
	// omp_set_num_threads(48);
	int u, v;
	while(inputchar() != ']') {
		read(u, v), ++u, ++v;
		link[u][++deg[u]] = v;
		exi[u] = exi[v] = 1;
		chkmax(n, u), chkmax(n, v);
	}
	cerr << "finish input" << endl;
	// rep(i, 1, n) read(u, v), adj[u].pb(v);
	rep(i, 1, n) {
		solve(i);
		cerr << "solving " << i << endl;	
	} 
	double bg = 0;
	rep(i, 1, n) chkmax(bg, bet[i]);
	putchar('[');
	rep(i, 1, n) if(exi[i]) printf("(%d,%.2lf)", i-1, bet[i] / bg), cout << ",]"[i == n];
	puts("");
	return 0;
}