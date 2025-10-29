#ifndef _CFG_BT_FILE_H
#define _CFG_BT_FILE_H

/* The record structure of bt nvram file */
typedef struct
{
    unsigned char addr[6];            // BT address
    unsigned char Voice[2];           // Voice setting for SCO connection
    unsigned char Codec[4];           // PCM codec setting
    unsigned char Radio[6];           // RF configuration
    unsigned char Sleep[7];           // Sleep mode configuration
    unsigned char BtFTR[2];           // Other feature setting
    unsigned char TxPWOffset[3];      // TX power channel offset compensation
    unsigned char CoexAdjust[6];      // BT/WIFI coexistence performance adjustment
    unsigned char RadioExt[2];        // RF configuration extended parameters
    unsigned char TxPWOffsetExt[4];   // TX power channel offset compensation
    unsigned char Reserved1[2];       // Reserved
    unsigned char Reserved2[4];       // Reserved
    unsigned char Reserved3[8];       // Reserved
    unsigned char Reserved4[8];       // Reserved
} ap_nvram_btradio_struct;

/* The record size and number of bt nvram file */
#define CFG_FILE_BT_ADDR_REC_SIZE    sizeof(ap_nvram_btradio_struct)
#define CFG_FILE_BT_ADDR_REC_TOTAL   1

#endif
