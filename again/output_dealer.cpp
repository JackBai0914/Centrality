/*
*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
* 
* @author: Xingjian Bai 
* @date: 2020-11-27 19:52:31
* @description: 
* /Users/jackbai/Desktop/并行计算/Centrality/github/latest_version/output_dealer.cpp 
* 
* @notes: 
* g++ -fsanitize=address -ftrapv output_dealer.cpp
*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
#include <bits/stdc++.h>
#define F first
#define S second
#define MP make_pair
#define TIME (double)clock()/CLOCKS_PER_SEC
using namespace std;
typedef long long ll;
typedef long double ld;
typedef pair <int, int> pii;
const int MOD = 1000000007; 
const ll INF = 1e18;
const ld eps = 1e-8;
#define FOR(i,a,b) for (int i = (a); i < (b); i ++)
#define F0R(i,a) FOR(i, 0, a)
#define ROF(i, a, b) for (int i = (b) - 1; i >= a; i --)
#define R0F(i, a) ROF(i, 0, a)
#define trav(a, x) for (auto& a: x)
#define debug(x) cerr << "(debug mod) " << #x << " = " << x << endl


int main() {
	string s;
	cin >> s;
	for (int i = 0; i < s.size(); i ++) {
		if (s[i] == '[' || s[i] == ']')
			continue ;
		if (s[i] == '(') {
			string fir = "";
			string sec = "";
			while (s[++ i] != ',')
				fir += s[i];
			while (s[++ i] != ')')
				sec += s[i];
			int a;
			double b;
			stringstream sa, sb;
			sa << fir, sb << sec;
			sa >> a, sb >> b;
			cout << a << " " << b << endl;
		}
	}
	


    return 0;
}