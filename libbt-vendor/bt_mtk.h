
#ifndef _BT_MTK_H
#define _BT_MTK_H

#include "bt_hci_bdroid.h"
#include "bt_vendor_lib.h"
#include "CFG_BT_File.h"
#include "os_dep.h"


#define HCI_CMD_MAX_SIZE        251
#define BD_ADDR_LEN             6

/********************************************************************************
** Macros to get and put bytes to and from a stream (Little Endian format).
*/
#define UINT16_TO_STREAM(p, u16) {*(p)++ = (UINT8)(u16); *(p)++ = (UINT8)((u16) >> 8);}
#define STREAM_TO_UINT16(u16, p) {u16 = ((UINT16)(*(p)) + (((UINT16)(*((p) + 1))) << 8)); (p) += 2;}


/********************************************************************************
** Structure Definitions
*/
typedef enum {
  CMD_SUCCESS,
  CMD_FAIL,
  CMD_PENDING,
} HCI_CMD_STATUS_T;

typedef union {
  ap_nvram_btradio_struct fields;
  unsigned char raw[sizeof(ap_nvram_btradio_struct)];
} BT_NVRAM_DATA_T;

typedef BOOL (*HCI_CMD_FUNC_T) (HC_BT_HDR *);
typedef struct {
  HCI_CMD_FUNC_T command_func;
} HCI_SEQ_T;

typedef struct {
  UINT32 chip_id;
  BT_NVRAM_DATA_T bt_nvram;
  HCI_SEQ_T *cur_script;
} BT_INIT_VAR_T;

/* Thread control block for Controller initialize */
typedef struct {
  pthread_t worker_thread;
  pthread_mutex_t mutex;
  pthread_mutexattr_t attr;
  pthread_cond_t cond;
  BOOL worker_thread_running;
} BT_INIT_CB_T;


/********************************************************************************
** Function Declaration
*/
void store_bdaddr(const unsigned char *addr);
void set_callbacks(const bt_vendor_callbacks_t* p_cb);
void clean_callbacks(void);
int init_uart(void);
void close_uart(void);
int mtk_fw_cfg(void);
int mtk_sco_cfg(void);
int mtk_prepare_off(void);
int mtk_set_fw_assert(uint8_t reason);
int mtk_set_psm_control(bool enable);
void clean_resource(void);

BOOL is_memzero(unsigned char *buf, int size);

#endif
