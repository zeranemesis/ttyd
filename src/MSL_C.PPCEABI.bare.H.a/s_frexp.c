static const double two54 = 1.8014398509461984e16;

double frexp(double x, int* eptr) {
	int hx;
	int ix;
	int lx;

	hx = ((int*)&x)[0];
	ix = hx & 0x7FFFFFFF;
	lx = ((int*)&x)[1];
	*eptr = 0;
	if (ix >= 0x7FF00000 || ((ix | lx) == 0)) {
		return x;
	}
	if (ix < 0x00100000) {
		*eptr = -54;
		x *= two54;
		ix = ((int*)&x)[0] & 0x7FFFFFFF;
	}
	((int*)&x)[0] = (((int*)&x)[0] & 0x800FFFFF) | 0x3FE00000;
	*eptr = ((ix >> 20) + *eptr) - 0x3FE;
	return x;
}
