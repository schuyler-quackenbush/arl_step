#include <stdio.h>

int verify_license( char *ifile );

int main(int argc, char *argv[])
{
	int ret_val;

	ret_val = verify_license( "./license.txt" );
	printf("License: %d\n", ret_val);
	return 0;
}