/* alg.c -- the 3A library majestic is to the ISP library, and the
 * attribute calls it answers, for when nothing else is.
 *
 * libisp_XM530 takes an AE and an AWB library through
 * XM_MPI_ISP_AELibRegCallBack and XM_MPI_ISP_AWBLibRegCallBack, and the
 * sensor libraries register with them through XM_MPI_AE/AWB_SensorRegCallBack
 * (and refuse to start if that fails). majestic is that library. Here it is
 * empty on purpose: the open AE and AWB in libisp_XM530 (LIBISP_OPEN_AE=1,
 * LIBISP_OPEN_AWB=1) take the sensor's callbacks themselves and do the work.
 * What libisp needs of these is that they exist: init and run functions
 * that answer, the run ones failing, so a frame the open 3A does not take
 * changes nothing.
 *
 * The attribute calls (exposure, white balance, colour matrix and the state
 * queries) are a store: Get gives what Set gave. The exposure attributes
 * are the ones that matter -- the open AE reads its target, ranges and
 * weights from XM_MPI_ISP_GetExposureAttr -- and start as automatic, every
 * zone weighted 1, the rest 0 (the sensor's own limits and target).
 *
 * SPDX-License-Identifier: MIT
 */
#include <string.h>

#include "mpi_ae.h"
#include "mpi_awb.h"

#define API __attribute__((visibility("default")))

/* ---- the AE and AWB libraries libisp registers ---- */

static XM_S32 ae_init(ISP_DEV d, XM_S32 h, const ISP_AE_PARAM_S *p) {
  (void)d, (void)h, (void)p;
  return XM_SUCCESS;
}
static XM_S32 ae_run(ISP_DEV d, XM_S32 h, const ISP_AE_INFO_S *i,
                     ISP_AE_RESULT_S *r, XM_S32 rsv) {
  (void)d, (void)h, (void)i, (void)r, (void)rsv;
  return XM_FAILURE;
}
static XM_S32 ae_ctrl(ISP_DEV d, XM_S32 h, XM_U32 cmd, XM_VOID *v) {
  (void)d, (void)h, (void)cmd, (void)v;
  return XM_SUCCESS;
}
static XM_S32 ae_exit(ISP_DEV d, XM_S32 h) {
  (void)d, (void)h;
  return XM_SUCCESS;
}
static XM_S32 awb_init(XM_S32 h, ISP_AWB_RESULT_BK_S *const p, XM_U32 w,
                       XM_U32 ht) {
  (void)h, (void)p, (void)w, (void)ht;
  return XM_SUCCESS;
}
static XM_S32 awb_run(XM_S32 h, const ISP_AWB_INFO_S *i, ISP_AWB_RESULT_S *r,
                      XM_S32 rsv) {
  (void)h, (void)i, (void)r, (void)rsv;
  return XM_FAILURE;
}
static XM_S32 awb_ctrl(XM_S32 h, XM_U32 cmd, XM_VOID *v) {
  (void)h, (void)cmd, (void)v;
  return XM_SUCCESS;
}
static XM_S32 awb_exit(XM_S32 h) {
  (void)h;
  return XM_SUCCESS;
}

API XM_S32 XM_MPI_ISP_AELibRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib,
                                       ISP_AE_REGISTER_S *pstRegister) {
  (void)IspDev, (void)pstAeLib;
  if (!pstRegister)
    return XM_FAILURE;
  pstRegister->stAeExpFunc.pfn_ae_init = ae_init;
  pstRegister->stAeExpFunc.pfn_ae_run = ae_run;
  pstRegister->stAeExpFunc.pfn_ae_ctrl = ae_ctrl;
  pstRegister->stAeExpFunc.pfn_ae_exit = ae_exit;
  return XM_SUCCESS;
}

API XM_S32 XM_MPI_ISP_AWBLibRegCallBack(ALG_LIB_S *pstAWBLib,
                                        ISP_AWB_REGISTER_S *pstRegister) {
  (void)pstAWBLib;
  if (!pstRegister)
    return XM_FAILURE;
  pstRegister->stAwbExpFunc.pfn_awb_init = awb_init;
  pstRegister->stAwbExpFunc.pfn_awb_run = awb_run;
  pstRegister->stAwbExpFunc.pfn_awb_ctrl = awb_ctrl;
  pstRegister->stAwbExpFunc.pfn_awb_exit = awb_exit;
  return XM_SUCCESS;
}

/* The sensor's half: accepted; the open 3A has the callbacks already. */
API XM_S32 XM_MPI_AE_SensorRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib,
                                       SENSOR_ID SensorId,
                                       AE_SENSOR_REGISTER_S *pstRegister) {
  (void)IspDev, (void)pstAeLib, (void)SensorId;
  return pstRegister ? XM_SUCCESS : XM_FAILURE;
}
API XM_S32 XM_MPI_AE_SensorUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib,
                                         SENSOR_ID SensorId) {
  (void)IspDev, (void)pstAeLib, (void)SensorId;
  return XM_SUCCESS;
}
API XM_S32 XM_MPI_AWB_SensorRegCallBack(ALG_LIB_S *pstAwbLib,
                                        SENSOR_ID SensorId,
                                        AWB_SENSOR_REGISTER_S *pstRegister) {
  (void)pstAwbLib, (void)SensorId;
  return pstRegister ? XM_SUCCESS : XM_FAILURE;
}
API XM_S32 XM_MPI_AWB_SensorUnRegCallBack(ALG_LIB_S *pstAwbLib,
                                          SENSOR_ID SensorId) {
  (void)pstAwbLib, (void)SensorId;
  return XM_SUCCESS;
}
API XM_S32 XM_MPI_AE_SetFrameRate(ISP_DEV IspDev, XM_U8 u8Fps) {
  (void)IspDev, (void)u8Fps;
  return XM_SUCCESS;
}

/* ---- the attributes: a store ---- */

static ISP_EXPOSURE_ATTR_S exposure;
static int exposure_set;
static ISP_WB_ATTR_S wb;
static ISP_COLORMATRIX_ATTR_S ccm;
static ISP_AWB_CALIBRATION_V2_S wbcal;
static ISP_EXP_STA_INFO_S expsta;

#define STORE_GET(var, out)                                                   \
  do {                                                                         \
    if (!(out))                                                                \
      return XM_FAILURE;                                                       \
    memcpy((out), &(var), sizeof(var));                                        \
    return XM_SUCCESS;                                                         \
  } while (0)
#define STORE_SET(var, in)                                                    \
  do {                                                                         \
    if (!(in))                                                                 \
      return XM_FAILURE;                                                       \
    memcpy(&(var), (in), sizeof(var));                                         \
    return XM_SUCCESS;                                                         \
  } while (0)

API XM_S32 XM_MPI_ISP_GetExposureAttr(ISP_DEV IspDev,
                                      ISP_EXPOSURE_ATTR_S *pstExpAttr) {
  (void)IspDev;
  if (!exposure_set) {
    memset(&exposure, 0, sizeof(exposure));
    exposure.enOpType = OP_TYPE_AUTO;
    memset(exposure.stAuto.au8Weight, 1, sizeof(exposure.stAuto.au8Weight));
    exposure_set = 1;
  }
  STORE_GET(exposure, pstExpAttr);
}
API XM_S32 XM_MPI_ISP_SetExposureAttr(ISP_DEV IspDev,
                                      const ISP_EXPOSURE_ATTR_S *pstExpAttr) {
  (void)IspDev;
  exposure_set = 1;
  STORE_SET(exposure, pstExpAttr);
}
API XM_S32 XM_MPI_ISP_GetExpStaInfo(ISP_DEV IspDev,
                                    ISP_EXP_STA_INFO_S *pstExpStatistic) {
  (void)IspDev;
  STORE_GET(expsta, pstExpStatistic);
}
API XM_S32 XM_MPI_ISP_SetExpStaInfo(ISP_DEV IspDev,
                                    ISP_EXP_STA_INFO_S *pstExpStatistic) {
  (void)IspDev;
  STORE_SET(expsta, pstExpStatistic);
}
API XM_S32 XM_MPI_ISP_GetStatisticsAE(ISP_DEV IspDev, ISP_AE_STAT_S *pstAeStat) {
  (void)IspDev;
  if (!pstAeStat)
    return XM_FAILURE;
  memset(pstAeStat, 0, sizeof(*pstAeStat));
  return XM_SUCCESS;
}
API XM_S32 XM_MPI_ISP_QueryInnerStateInfo(ISP_DEV IspDev,
                                          ISP_INNER_STATE_INFO_S *p) {
  (void)IspDev;
  if (!p)
    return XM_FAILURE;
  memset(p, 0, sizeof(*p));
  return XM_SUCCESS;
}
API XM_S32 XM_MPI_ISP_GetWBAttr(ISP_WB_ATTR_S *pstWBAttr) {
  STORE_GET(wb, pstWBAttr);
}
API XM_S32 XM_MPI_ISP_SetWBAttr(const ISP_WB_ATTR_S *pstWBAttr) {
  STORE_SET(wb, pstWBAttr);
}
API XM_S32 XM_MPI_ISP_GetCCMAttr(ISP_COLORMATRIX_ATTR_S *pstCCMAttr) {
  STORE_GET(ccm, pstCCMAttr);
}
API XM_S32 XM_MPI_ISP_SetCCMAttr(const ISP_COLORMATRIX_ATTR_S *pstCCMAttr) {
  STORE_SET(ccm, pstCCMAttr);
}
API XM_S32 XM_MPI_ISP_GetWBCalAttr(ISP_AWB_CALIBRATION_V2_S *pstWBCalAttr) {
  STORE_GET(wbcal, pstWBCalAttr);
}
API XM_S32 XM_MPI_ISP_SetWBCalAttr(const ISP_AWB_CALIBRATION_V2_S *pstWBCalAttr) {
  STORE_SET(wbcal, pstWBCalAttr);
}
API XM_S32 XM_MPI_ISP_QueryWBInfo(ISP_WB_INFO_S *pstWBInfo) {
  if (!pstWBInfo)
    return XM_FAILURE;
  memset(pstWBInfo, 0, sizeof(*pstWBInfo));
  return XM_SUCCESS;
}
