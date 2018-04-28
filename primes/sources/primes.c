#include <stdio.h>
#ifdef __m68k__
#include <mint/osbind.h>
#endif /* __m68k__ */

extern void primes_asm(int product);

#ifndef __m68k__
void primes(int product)
{
	int i;
	for (i = 2; i < product; i++)
	{
		if ((product % i) == 0)			// Teiler gefunden
		{
			printf(" %d *", i);
			primes(product / i);
			return;
		}
	}
	printf(" %d\r\n", product);
}
#endif /* __m68k__ */

int main(int argc, char *argv[])
{
	int product;

#ifdef __m68k__
	while (Cconis()) Cconin();
#endif

	if (argc != 2)
	{
		(void) Cconws("usage: PRIMES.TTP <product to calculate prime factors>\r\n");
		Cconin();

		exit(1);
	}

	product = atoi(argv[1]);
	printf("calculating prime factors of %d\r\n\r\n", product);
#ifdef __m68k__
	primes_asm(product);
#else
	primes(product);
#endif /* __m68k__ */
	printf("\r\nPress any key\r\n");
#ifdef __m68k__
	Cconin();
#endif /* __m68k__ */
	return 0;
}

