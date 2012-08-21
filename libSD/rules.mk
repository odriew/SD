# libSD rules.mk file.

include $(MAKEDIR)/header.mk

SD_INCLUDES := -I$(d)/include
CFLAGS_$(d) := $(WIRISH_INCLUDES) $(LIBMAPLE_INCLUDES) $(SD_INCLUDES)
cppSRCS_$(d) := SD.cpp

include $(MAKEDIR)/footer.mk
