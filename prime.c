int div_num(int x) {

	// every number is divisible by 1 and itself, so we can skip them
	int div = x == 1 ? 1 : 2;

	// odd numbers are never divisible by even numbers, so we skip them
	int step = x % 2 == 0 ? 1 : 2;

	// min possible div for even numbers is 2 and for odd numbers is 3
	int min_div = step + 1;

	// max possible div is x / min_div (besides itself)
	int max_div = (int)(x / min_div);

	// Calculate actual min and max div for odd numbers
	if (step == 2) {
		while(x % min_div != 0 && min_div < max_div){
			min_div += step;
		}
		max_div = x / min_div;
	}

	// if the number is prime, it won't even enter the loop
	for (int i = min_div; i <= max_div; i += step) {
		if (x % i == 0)
			div++;
	}

	return div;
}
