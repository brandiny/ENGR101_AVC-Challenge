#include <iostream>
#include <array>
#include <algorithm>
#define int long long 
using namespace std;

signed main() {
	int n, q;
	cin >> n >> q;
	
	int numbers [n];
	for (int i = 0; i < n; ++i) 
		cin >> numbers[i];
	
	int prefix_min [n];
	prefix_min[0] = numbers[0];
	for (int i = 1; i < n; ++i) {
		prefix_min[i] = min(prefix_min[i-1], numbers[i]);
	}
	
	/*
	for (auto x: prefix_min)
		cout << x << " ";
	*/

	for (int i = 0; i < q; ++i) {
		int a, b;
		cin >> a >> b;
		if (a == 1)
			cout << prefix_min[b-1] << endl;
		else 
			cout << min(prefix_min[b-1], prefix_min[a-2]) << endl;
		
	}
	
}

