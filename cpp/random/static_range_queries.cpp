#include <iostream>
#include <array>
#define int long long 
using namespace std;

signed main() {
	int n, q;
	cin >> n >> q;
	
	int numbers [n];
	for (int i = 0; i < n; ++i) 
		cin >> numbers[i];
	
	int prefix_sum [n];
	prefix_sum[0] = numbers[0];
	for (int i = 1; i < n; ++i) {
		prefix_sum[i] = prefix_sum[i-1] + numbers[i];
	}
	/*
	for (auto x: prefix_sum) {
		cout << x << " ";
	}*/
	
	for (int i = 0; i < q; ++i) {
		int a, b;
		cin >> a >> b;
		if (a == 1)
			cout << prefix_sum[b-1] << endl;
		else 
			cout << prefix_sum[b-1] - prefix_sum[a-2] << endl;
		
	}
}
