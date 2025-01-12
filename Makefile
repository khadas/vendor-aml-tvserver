################################################################################
#
# aml_tvserver
#
################################################################################
OUT_DIR ?= .
LOCAL_PATH = $(shell pwd)
LDFLAGS += -Wl,--no-as-needed -lstdc++ -lpthread -lz -ldl -lrt -L$(STAGING_DIR)/usr/lib
CFLAGS += -Wall -Wno-unknown-pragmas -Wno-format -Wno-format-security\
          -O3 -fexceptions -fnon-call-exceptions -D_GNU_SOURCE \
          -I$(STAGING_DIR)/usr/include -DHAVE_AUDIO

LDFLAGS += -lbinder -llog -lubootenv

LDFLAGS += -L $(OUT_DIR)/

################################################################################
# libtv.so - src files
################################################################################
tv_SRCS  = \
	$(LOCAL_PATH)/libtv/tvutils/CFile.cpp \
	$(LOCAL_PATH)/libtv/tvutils/ConfigFile.cpp \
	$(LOCAL_PATH)/libtv/tvutils/CSerialPort.cpp \
	$(LOCAL_PATH)/libtv/tvutils/TvConfigManager.cpp \
	$(LOCAL_PATH)/libtv/tvutils/tvutils.cpp \
	$(LOCAL_PATH)/libtv/tvutils/zepoll.cpp \
	$(LOCAL_PATH)/libtv/tvutils/CTvLog.cpp \
	$(LOCAL_PATH)/libtv/tvutils/CMsgQueue.cpp \
	$(LOCAL_PATH)/libtv/CHDMIRxManager.cpp \
	$(LOCAL_PATH)/libtv/CTv.cpp \
	$(LOCAL_PATH)/libtv/CTvin.cpp \
	$(LOCAL_PATH)/libtv/CTvDevicesPollDetect.cpp \
	$(LOCAL_PATH)/libtv/CTvAudio.cpp \
	$(LOCAL_PATH)/libtv/CAmVideo.cpp \
	$(LOCAL_PATH)/libtv/CVpp.cpp \
	$(NULL)

################################################################################
# libtvclient.so - src files
################################################################################
tvclient_SRCS  = \
	$(LOCAL_PATH)/client/TvClient.cpp \
	$(LOCAL_PATH)/client/CTvClientLog.cpp \
	$(LOCAL_PATH)/client/TvClientWrapper.cpp \
	$(NULL)

tvclient_HEADERS = \
	$(LOCAL_PATH)/client/include \
	$(NULL)
################################################################################
# tvservice - src files
################################################################################
tvservice_SRCS  = \
	$(LOCAL_PATH)/service/main_tvservice.cpp \
	$(LOCAL_PATH)/service/TvService.cpp \
	$(NULL)

################################################################################
# tvtest - src files
################################################################################
tvtest_SRCS  = \
	$(LOCAL_PATH)/test/main_tvtest.c \
	$(NULL)

# ---------------------------------------------------------------------
#  Build rules
BUILD_TARGETS = libtvclient.so libtv.so tvservice tvtest
BUILD_TARGETS_FULLPATH := $(patsubst %, $(OUT_DIR)/%, $(BUILD_TARGETS))

.PHONY: all install clean

libtvclient.so: $(tvclient_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC -I$(tvclient_HEADERS) \
	-o $(OUT_DIR)/$@ $^ $(LDLIBS)

libtv.so: $(tv_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC -I$(tvclient_HEADERS) \
	-I$(LOCAL_PATH)/libtv/tvutils -laudio_client -o $(OUT_DIR)/$@ $^ $(LDLIBS)

tvservice: $(tvservice_SRCS) libtv.so
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) -I$(tvclient_HEADERS) \
	-I$(LOCAL_PATH)/libtv -I$(LOCAL_PATH)/libtv/tvutils \
	-L$(LOCAL_PATH) -ltv -laudio_client -o $(OUT_DIR)/$@ $(filter-out %.so,$^) $(LDLIBS)

tvtest: $(tvtest_SRCS) libtvclient.so
	$(CC) $(CFLAGS) -I$(tvclient_HEADERS) -L$(LOCAL_PATH) \
	-ltvclient $(LDFLAGS) -o $(OUT_DIR)/$@ $(filter-out %.so,$^) $(LDLIBS)

all: $(BUILD_TARGETS)

clean:
	rm -f $(OUT_DIR)/*.o $(BUILD_TARGETS_FULLPATH)
	rm -rf $(STAGING_DIR)/usr/include/tvclient
	rm -rf $(STAGING_DIR)/usr/lib/libtvclient.so
	rm -rf $(TARGET_DIR)/usr/include/tvclient
	rm -rf $(TARGET_DIR)/usr/lib/libtvclient.so
	rm -rf $(TARGET_DIR)/usr/lib/libtv.so
	rm -rf $(TARGET_DIR)/usr/bin/tvtest
	rm -rf $(TARGET_DIR)/usr/bin/tvservice

install:
	install -m 0644 $(OUT_DIR)/libtvclient.so $(TARGET_DIR)/usr/lib
	install -m 0644 $(OUT_DIR)/libtv.so $(TARGET_DIR)/usr/lib/
	install -m 0755 $(OUT_DIR)/tvservice $(TARGET_DIR)/usr/bin/
	install -m 0755 $(OUT_DIR)/tvtest $(TARGET_DIR)/usr/bin/
