ifneq ($(KERNELRELEASE),)
	obj-m += test.o
test-objs:=geo_input.o geo.o

else
PWD = $(shell pwd)

#KDIR := /usr/src/$(shell uname -r)
KDIR = /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

endif
