MODULE_NAME = led_driver
KDIR = /lib/modules/`uname -r`/build
obj-m := $(MODULE_NAME).o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
insert:
	insmod $(MODULE_NAME).ko
device:
	mknod /dev/$(MODULE_NAME) c 237 0
chmod:
	chmod ug+w /dev/$(MODULE_NAME)
delete:
	rmmod $(MODULE_NAME)
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean