include flags.mk

OBJ = main.o

LIBS = logger/liblogger.a utils/libutils.a config/libconfig.a http/libhttp.a server/libserver.a daemon/libdaemon.a

all: httpd

httpd: $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group

include libs.mk

clean:
	$(RM) $(OBJ)
	$(MAKE) -C logger clean
	$(MAKE) -C utils clean
	$(MAKE) -C config clean
	$(MAKE) -C http clean
	$(MAKE) -C daemon clean
	$(MAKE) -C server clean

.PHONY: all
