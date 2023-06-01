MODULE_NAME = motor_driver
FILE_NAME = motor_test
KDIR = /lib/modules/`uname -r`/build
obj-m := $(MODULE_NAME).o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
app:
	gcc -o motor motor_test.c
insert:
	insmod $(MODULE_NAME).ko
device:
	mknod /dev/$(MODULE_NAME) c 236 0
chmod:
	chmod ug+w /dev/$(MODULE_NAME)
delete:
	rmmod $(MODULE_NAME)
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
