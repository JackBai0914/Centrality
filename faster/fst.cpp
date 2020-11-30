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
#define eps 0
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

int n, link[MAXN][30], deg[MAXN];
atomic<int> dis[MAXN], path[MAXN], suc_cnt[MAXN], q_cnt[MAXN];
int q[100][MAXN], suc[MAXN][30];
double _d[MAXN], bet[MAXN];
void solve(int s) {
	atomic<int> phase;
	phase = 0;
	for(int i = 1; i <= n; i++) _d[i] = path[i] = 0, dis[i] = -1, suc_cnt[i] = 0;
	for(int i = 0; i <= 100; i++) q_cnt[i] = 0;
	dis[s] = 0, path[s] = 1;
	q_cnt[phase] = 1, q[phase][0] = s;
	while(q_cnt[phase] > 0) {
		int *f = q[phase], *g = q[phase + 1];
		// cout << "Phase " << phase << endl;
		// #pragma omp parallel for
		for (int i = 0; i < q_cnt[phase]; i++) {
			int u = f[i];
			// #pragma omp parallel for
			for(int j = 1; j <= deg[u]; j++) {
				int v = link[u][j];
				int p, dv = -1;
				dis[v].compare_exchange_weak(dv, phase + 1);
				// if(u == 3) cerr << "solve: " << s << ' ' << v << ' ' << phase << ' ' << dv << endl;
				if(dv == -1) p = q_cnt[phase + 1].fetch_add(1), g[p] = v, dv = phase + 1;
				// cerr << dv << endl;
				if(dv == phase + 1) p = suc_cnt[u].fetch_add(1), suc[u][p] = v, path[v].fetch_add(path[u]);
			}
		}
		++phase;
	}
	--phase;
	while(phase > 0) {
		--phase;
		// #pragma omp parallel for
		for(int i = 0; i < q_cnt[phase]; i++) {
			int u = q[phase][i];
			for(int j = 0; j < suc_cnt[u]; j++) {
				int v = suc[u][j];
				// cout << "Link: " << u << ' ' << v << endl;
				_d[u] += 1. * path[u] / path[v] * (1 + _d[v]);
			}
			if(u != s) bet[u] += _d[u];
		}
	}
}
int exi[MAXN];
int main() {
	// omp_set_num_threads(24);
	int u, v;
	while(inputchar() != ']') {
		read(u, v), ++u, ++v;
		link[u][++deg[u]] = v;
		exi[u] = exi[v] = 1;
		chkmax(n, u), chkmax(n, v);
	}
	rep(i, 1, n) solve(i);


    double mx = 0;
    rep(i, 1, n) chkmax(mx, bet[i]);

    cout << fixed << setprecision(2);
    cout << '[';
    bool ini = true;
    rep(i, 1, n) {
        if (exi[i]) {
            if (!ini)
                cout << ',';
            cout << '(' << i-1 << ',' << bet[i] << ')';
            ini = false;
        }
    }
    cout << ']';

	// double bg = 0;
	// rep(i, 1, n) chkmax(bg, bet[i]);
	// putchar('[');
 //    cerr << fixed << setprecision(10);

	// // cerr << bg << endl;
	// // rep (i, 1, n)
	// // 	if (i == 362)
	// // 		cerr << bet[i] << " " << bg << " " << bet[i] / bg << " " << bet[i] / bg + eps << endl;
	// rep(i, 1, n) if(exi[i]) printf("(%d,%.2f)", i-1, bet[i] / bg + eps), cout << ",]"[i == n];
	// rep(i, 1, n) if(exi[i]) printf("(%d,%.2lf)", i-1, bet[i]), cout << ",]"[i == n];
	return 0;
}