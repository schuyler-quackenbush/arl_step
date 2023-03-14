#include <stdio.h>
#include "license.h"

int main(int argc, char *argv[])
{
	char user_info[LICENSE_LEN];
	int ret_val;

	ret_val = check_license( "./license.txt", user_info );
	printf("License: %d\n", ret_val);
	printf("%s", user_info);

	return 0;
}