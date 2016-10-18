CROSS_COMPILE = mips-openwrt-linux-
CC = gcc

SOURCE = client1.c client2.c
PROJECT_CLIENT1 = client1.o
PROJECT_CLIENT2 = client2.o
PROJECT = $(PROJECT_CLIENT1) $(PROJECT_CLIENT2)

OBJECT_CLIENT1 = client1 
OBJECT_CLIENT2 = client2
OBJECT = $(OBJECT_CLIENT1) $(OBJECT_CLIENT2)
LDFLAGS = -L/home/heenbo/work/openwrt-bb/staging_dir/target-mips_34kc_uClibc-0.9.33.2/usr/lib -lblobmsg_json -ljson-c -lubox -lubus -pthread
CFLAGS = -g -I/home/heenbo/work/openwrt-bb/build_dir/target-mips_34kc_uClibc-0.9.33.2/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.8-linaro_uClibc-0.9.33.2/staging_dir/target-mips_34kc_uClibc-0.9.33.2/usr/include

$(OBJECT) : $(PROJECT)
	${CROSS_COMPILE}${CC} $(PROJECT_CLIENT1) -o $(OBJECT_CLIENT1) ${LDFLAGS}
	${CROSS_COMPILE}${CC} $(PROJECT_CLIENT2) -o $(OBJECT_CLIENT2) ${LDFLAGS}

$(PROJECT) : $(SOURCE)
	${CROSS_COMPILE}${CC} -c $^ ${CFLAGS}

clean : 
	-rm -f $(PROJECT) $(OBJECT) *.o

