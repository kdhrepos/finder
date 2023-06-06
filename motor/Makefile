MODULE_NAME = motor_driver
FILE_NAME = motor_test
KDIR = /lib/modules/`uname -r`/build
obj-m := $(MODULE_NAME).o
default: # 모듈 파일 컴파일
	$(MAKE) -C $(KDIR) M=$$PWD modules 
app: # motor test 실행 파일 컴파일
	gcc -o motor motor_test.c
insert: # 모듈 삽입
	insmod $(MODULE_NAME).ko
device: # 장치 파일 생성
	mknod /dev/$(MODULE_NAME) c 236 0 #
chmod: # 유저 레벨에서 제어 위해 장치 파일 권한 변경
	chmod ug+w /dev/$(MODULE_NAME)
delete: # 모듈 삭제
	rmmod $(MODULE_NAME)
clean: # 모듈 파일 삭제
	$(MAKE) -C $(KDIR) M=$$PWD clean
