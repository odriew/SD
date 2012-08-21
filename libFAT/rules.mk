# libFAT rules.mk file.

include $(MAKEDIR)/header.mk

FAT_INCLUDES := -I$(d)/include
CFLAGS_$(d) := $(WIRISH_INCLUDES) $(LIBMAPLE_INCLUDES) $(FAT_INCLUDES)
cppSRCS_$(d) := FAT.cpp

include $(MAKEDIR)/footer.mk
