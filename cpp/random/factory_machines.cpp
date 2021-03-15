#include <iostream>
#include <vector>
#include <algorithm>
#define int long long
using namespace std;

bool enoughProducts(int time, int needed, vector<int> v);

signed main() {
	int n, t;
	cin >> n >> t;
	
	int total_time = 0;
	vector<int> product_times(n);
	for (int i = 0; i < n; ++i) {
		cin >> product_times[i];
		total_time += product_times[i];
	}
	
	unsigned int L, R;
	L = 0;
	R = 1e18;
	while (L < R) {
		int M = (L + R) / 2;
		bool good = enoughProducts(M, t, product_times);
		if (good)
			 R = M;
		else
			L = M+1;
		//cout << "L " << L << " R " << R << endl;
	}
	
	cout << L;
	
}

bool enoughProducts(int time, int needed, vector<int> v) {
	int products = 0;
	for (auto x: v) {
		products += time / x;
		if (products > needed) {
			break;
		}
	}
	return products >= needed;
}

