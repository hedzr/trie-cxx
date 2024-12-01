//
// Created by Hedzr Yeh on 2022/8/25.
//

#include <iostream>

void f() {
	int a[2] = {1, 0};
	int b = a[2];
	std::cout << b << std::endl;
}

int main() {
	f();
	return 0;
}