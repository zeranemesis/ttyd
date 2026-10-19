extern unsigned char __upper_map[];

double __ieee754_sqrt(double x);

double sqrt(double x) {
	return __ieee754_sqrt(x);
}

static void upper_str(signed char* s) {
	signed char* p = s;
	while (*p != 0) {
		*p = (*p == -1) ? -1 : __upper_map[*p & 0xFF];
		p++;
	}
}
