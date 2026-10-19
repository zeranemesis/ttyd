static const double zero = 0.0;

double __kernel_tan(double x, double y, int iy);
int __ieee754_rem_pio2(double x, double* y);

double tan(double x) {
	double y[2];
	int n;
	int ix;

	ix = ((int*)&x)[0] & 0x7FFFFFFF;
	if (ix <= 0x3FE921FB) {
		return __kernel_tan(x, zero, 1);
	}
	if (ix >= 0x7FF00000) {
		return x - x;
	}
	n = __ieee754_rem_pio2(x, y);
	return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
}
