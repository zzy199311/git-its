#include <stdio.h>
#include <string.h>

int main()
{
int i;
unsigned char mac[6] = {0xaf,0x01,0x02,0x04,0x0f,0xff};
unsigned char gnaddr[6];
memcpy(gnaddr, mac, 6);
/*
mac[0] = 0xa3;
mac[1] = 0x01;
mac[2] = 0x02;
mac[3] = 0x03;
mac[4] = 0x04;
mac[5] = 0xaf;
*/
for(i = 0; i<6; i++)
{
printf("%02x:", gnaddr[i]);
}
}
