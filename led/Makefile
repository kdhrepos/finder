MODULE_NAME = led_driver
KDIR = /lib/modules/`uname -r`/build
obj-m := $(MODULE_NAME).o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
install:
	cp -f $(MODULE_NAME).ko /root/raspberry/nfs
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean