static const double zero = 0.0;

double __kernel_sin(double x, double y, int iy);
double __kernel_cos(double x, double y);
int __ieee754_rem_pio2(double x, double* y);

double sin(double x) {
	double y[2];
	int n;
	int ix;

	ix = ((int*)&x)[0] & 0x7FFFFFFF;
	if (ix <= 0x3FE921FB) {
		return __kernel_sin(x, zero, 0);
	}
	if (ix >= 0x7FF00000) {
		return x - x;
	}
	n = __ieee754_rem_pio2(x, y);
	switch (n & 3) {
	case 0:
		return __kernel_sin(y[0], y[1], 1);
	case 1:
		return __kernel_cos(y[0], y[1]);
	case 2:
		return -__kernel_sin(y[0], y[1], 1);
	default:
		return -__kernel_cos(y[0], y[1]);
	}
}
