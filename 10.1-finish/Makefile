ifneq ($(KERNELRELEASE),)
	obj-m += test.o
test-objs:= geo.o mib.o geo_output.o geo_input.o

else
PWD = $(shell pwd)

#KDIR := /usr/src/$(shell uname -r)
KDIR = /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules #-Wall

clean:
	make -C $(KDIR) M=$(PWD) clean

endif
