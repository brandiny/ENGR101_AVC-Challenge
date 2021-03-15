#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

signed main() {
	int N; 
	cin >> N;
	vector<<int, int>> arrive_leave(N);
	for (int i = 0; i < N; ++i) {
		cin >> arrive_leave[i];
	}
	for (auto x: arrive_leave) {
		cout << x << " ";
	}
	
	return 0;
}
