double copysign(double x, double y) {
	((unsigned int*)&x)[0] = (((unsigned int*)&x)[0] & 0x7FFFFFFF) | (((unsigned int*)&y)[0] & 0x80000000);
	return x;
}
