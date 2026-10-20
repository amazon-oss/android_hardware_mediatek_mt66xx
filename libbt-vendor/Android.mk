LOCAL_PATH := $(call my-dir)

ifneq ($(BOARD_MTK_BT_CHIP),)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  mtk.c \
  radiomgr.c \
  radiomod.c \
  bt_drv.c

LOCAL_C_INCLUDES := \
  system/bt/hci/include \
  system/bt/include

# unused-function is misleading here because some functions are used
# via function pointers. So we disable the error for unused-function.
LOCAL_CFLAGS := -Wno-error=unused-function

ifneq ($(filter MTK_MT6628,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_MT6628
endif
ifneq ($(filter MTK_MT6630,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_MT6630
endif
ifneq ($(filter MTK_CONSYS_MT8163,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT8163
endif
ifneq ($(filter MTK_CONSYS_MT8127,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT8127
endif
ifneq ($(filter MTK_CONSYS_MT6582,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6582
endif
ifneq ($(filter MTK_CONSYS_MT6592,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6592
endif
ifneq ($(filter MTK_CONSYS_MT6752,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6752
endif
ifneq ($(filter MTK_CONSYS_MT6735,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6735
endif
ifneq ($(filter MTK_CONSYS_MT6580,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6580
endif
ifneq ($(filter MTK_CONSYS_MT6755,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6755
endif
ifneq ($(filter MTK_CONSYS_MT6797,$(BOARD_MTK_BT_CHIP)),)
LOCAL_CFLAGS += -DMTK_CONSYS_MT6797
endif

# Module definition
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libbt-vendor
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SHARED_LIBRARIES := liblog libcutils

include $(BUILD_SHARED_LIBRARY)

endif
