#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <cutils/properties.h>

/* use nvram */
#include "CFG_BT_File.h"
#include "CFG_BT_Default.h"

#include "bt_hci_bdroid.h"
#include "bt_vendor_lib.h"
#include "bt_mtk.h"


/**************************************************************************
 *                  G L O B A L   V A R I A B L E S                       *
***************************************************************************/

BT_INIT_VAR_T btinit[1];
BT_INIT_CB_T  btinit_ctrl;

static uint16_t wOpCode;
static HCI_CMD_STATUS_T cmd_status;
extern bt_vendor_callbacks_t *bt_vnd_cbacks;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S                 *
***************************************************************************/

static BOOL GORMcmd_HCC_Get_Local_BD_Addr(HC_BT_HDR *);
static VOID GORMevt_HCE_Get_Local_BD_Addr(VOID *);
static BOOL GORMcmd_HCC_Set_Local_BD_Addr(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_LinkKeyType(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_UnitKey(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Encryption(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_PinCodeType(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Voice(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_PCM(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Radio(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_TX_Power_Offset(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Sleep_Timeout(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Coex_Performance_Adjust(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_BT_FTR(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_OSC_Info(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_LPO_Info(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_PTA(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_BLEPTA(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Internal_PTA_1(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_Internal_PTA_2(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_RF_Reg_100(HC_BT_HDR *);
static BOOL GORMcmd_HCC_RESET(HC_BT_HDR *);
static BOOL GORMcmd_HCC_Set_FW_SysLog(HC_BT_HDR *);


static VOID GORMevt_HCE_Common_Complete(VOID *);
static BOOL WriteBDAddrToNvram(UCHAR *);


//===================================================================
// Combo chip
#ifdef MTK_MT6628
HCI_SEQ_T bt_init_preload_script_6628[] =
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_LinkKeyType         }, /*0xFC1B*/
    {  GORMcmd_HCC_Set_UnitKey             }, /*0xFC75*/
    {  GORMcmd_HCC_Set_Encryption          }, /*0xFC76*/
    {  GORMcmd_HCC_Set_PinCodeType         }, /*0x0C0A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_Set_BT_FTR              }, /*0xFC7D*/
    {  GORMcmd_HCC_Set_OSC_Info            }, /*0xFC7B*/
    {  GORMcmd_HCC_Set_LPO_Info            }, /*0xFC7C*/
    {  GORMcmd_HCC_Set_PTA                 }, /*0xFC74*/
    {  GORMcmd_HCC_Set_BLEPTA              }, /*0xFCFC*/
    {  GORMcmd_HCC_RESET                   }, /*0x0C03*/
    {  GORMcmd_HCC_Set_Internal_PTA_1      }, /*0xFCFB*/
    {  GORMcmd_HCC_Set_Internal_PTA_2      }, /*0xFCFB*/
    {  GORMcmd_HCC_Set_RF_Reg_100          }, /*0xFCB0*/
    {  GORMcmd_HCC_Set_FW_SysLog           }, /*0xFCBE*/
    {  0  },
};

HCI_SEQ_T bt_init_postload_script_6628[] =
{
    {  GORMcmd_HCC_Set_Voice               }, /*0x0C26*/
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  0  },
};
#endif

#ifdef MTK_MT6630
HCI_SEQ_T bt_init_preload_script_6630[] =
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_Coex_Performance_Adjust }, /*0xFC22*/
    {  GORMcmd_HCC_Set_FW_SysLog           }, /*0xFCBE*/
    {  0  },
};

HCI_SEQ_T bt_init_postload_script_6630[] =
{
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  0  },
};
#endif

#ifdef MTK_CONSYS_MT8163
HCI_SEQ_T bt_init_preload_script_8163[] =
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    //{  GORMcmd_HCC_Coex_Performance_Adjust }, /*0xFC22*/
    {  GORMcmd_HCC_Set_FW_SysLog           }, /*0xFCBE*/
    {  0  },
};

HCI_SEQ_T bt_init_postload_script_8163[] =
{
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  0  },
};
#endif

#ifdef MTK_CONSYS_MT8127
HCI_SEQ_T bt_init_preload_script_8127[] =
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    //{  GORMcmd_HCC_Coex_Performance_Adjust }, /*0xFC22*/
    {  GORMcmd_HCC_Set_FW_SysLog           }, /*0xFCBE*/
    {  0  },
};

HCI_SEQ_T bt_init_postload_script_8127[] =
{
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  0  },
};
#endif

#if defined(MTK_CONSYS_MT6582) || \
    defined(MTK_CONSYS_MT6592) || \
    defined(MTK_CONSYS_MT6752) || \
    defined(MTK_CONSYS_MT6735) || \
    defined(MTK_CONSYS_MT6580) || \
    defined(MTK_CONSYS_MT6755) || \
    defined(MTK_CONSYS_MT6797)
HCI_SEQ_T bt_init_preload_script_consys[] =
{
    {  GORMcmd_HCC_Get_Local_BD_Addr       }, /*0x1009*/
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_RESET                   }, /*0x0C03*/
    {  GORMcmd_HCC_Set_FW_SysLog           }, /*0xFCBE*/
    {  0  },
};

HCI_SEQ_T bt_init_postload_script_consys[] =
{
    {  0  },
};
#endif

/**************************************************************************
 *                          F U N C T I O N S                             *
***************************************************************************/

static BOOL GORMcmd_HCC_Get_Local_BD_Addr(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x1009;

    p_cmd->len = 3;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 0;

    LOG_DBG("GORMcmd_HCC_Get_Local_BD_Addr\n");

    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Get_Local_BD_Addr);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static VOID GORMevt_HCE_Get_Local_BD_Addr(VOID *p_evt)
{
    HC_BT_HDR *p_buf = (HC_BT_HDR *)p_evt;
    uint8_t *p;
    uint8_t event, status;
    uint16_t opcode;
    BOOL success;

    UCHAR ucBDAddr[6];

    LOG_DBG("GORMevt_HCE_Get_Local_BD_Addr\n");

    p = (uint8_t *)(p_buf + 1);
    event = *p;
    p += 3;
    STREAM_TO_UINT16(opcode, p)
    status = *p++;

    if ((event == 0x0E) && /* Command Complete Event */
        (opcode == 0x1009) && /* OpCode correct */
        (status == 0)) /* Success */
    {
        success = TRUE;
    }
    else {
        success = FALSE;
    }

    ucBDAddr[5] = *p++;
    ucBDAddr[4] = *p++;
    ucBDAddr[3] = *p++;
    ucBDAddr[2] = *p++;
    ucBDAddr[1] = *p++;
    ucBDAddr[0] = *p++;

    LOG_WAN("Retrieve eFUSE address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            ucBDAddr[0], ucBDAddr[1], ucBDAddr[2], ucBDAddr[3], ucBDAddr[4], ucBDAddr[5]);

    /* if we already have a BD address set, don't override it. */
    if (is_memzero(btinit->bt_nvram.fields.addr, BD_ADDR_LEN)) {
        memcpy(btinit->bt_nvram.fields.addr, ucBDAddr, 6);
    } else {
        LOG_WAN("Overriding it and using HAL provided address: %02x-%02x-%02x-%02x-%02x-%02x\n",
                btinit->bt_nvram.fields.addr[0], btinit->bt_nvram.fields.addr[1],
                btinit->bt_nvram.fields.addr[2], btinit->bt_nvram.fields.addr[3],
                btinit->bt_nvram.fields.addr[4], btinit->bt_nvram.fields.addr[5]);
    }

    if (bt_vnd_cbacks) {
        bt_vnd_cbacks->dealloc(p_buf);
    }

    pthread_mutex_lock(&btinit_ctrl.mutex);
    cmd_status = success ? CMD_SUCCESS : CMD_FAIL;
    /* Wake up command tx thread */
    pthread_cond_signal(&btinit_ctrl.cond);
    pthread_mutex_unlock(&btinit_ctrl.mutex);

    return;
}

static BOOL GORMcmd_HCC_Set_Local_BD_Addr(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC1A;

    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.addr[5];
    *p++ = btinit->bt_nvram.fields.addr[4];
    *p++ = btinit->bt_nvram.fields.addr[3];
    *p++ = btinit->bt_nvram.fields.addr[2];
    *p++ = btinit->bt_nvram.fields.addr[1];
    *p++ = btinit->bt_nvram.fields.addr[0];

    LOG_DBG("GORMcmd_HCC_Set_Local_BD_Addr\n");

    LOG_WAN("Write BD address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            btinit->bt_nvram.fields.addr[0], btinit->bt_nvram.fields.addr[1],
            btinit->bt_nvram.fields.addr[2], btinit->bt_nvram.fields.addr[3],
            btinit->bt_nvram.fields.addr[4], btinit->bt_nvram.fields.addr[5]);


    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

#ifdef MTK_MT6628
static BOOL GORMcmd_HCC_Set_LinkKeyType(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC1B;

    p_cmd->len = 4;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 1;

    /* HCI cmd params */
    *p++ = 0x01; /* 00: Unit key; 01: Combination key */

    LOG_DBG("GORMcmd_HCC_Set_LinkKeyType\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_UnitKey(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC75;

    p_cmd->len = 19;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 16;

    /* HCI cmd params */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    LOG_DBG("GORMcmd_HCC_Set_UnitKey\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_Encryption(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC76;

    p_cmd->len = 6;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 3;

    /* HCI cmd params */
    *p++ = 0x00;
    *p++ = 0x02;
    *p++ = 0x10;

    LOG_DBG("GORMcmd_HCC_Set_Encryption\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_PinCodeType(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x0C0A;

    p_cmd->len = 4;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 1;

    /* HCI cmd params */
    *p++ = 0x00; /* 00: Variable PIN; 01: Fixed PIN */

    LOG_DBG("GORMcmd_HCC_Set_PinCodeType\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_Voice(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x0C26;

    p_cmd->len = 5;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 2;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.Voice[0];
    *p++ = btinit->bt_nvram.fields.Voice[1];

    LOG_DBG("GORMcmd_HCC_Set_Voice\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}
#endif

static BOOL GORMcmd_HCC_Set_PCM(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC72;

    p_cmd->len = 7;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 4;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.Codec[0];
    *p++ = btinit->bt_nvram.fields.Codec[1];
    *p++ = btinit->bt_nvram.fields.Codec[2];
    *p++ = btinit->bt_nvram.fields.Codec[3];

    LOG_DBG("GORMcmd_HCC_Set_PCM\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_Radio(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC79;

    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.Radio[0];
    *p++ = btinit->bt_nvram.fields.Radio[1];
    *p++ = btinit->bt_nvram.fields.Radio[2];
    *p++ = btinit->bt_nvram.fields.Radio[3];
    *p++ = btinit->bt_nvram.fields.Radio[4];
    *p++ = btinit->bt_nvram.fields.Radio[5];

    LOG_DBG("GORMcmd_HCC_Set_Radio\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_TX_Power_Offset(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC93;

    p_cmd->len = 6;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 3;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.TxPWOffset[0];
    *p++ = btinit->bt_nvram.fields.TxPWOffset[1];
    *p++ = btinit->bt_nvram.fields.TxPWOffset[2];

    LOG_DBG("GORMcmd_HCC_Set_TX_Power_Offset\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_Sleep_Timeout(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7A;

    p_cmd->len = 10;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 7;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.Sleep[0];
    *p++ = btinit->bt_nvram.fields.Sleep[1];
    *p++ = btinit->bt_nvram.fields.Sleep[2];
    *p++ = btinit->bt_nvram.fields.Sleep[3];
    *p++ = btinit->bt_nvram.fields.Sleep[4];
    *p++ = btinit->bt_nvram.fields.Sleep[5];
    *p++ = btinit->bt_nvram.fields.Sleep[6];

    LOG_DBG("GORMcmd_HCC_Set_Sleep_Timeout\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

//#ifdef MTK_MT6630
#if defined(MTK_MT6630) || defined(MTK_CONSYS_MT8163) || defined(MTK_CONSYS_MT8127)
static BOOL GORMcmd_HCC_Coex_Performance_Adjust(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC22;

    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.CoexAdjust[0];
    *p++ = btinit->bt_nvram.fields.CoexAdjust[1];
    *p++ = btinit->bt_nvram.fields.CoexAdjust[2];
    *p++ = btinit->bt_nvram.fields.CoexAdjust[3];
    *p++ = btinit->bt_nvram.fields.CoexAdjust[4];
    *p++ = btinit->bt_nvram.fields.CoexAdjust[5];

    LOG_DBG("GORMcmd_HCC_Coex_Performance_Adjust\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}
#endif

#ifdef MTK_MT6628
static BOOL GORMcmd_HCC_Set_BT_FTR(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7D;

    p_cmd->len = 5;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 2;

    /* HCI cmd params */
    *p++ = btinit->bt_nvram.fields.BtFTR[0];
    *p++ = btinit->bt_nvram.fields.BtFTR[1];

    LOG_DBG("GORMcmd_HCC_Set_BT_FTR\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_OSC_Info(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7B;

    p_cmd->len = 8;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 5;

    /* HCI cmd params */
    *p++ = 0x01;
    *p++ = 0x01;
    *p++ = 0x14; /* clock drift */
    *p++ = 0x0A; /* clock jitter */
    *p++ = 0x08; /* OSC stable time */

    LOG_DBG("GORMcmd_HCC_Set_OSC_Info\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_LPO_Info(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC7C;

    p_cmd->len = 13;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 10;

    /* HCI cmd params */
    *p++ = 0x01; /* LPO source = external */
    *p++ = 0xFA; /* LPO clock drift = 250ppm */
    *p++ = 0x0A; /* LPO clock jitter = 10us */
    *p++ = 0x02; /* LPO calibration mode = manual mode */
    *p++ = 0x00; /* LPO calibration interval = 10 mins */
    *p++ = 0xA6;
    *p++ = 0x0E;
    *p++ = 0x00;
    *p++ = 0x40; /* LPO calibration cycles = 64 */
    *p++ = 0x00;

    LOG_DBG("GORMcmd_HCC_Set_LPO_Info\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_PTA(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFC74;

    p_cmd->len = 13;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 10;

    /* HCI cmd params */
    *p++ = 0xC9; /* PTA mode register */
    *p++ = 0x8B;
    *p++ = 0xBF;
    *p++ = 0x00;
    *p++ = 0x00; /* PTA time register */
    *p++ = 0x52;
    *p++ = 0x0E;
    *p++ = 0x0E;
    *p++ = 0x1F; /* PTA priority setting */
    *p++ = 0x1B;

    LOG_DBG("GORMcmd_HCC_Set_PTA\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_BLEPTA(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFCFC;

    p_cmd->len = 8;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 5;

    /* HCI cmd params */
    *p++ = 0x16; /* Select BLE PTA command */
    *p++ = 0x0E; /* BLE PTA time setting */
    *p++ = 0x0E;
    *p++ = 0x00; /* BLE PTA priority setting */
    *p++ = 0x07;

    LOG_DBG("GORMcmd_HCC_Set_BLEPTA\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_Internal_PTA_1(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFCFB;

    p_cmd->len = 18;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 15;

    /* HCI cmd params */
    *p++ = 0x00;
    *p++ = 0x01; /* PTA high level Tx */
    *p++ = 0x0F; /* PTA mid level Tx */
    *p++ = 0x0F; /* PTA low level Tx */
    *p++ = 0x01; /* PTA high level Rx */
    *p++ = 0x0F; /* PTA mid level Rx */
    *p++ = 0x0F; /* PTA low level Rx */
    *p++ = 0x01; /* BLE PTA high level Tx */
    *p++ = 0x0F; /* BLE PTA mid level Tx */
    *p++ = 0x0F; /* BLE PTA low level Tx */
    *p++ = 0x01; /* BLE PTA high level Rx */
    *p++ = 0x0F; /* BLE PTA mid level Rx */
    *p++ = 0x0F; /* BLE PTA low level Rx */
    *p++ = 0x02; /* time_r2g */
    *p++ = 0x01;

    LOG_DBG("GORMcmd_HCC_Set_Internal_PTA_1\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_Internal_PTA_2(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFCFB;

    p_cmd->len = 10;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 7;

    /* HCI cmd params */
    *p++ = 0x01;
    *p++ = 0x19; /* wifi20_hb */
    *p++ = 0x19; /* wifi40_hb */
    *p++ = 0x07; /* next RSSI update BT slots */
    *p++ = 0xD0;
    *p++ = 0x00; /* stream identify by host */
    *p++ = 0x01; /* enable auto AFH */

    LOG_DBG("GORMcmd_HCC_Set_Internal_PTA_2\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_RF_Reg_100(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0xFCB0;

    p_cmd->len = 9;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 6;

    /* HCI cmd params */
    *p++ = 0x64;
    *p++ = 0x01;
    *p++ = 0x02;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    LOG_DBG("GORMcmd_HCC_Set_RF_Reg_100\n");

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}
#endif

static BOOL GORMcmd_HCC_RESET(HC_BT_HDR *p_cmd)
{
    uint8_t *p, ret;
    wOpCode = 0x0C03;

    p_cmd->len = 3;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 0;

    LOG_DBG("GORMcmd_HCC_RESET\n");

    if (bt_vnd_cbacks){
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else{
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static BOOL GORMcmd_HCC_Set_FW_SysLog(HC_BT_HDR *p_cmd)
{
    char bt_syslog_val[PROPERTY_VALUE_MAX];
    uint8_t *p, ret;
    wOpCode = 0xFCBE;

    p_cmd->len = 4;
    p = (uint8_t *)(p_cmd + 1);
    UINT16_TO_STREAM(p, wOpCode);
    *p++ = 1;

    /* HCI cmd params */
    if (property_get("persist.bt.syslog.enable", bt_syslog_val, NULL) &&
        0 == strcmp(bt_syslog_val, "1")) {
        *p++ = 0x05;
        LOG_DBG("GORMcmd_HCC_Set_FW_SysLog enable\n");
    }
    else {
        *p++ = 0x00;
        LOG_DBG("GORMcmd_HCC_Set_FW_SysLog disable\n");
    }

    if (bt_vnd_cbacks) {
        ret = bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
    }
    else {
        LOG_ERR("No HCI packet transmit callback\n");
        ret = FALSE;
    }

    return ret;
}

static VOID GORMevt_HCE_Common_Complete(VOID *p_evt)
{
    HC_BT_HDR *p_buf = (HC_BT_HDR *)p_evt;
    uint8_t *p;
    uint8_t event, status;
    uint16_t opcode;
    BOOL success;

    LOG_DBG("GORMevt_HCE_Common_Complete\n");

    p = (uint8_t *)(p_buf + 1);
    event = *p;
    p += 3;
    STREAM_TO_UINT16(opcode, p);
    status = *p;

    if ((event == 0x0E) && /* Command Complete Event */
        (opcode == wOpCode) && /* OpCode correct */
        (status == 0)) /* Success */
    {
        success = TRUE;
    }
    else {
        success = FALSE;
    }

    if (bt_vnd_cbacks) {
        bt_vnd_cbacks->dealloc(p_buf);
    }

    pthread_mutex_lock(&btinit_ctrl.mutex);
    cmd_status = success ? CMD_SUCCESS : CMD_FAIL;
    /* Wake up command tx thread */
    pthread_cond_signal(&btinit_ctrl.cond);
    pthread_mutex_unlock(&btinit_ctrl.mutex);

    return;
}


VOID thread_exit(INT32 signo)
{
    pthread_t tid = pthread_self();
    (void)signo;

    LOG_DBG("Thread %lu is forced to exit...\n", tid);
    if (pthread_mutex_unlock(&btinit_ctrl.mutex) != 0) {
        LOG_ERR("pthread_mutex_unlock error\n");
    }
    pthread_mutexattr_destroy(&btinit_ctrl.attr);
    pthread_mutex_destroy(&btinit_ctrl.mutex);
    pthread_cond_destroy(&btinit_ctrl.cond);
    pthread_exit(0);
}

VOID *GORM_FW_Init_Thread(VOID *ptr)
{
    INT32 i = 0;
    HC_BT_HDR  *p_buf = NULL;
    bt_vendor_op_result_t ret = BT_VND_OP_RESULT_FAIL;
    (void)ptr;

    LOG_DBG("FW init thread starts\n");

    pthread_mutexattr_init(&btinit_ctrl.attr);
    pthread_mutexattr_settype(&btinit_ctrl.attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&btinit_ctrl.mutex, &btinit_ctrl.attr);
    pthread_cond_init(&btinit_ctrl.cond, NULL);

    /* preload init script */
    switch (btinit->chip_id) {
    #ifdef MTK_MT6628
      case 0x6628:
        btinit->cur_script = bt_init_preload_script_6628;
        break;
    #endif
    #ifdef MTK_MT6630
      case 0x6630:
        btinit->cur_script = bt_init_preload_script_6630;
        break;
    #endif
#ifdef MTK_CONSYS_MT8163
      case 0x8163:
        btinit->cur_script = bt_init_preload_script_8163;
        break;
#endif
    #ifdef MTK_CONSYS_MT8127
      case 0x8127:
        btinit->cur_script = bt_init_preload_script_8127;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6582
      case 0x6582:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6592
      case 0x6592:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6752
      case 0x6752:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6735
      case 0x0321:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
      case 0x0335:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
      case 0x0337:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6580
      case 0x6580:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6755
      case 0x6755:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6797
      case 0x6797:
        btinit->cur_script = bt_init_preload_script_consys;
        break;
    #endif
      default:
        LOG_ERR("Unknown combo chip id\n");
        break;
    }

    /* Can not find matching script, simply skip */
    if ((btinit->cur_script) == NULL) {
        LOG_ERR("No matching init script\n");
        goto exit;
    }

    while (btinit->cur_script[i].command_func)
    {
        p_buf = NULL;

        if (bt_vnd_cbacks) {
            p_buf = (HC_BT_HDR *)bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
        }
        else {
            LOG_ERR("No libbt-hci callbacks!\n");
        }

        if (p_buf) {
            p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
            p_buf->offset = 0;
            p_buf->layer_specific = 0;

            cmd_status = CMD_PENDING;

            if (btinit->cur_script[i].command_func(p_buf) == FALSE) {
                LOG_ERR("Send command %d fails\n", i);
                if (bt_vnd_cbacks) {
                    bt_vnd_cbacks->dealloc(p_buf);
                }
                goto exit;
            }
        }
        else {
            LOG_ERR("Alloc command %d buffer fails\n", i);
            goto exit;
        }

        /* Wait for event returned */
        pthread_mutex_lock(&btinit_ctrl.mutex);
        while (cmd_status == CMD_PENDING) {
            pthread_cond_wait(&btinit_ctrl.cond, &btinit_ctrl.mutex);
        }

        if (cmd_status == CMD_FAIL) {
            LOG_ERR("The event of command %d error\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
            goto exit;
        }
        else {
            LOG_DBG("The event of command %d success\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
        }

        i ++;
    }

    ret = BT_VND_OP_RESULT_SUCCESS;

exit:
    pthread_mutexattr_destroy(&btinit_ctrl.attr);
    pthread_mutex_destroy(&btinit_ctrl.mutex);
    pthread_cond_destroy(&btinit_ctrl.cond);

    if (bt_vnd_cbacks) {
        bt_vnd_cbacks->fwcfg_cb(ret);
    }

    btinit_ctrl.worker_thread_running = FALSE;
    return NULL;
}

VOID *GORM_SCO_Init_Thread(VOID *ptr)
{
    INT32 i = 0;
    HC_BT_HDR  *p_buf = NULL;
    bt_vendor_op_result_t ret = BT_VND_OP_RESULT_FAIL;
    (void)ptr;

    LOG_DBG("SCO init thread starts\n");

    pthread_mutexattr_init(&btinit_ctrl.attr);
    pthread_mutexattr_settype(&btinit_ctrl.attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&btinit_ctrl.mutex, &btinit_ctrl.attr);
    pthread_cond_init(&btinit_ctrl.cond, NULL);

    /* postload init script */
    switch (btinit->chip_id) {
    #ifdef MTK_MT6628
      case 0x6628:
        btinit->cur_script = bt_init_postload_script_6628;
        break;
    #endif
    #ifdef MTK_MT6630
      case 0x6630:
        btinit->cur_script = bt_init_postload_script_6630;
        break;
    #endif
#ifdef MTK_CONSYS_MT8163
        case 0x8163:
            btinit->cur_script = bt_init_preload_script_8163;
            break;
#endif
    #ifdef MTK_CONSYS_MT8127
      case 0x8127:
        btinit->cur_script = bt_init_postload_script_8127;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6582
      case 0x6582:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6592
      case 0x6592:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6752
      case 0x6752:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6735
      case 0x0321:
      case 0x0335:
      case 0x0337:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6580
      case 0x6580:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6755
      case 0x6755:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    #ifdef MTK_CONSYS_MT6797
      case 0x6797:
        btinit->cur_script = bt_init_postload_script_consys;
        break;
    #endif
    default:
        LOG_ERR("Unknown combo chip id\n");
        break;
    }

    /* Can not find matching script, simply skip */
    if ((btinit->cur_script) == NULL) {
        LOG_ERR("No matching init script\n");
        goto exit;
    }

    while (btinit->cur_script[i].command_func)
    {
        p_buf = NULL;

        if (bt_vnd_cbacks) {
            p_buf = (HC_BT_HDR *)bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
        }
        else {
            LOG_ERR("No libbt-hci callbacks!\n");
        }

        if (p_buf) {
            p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
            p_buf->offset = 0;
            p_buf->layer_specific = 0;

            cmd_status = CMD_PENDING;

            if (btinit->cur_script[i].command_func(p_buf) == FALSE) {
                LOG_ERR("Send command %d fails\n", i);
                if (bt_vnd_cbacks) {
                    bt_vnd_cbacks->dealloc(p_buf);
                }
                goto exit;
            }
        }
        else {
            LOG_ERR("Alloc command %d buffer fails\n", i);
            goto exit;
        }

        /* Wait for event returned */
        pthread_mutex_lock(&btinit_ctrl.mutex);
        while (cmd_status == CMD_PENDING) {
            pthread_cond_wait(&btinit_ctrl.cond, &btinit_ctrl.mutex);
        }

        if (cmd_status == CMD_FAIL) {
            LOG_ERR("The event of command %d error\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
            goto exit;
        }
        else {
            LOG_DBG("The event of command %d success\n", i);
            pthread_mutex_unlock(&btinit_ctrl.mutex);
        }

        i ++;
    }

    ret = BT_VND_OP_RESULT_SUCCESS;

exit:
    pthread_mutexattr_destroy(&btinit_ctrl.attr);
    pthread_mutex_destroy(&btinit_ctrl.mutex);
    pthread_cond_destroy(&btinit_ctrl.cond);

    if (bt_vnd_cbacks) {
        bt_vnd_cbacks->scocfg_cb(ret);
    }

    btinit_ctrl.worker_thread_running = FALSE;
    return NULL;
}
