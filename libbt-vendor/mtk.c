#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <cutils/properties.h>

/* include nvram defs to do less work but we don't use libnvram */
#include "CFG_BT_File.h"
#include "CFG_BT_Default.h"

#include "bt_mtk.h"
#include "cust_bt.h"

/**************************************************************************
 *                    G L O B A L   D E F I N E S                         *
***************************************************************************/

#define COMBO_IOC_MAGIC         0xb0
#define COMBO_IOCTL_FW_ASSERT   _IOWR(COMBO_IOC_MAGIC, 0, int)
#define COMBO_IOCTL_BT_SET_PSM  _IOWR(COMBO_IOC_MAGIC, 1, bool)
/**************************************************************************
 *                  G L O B A L   V A R I A B L E S                       *
***************************************************************************/

static unsigned char preset_bdaddr[BD_ADDR_LEN];
static ap_nvram_btradio_struct ucNvRamData;
const bt_vendor_callbacks_t *bt_vnd_cbacks = NULL;
static int  bt_fd = -1;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S                 *
***************************************************************************/

extern BOOL BT_InitDevice(
    UINT32  chipId,
    ap_nvram_btradio_struct *pucNvRamData
);

extern BOOL BT_InitSCO(VOID);
extern BOOL BT_DeinitDevice(VOID);
extern VOID BT_Cleanup(VOID);

/**************************************************************************
 *                          F U N C T I O N S                             *
***************************************************************************/

BOOL is_memzero(unsigned char *buf, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        if (*(buf+i) != 0) return FALSE;
    }
    return TRUE;
}

/* Store the preset BD address from BT HAL */
void store_bdaddr(const unsigned char *addr)
{
    memcpy(preset_bdaddr, addr, BD_ADDR_LEN);
}

/* Register callback functions to libbt-hci.so */
void set_callbacks(const bt_vendor_callbacks_t *p_cb)
{
    bt_vnd_cbacks = p_cb;
}

/* Cleanup callback functions previously registered */
void clean_callbacks(void)
{
    bt_vnd_cbacks = NULL;
}

/* Initialize UART port */
int init_uart(void)
{
    LOG_TRC();

    bt_fd = open(CUST_BT_SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (bt_fd < 0) {
        LOG_ERR("Can't open serial port\n");
        return -1;
    }

    return bt_fd;
}

/* Close UART port previously opened */
void close_uart(void)
{
    if (bt_fd >= 0) close(bt_fd);
    bt_fd = -1;
}


static int bt_get_combo_id(unsigned int *pChipId)
{
    int  chipId_ready_retry = 0;
    char chipId_val[PROPERTY_VALUE_MAX];

    do {
        if (property_get("persist.mtk.wcn.combo.chipid", chipId_val, NULL) &&
            0 != strcmp(chipId_val, "-1")) {
            *pChipId = (unsigned int)strtoul(chipId_val, NULL, 16);
            break;
        }
        else {
            chipId_ready_retry ++;
            usleep(500000);
        }
    } while(chipId_ready_retry < 10);

    LOG_DBG("Get combo chip id retry %d\n", chipId_ready_retry);
    if (chipId_ready_retry >= 10) {
        LOG_DBG("Invalid combo chip id!\n");
        return -1;
    }
    else {
        LOG_DBG("Combo chip id %x\n", *pChipId);
        return 0;
    }
}

/* MTK specific chip initialize process */
int mtk_fw_cfg(void)
{
    unsigned int chipId = 0;

    LOG_TRC();

    /* Get combo chip id */
    if (bt_get_combo_id(&chipId) < 0) {
        LOG_ERR("Get combo chip id fails\n");
        return -1;
    }

    switch (chipId) {
      case 0x6628:
        /* Use MT6628 default value */
        memcpy(&ucNvRamData, &stBtDefault_6628, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6630:
        /* Use MT6630 default value */
        memcpy(&ucNvRamData, &stBtDefault_6630, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x8163:
        /* NVRAM is MT8163 default */
        memcpy(&ucNvRamData, &stBtDefault_8163, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x8127:
        /* Use MT8127 default value */
        memcpy(&ucNvRamData, &stBtDefault_8127, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6582:
        /* Use MT6582 default value */
        memcpy(&ucNvRamData, &stBtDefault_6582, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6592:
        /* Use MT6592 default value */
        memcpy(&ucNvRamData, &stBtDefault_6592, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6752:
        /* Use MT6752 default value */
        memcpy(&ucNvRamData, &stBtDefault_6752, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x0321:
        /* Use MT6735 default value */
        memcpy(&ucNvRamData, &stBtDefault_6735, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x0335:
        /* Use MT6735m default value */
        memcpy(&ucNvRamData, &stBtDefault_6735m, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x0337:
        /* Use MT6735m default value */
        memcpy(&ucNvRamData, &stBtDefault_6753, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6580:
        /* Use MT6580 default value */
        memcpy(&ucNvRamData, &stBtDefault_6580, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6755:
        /* Use MT6755 default value */
        memcpy(&ucNvRamData, &stBtDefault_6755, sizeof(ap_nvram_btradio_struct));
        break;
      case 0x6797:
        /* Use MT6797 default value */
        memcpy(&ucNvRamData, &stBtDefault_6797, sizeof(ap_nvram_btradio_struct));
        break;
      default:
        LOG_ERR("Unknown combo chip id\n");
        return -1;
    }

    /* Try to use the preset BD address from BT HAL if it is valid */
    if (!is_memzero(preset_bdaddr, BD_ADDR_LEN))
        memcpy(ucNvRamData.addr, preset_bdaddr, BD_ADDR_LEN);

    return (BT_InitDevice(
              chipId,
              &ucNvRamData) == TRUE ? 0 : -1);
}

/* MTK specific SCO/PCM configuration */
int mtk_sco_cfg(void)
{
    return (BT_InitSCO() == TRUE ? 0 : -1);
}

/* MTK specific deinitialize process */
int mtk_prepare_off(void)
{
    /*
    * On KK, BlueDroid adds BT_VND_OP_EPILOG procedure when BT disable:
    *   - 1. BT_VND_OP_EPILOG;
    *   - 2. In vendor epilog_cb, send EXIT event to bt_hc_worker_thread;
    *   - 3. Wait for bt_hc_worker_thread exit;
    *   - 4. userial close;
    *   - 5. vendor cleanup;
    *   - 6. Set power off.
    * On L, the disable flow is modified as below:
    *   - 1. userial Rx thread exit;
    *   - 2. BT_VND_OP_EPILOG;
    *   - 3. Write reactor->event_fd to trigger bt_hc_worker_thread exit
    *        (not wait to vendor epilog_cb and do nothing in epilog_cb);
    *   - 4. Wait for bt_hc_worker_thread exit;
    *   - 5. userial close;
    *   - 6. Set power off;
    *   - 7. vendor cleanup.
    *
    * It seems BlueDroid does not expect Tx/Rx interaction with chip during
    * BT_VND_OP_EPILOG procedure, and also does not need to do it in a new
    * thread context (NE may occur in __pthread_start if bt_hc_worker_thread
    * has already exited).
    * So BT_VND_OP_EPILOG procedure may be not for chip deinitialization,
    * do nothing, just notify success.
    *
    * [FIXME!!]How to do if chip deinit is needed?
    */
    //return (BT_DeinitDevice() == TRUE ? 0 : -1);
    if (bt_vnd_cbacks) {
        bt_vnd_cbacks->epilog_cb(BT_VND_OP_RESULT_SUCCESS);
    }
    return 0;
}

int mtk_set_fw_assert(uint8_t reason)
{
    if (bt_fd < 0) {
        LOG_ERR("Invalid bt fd!\n");
        return -1;
    }
    if (ioctl(bt_fd, COMBO_IOCTL_FW_ASSERT, reason) < 0) {
        LOG_ERR("Set COMBO FW ASSERT fails, %s, errno[%d]\n", strerror(errno), errno);
        return errno;
    }
    return 0;
}

int mtk_set_psm_control(bool enable)
{
    if (bt_fd < 0) {
        LOG_ERR("Invalid bt fd!\n");
        return -1;
    }
    if (ioctl(bt_fd, COMBO_IOCTL_BT_SET_PSM, enable) < 0) {
        LOG_ERR("Set COMBO_IOCTL_BT_SET_PSM fails enable = %d, %s, errno[%d]\n", enable, strerror(errno), errno);
        return errno;
    }
    return 0;
}

/* Cleanup driver resources, e.g thread exit */
void clean_resource(void)
{
    BT_Cleanup();
}
