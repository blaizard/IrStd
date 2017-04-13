ROOT_IRSTD:=$(MODULE_PATH)

include $(ROOT_IRSTD)Logger/config.mk
include $(ROOT_IRSTD)Memory/config.mk
include $(ROOT_IRSTD)Exception/config.mk

irstd := \
	process_irstdlogger \
	process_irstdmemory \
	process_irstdexception

