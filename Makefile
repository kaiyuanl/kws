obj-m += kws.o
kws-objs := init.o master.o worker.o socket.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CFLAGS="-Wno-error=implicit-function-declaration"
	rm *.order *.symvers *.mod.c
	rm .*.*.*
	rm -rf .tmp_versions

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
