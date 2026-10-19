extern unsigned int lbl_8042ACE0;

int rand(void) {
	lbl_8042ACE0 = lbl_8042ACE0 * 1103515245 + 12345;
	return (lbl_8042ACE0 >> 16) & 0x7FFF;
}
