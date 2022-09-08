#include <stdio.h>

int div_num(int x) {
	int div = 0;
	for (int i = 2; i < x; i++) {
		if (x % i == 0)
			div++;
	}
	return div;
}
