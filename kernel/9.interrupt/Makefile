# Makefile 4.1.15
ifneq ($(KERNELRELEASE),)

obj-m :=interrupt.o

else

KDIR = /home/mount/linux-imx-rel_imx_4.1.15_2.1.0_ga
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=`pwd` modules clean

endif
