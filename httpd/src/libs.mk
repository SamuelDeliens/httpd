SRC_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

logger/liblogger.a:
	$(MAKE) -C $(SRC_DIR)logger

utils/libutils.a:
	$(MAKE) -C $(SRC_DIR)utils

config/libconfig.a:
	$(MAKE) -C $(SRC_DIR)config

http/libhttp.a:
	$(MAKE) -C $(SRC_DIR)http

server/libserver.a:
	$(MAKE) -C $(SRC_DIR)server

daemon/libdaemon.a:
	$(MAKE) -C $(SRC_DIR)daemon