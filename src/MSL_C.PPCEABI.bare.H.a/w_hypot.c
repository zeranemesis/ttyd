double __ieee754_hypot(double x, double y);

double hypot(double x, double y) {
	return __ieee754_hypot(x, y);
}
