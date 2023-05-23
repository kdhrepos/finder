MODULE_NAME = motor_driver
FILE_NAME = motor_test
KDIR = /lib/modules/`uname -r`/build
obj-m := $(MODULE_NAME).o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
app:
	gcc -o motor motor_test.c
install:
	cp -f $(MODULE_NAME).ko /root/raspberry/nfs
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean