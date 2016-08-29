#include <stdio.h>
#include "geo.h"

int main()
{
/*
		char a[] = "this is a array";
		char *p;
        p = a;
		printf("%s\n",p);
		return 0;
*/
	//struct geohdr_basic hdr;
	struct geohdr basic *gbhdr;
`	gbhdr->version =4;
	printf("pointer gbhdr: %p\n",gbhdr);
	struct geohdr_common *gchdr = (struct geohdr_common*) ((unsigned char*)gbhdr + 64);
	printf("pointer gchdr: %p\n",gchdr);
	return 0;
	
}
