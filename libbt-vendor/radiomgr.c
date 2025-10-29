#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "bt_mtk.h"


/**************************************************************************
 *                  G L O B A L   V A R I A B L E S                       *
***************************************************************************/

extern BT_INIT_VAR_T btinit[1];
extern BT_INIT_CB_T btinit_ctrl;

/**************************************************************************
 *                          F U N C T I O N S                             *
***************************************************************************/

extern VOID *GORM_FW_Init_Thread(VOID *ptr);
extern VOID *GORM_SCO_Init_Thread(VOID *ptr);
extern VOID thread_exit(INT32 signo);


BOOL BT_InitDevice(
    UINT32  chipId,
    ap_nvram_btradio_struct *pucNvRamData
    )
{
    LOG_DBG("BT_InitDevice\n");

    memset(btinit, 0, sizeof(BT_INIT_VAR_T));
    btinit_ctrl.worker_thread_running = FALSE;

    btinit->chip_id = chipId;
    /* Copy configuration data */
    memcpy(btinit->bt_nvram.raw, pucNvRamData, sizeof(ap_nvram_btradio_struct));

    if (SIG_ERR == signal(SIGRTMIN, thread_exit)) {
        LOG_ERR("Register signal handler fails errno(%d)\n", errno);
    }

    if (pthread_create(&btinit_ctrl.worker_thread, NULL, \
          GORM_FW_Init_Thread, NULL) != 0) {
        LOG_ERR("Create FW init thread fails\n");
        return FALSE;
    }
    else {
        btinit_ctrl.worker_thread_running = TRUE;
        return TRUE;
    }
}

BOOL BT_InitSCO(VOID)
{
    LOG_DBG("BT_InitSCO\n");

    if (pthread_create(&btinit_ctrl.worker_thread, NULL, \
          GORM_SCO_Init_Thread, NULL) != 0) {
        LOG_ERR("Create SCO init thread fails\n");
        return FALSE;
    }
    else {
        btinit_ctrl.worker_thread_running = TRUE;
        return TRUE;
    }
}

BOOL BT_DeinitDevice(VOID)
{
    LOG_DBG("BT_DeinitDevice\n");
    /* Do nothing on combo chip */
    return TRUE;
}

VOID BT_Cleanup(VOID)
{
    /* Cancel any remaining running thread */
    if (btinit_ctrl.worker_thread_running) {
        pthread_kill(btinit_ctrl.worker_thread, SIGRTMIN);
        /* Wait until thread exit */
        pthread_join(btinit_ctrl.worker_thread, NULL);
        btinit_ctrl.worker_thread_running = FALSE;
    }
    if (SIG_ERR == signal(SIGRTMIN, SIG_DFL)) {
        LOG_ERR("Restore signal handler fails errno(%d)\n", errno);
    }
    return;
}
