ifneq ($(KERNELRELEASE),)
	obj-m += geo_input.o
	#geo.h-objs := file1.o file2.o file 3.o
else
PWD = $(shell pwd)

#KDIR := /usr/src/$(shell uname -r)
KDIR = /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

endif
