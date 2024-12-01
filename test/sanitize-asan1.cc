//
// Created by Hedzr Yeh on 2022/8/25.
//

int global_array[100] = {-1};

int main(int argc, char **argv) {
	return global_array[argc + 100]; // global buffer overflow
}