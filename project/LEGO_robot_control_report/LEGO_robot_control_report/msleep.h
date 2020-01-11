void Msleep(int sleeptime)
{
	int i = 1;
	int n = 0;
	clock_t t1;
	clock_t t2;
	int t3 = 0;

	t1 = clock();
	while(t3 < sleeptime){
		n *= i;
		t2 = clock();
		t3 = (int)(1000 * (t2 - t1) / CLOCKS_PER_SEC);
	}
}
