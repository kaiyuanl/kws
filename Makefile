TARGET=kws

obj-m += ${TARGET}.o
${TARGET}-objs := init.o master.o worker.o socket.o queue.o request.o

BUILD=/lib/modules/$(shell uname -r)/build

CFLAG="-D KWS_DEBUG"

all:
	make -C $(BUILD) M=$(PWD) -S modules
	rm *.order *.symvers *.mod.c
	rm .*.*.*
	rm -rf .tmp_versions

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
