/* camera.c -- the camera_* and Camera* functions libispapp_XM530 calls, which
 * majestic defines: none of them is needed to bring a picture up.
 *
 * libispapp calls five outside its UART tuning tool (com.c): camera_init,
 * camera_set_vstd, camera_set_ircut_mode, camera_set_color and
 * camera_set_dnc_mode, once each at start. The rest answer that tool. Every
 * one is here because musl binds a library's symbols at load: one missing
 * and libispapp does not load. Each answers success and changes nothing;
 * the getters leave what they are given as it was. An application that
 * wants one to act (day and night, IR-cut, colour) defines it itself:
 * these are weak, and an application's own definition wins.
 *
 * Generated from libraries/isp/include/xm/Camera.h by the prototypes there.
 *
 * SPDX-License-Identifier: MIT
 */
#include "Camera.h"

#define WEAK __attribute__((weak, visibility("default")))

WEAK int CameraGetNFLevel(int daynight, int *pNrLevel) {
  (void)daynight; (void)pNrLevel;
  return 0;
}

WEAK int CameraGetSwapICR(int *pSwap) {
  (void)pSwap;
  return 0;
}

WEAK int CameraSetNFLevel(int daynight, int nf_level) {
  (void)daynight; (void)nf_level;
  return 0;
}

WEAK int CameraSwapICR(int swap) {
  (void)swap;
  return 0;
}

WEAK int Camera_Get_StyleMode(int *pChoice) {
  (void)pChoice;
  return 0;
}

WEAK int Camera_Set_StyleMode(int choice) {
  (void)choice;
  return 0;
}

WEAK int camera_bwBurst_get(XM_U8 *pu8Enable) {
  (void)pu8Enable;
  return 0;
}

WEAK int camera_bwBurst_set(XM_U8 u8Enable) {
  (void)u8Enable;
  return 0;
}

WEAK int camera_debug(char *cmd) {
  (void)cmd;
  return 0;
}

WEAK int camera_es_shutter_ex(XM_U8 u8Mode, XM_U8 *pu8Status) {
  (void)u8Mode; (void)pu8Status;
  return 0;
}

WEAK int camera_get_ae_sensitivity(int *pLevel) {
  (void)pLevel;
  return 0;
}

WEAK int camera_get_ae_sensitivity2(int *pLevel) {
  (void)pLevel;
  return 0;
}

WEAK int camera_get_antiFalseColor(unsigned int *pu32Level) {
  (void)pu32Level;
  return 0;
}

WEAK int camera_get_blc(XM_U32 *pMode) {
  (void)pMode;
  return 0;
}

WEAK int camera_get_color(int channel, CAMERA_COLOR *pColor) {
  (void)channel; (void)pColor;
  return 0;
}

WEAK int camera_get_dn_state(XM_U8 *pu8Mode) {
  (void)pu8Mode;
  return 0;
}

WEAK int camera_get_dnc_mode(XM_U32 *pMode) {
  (void)pMode;
  return 0;
}

WEAK int camera_get_dnc_thr(int *pDnc_thr) {
  (void)pDnc_thr;
  return 0;
}

WEAK int camera_get_es_shutter(int *pEshutterLevel) {
  (void)pEshutterLevel;
  return 0;
}

WEAK int camera_get_exposure_level(int *pLevel, XM_U32 *pValue1, XM_U32 *pValue2) {
  (void)pLevel; (void)pValue1; (void)pValue2;
  return 0;
}

WEAK int camera_get_flip(int *pFlip) {
  (void)pFlip;
  return 0;
}

WEAK int camera_get_gain(int *pGainLevel, int *pAutoEn) {
  (void)pGainLevel; (void)pAutoEn;
  return 0;
}

WEAK int camera_get_gamma(XM_U32 *pu32Gamma) {
  (void)pu32Gamma;
  return 0;
}

WEAK int camera_get_ircut_mode(int *pIrcutMode) {
  (void)pIrcutMode;
  return 0;
}

WEAK int camera_get_mirror(int *pMirror) {
  (void)pMirror;
  return 0;
}

WEAK int camera_get_refrence_level(void) {
  return 0;
}

WEAK int camera_get_reject_flicker(int *pEn) {
  (void)pEn;
  return 0;
}

WEAK int camera_get_sawtooth(unsigned int *pu32Level) {
  (void)pu32Level;
  return 0;
}

WEAK int camera_get_scene(CAMERA_SCENE *pScene) {
  (void)pScene;
  return 0;
}

WEAK int camera_get_wbGM(XM_U8 *pu8Data) {
  (void)pu8Data;
  return 0;
}

WEAK int camera_get_wbRB(XM_U8 *pu8Data) {
  (void)pu8Data;
  return 0;
}

WEAK int camera_get_wdr(int *pLevel, int *pEnable) {
  (void)pLevel; (void)pEnable;
  return 0;
}

WEAK int camera_init(XM_U32 *pu32ProductType) {
  (void)pu32ProductType;
  return 0;
}

WEAK int camera_para_get(CAM_INIT_DATA *pstCamearPara) {
  (void)pstCamearPara;
  return 0;
}

WEAK int camera_para_set(CAM_INIT_DATA *pstCamearPara) {
  (void)pstCamearPara;
  return 0;
}

WEAK int camera_set_ae_sensitivity(int ae_sensitivity) {
  (void)ae_sensitivity;
  return 0;
}

WEAK int camera_set_ae_sensitivity2(int level) {
  (void)level;
  return 0;
}

WEAK int camera_set_antiFalseColor(unsigned int u32Level) {
  (void)u32Level;
  return 0;
}

WEAK int camera_set_blc(unsigned int mode) {
  (void)mode;
  return 0;
}

WEAK int camera_set_color(int channel, CAMERA_COLOR *pColor) {
  (void)channel; (void)pColor;
  return 0;
}

WEAK int camera_set_dnc_mode(unsigned int mode) {
  (void)mode;
  return 0;
}

WEAK int camera_set_dnc_thr(int dnc_thr) {
  (void)dnc_thr;
  return 0;
}

WEAK int camera_set_es_shutter(int es_shutter) {
  (void)es_shutter;
  return 0;
}

WEAK int camera_set_exposure_level(int level, unsigned int value1, unsigned int value2) {
  (void)level; (void)value1; (void)value2;
  return 0;
}

WEAK int camera_set_flip(int flip) {
  (void)flip;
  return 0;
}

WEAK int camera_set_gain(int s32GainMax, int s32AutoEn) {
  (void)s32GainMax; (void)s32AutoEn;
  return 0;
}

WEAK int camera_set_gamma(XM_U32 u32Gamma) {
  (void)u32Gamma;
  return 0;
}

WEAK int camera_set_ircut_mode(int ircut_mode) {
  (void)ircut_mode;
  return 0;
}

WEAK int camera_set_mirror(int mirror) {
  (void)mirror;
  return 0;
}

WEAK int camera_set_refrence_level(int level) {
  (void)level;
  return 0;
}

WEAK int camera_set_reject_flicker(int en) {
  (void)en;
  return 0;
}

WEAK int camera_set_sawtooth(unsigned int u32Level) {
  (void)u32Level;
  return 0;
}

WEAK int camera_set_scene(CAMERA_SCENE scene) {
  (void)scene;
  return 0;
}

WEAK int camera_set_vstd(int channel, unsigned int u32Vstd) {
  (void)channel; (void)u32Vstd;
  return 0;
}

WEAK int camera_set_wbGM(XM_U8 u8Data) {
  (void)u8Data;
  return 0;
}

WEAK int camera_set_wbRB(XM_U8 u8Data) {
  (void)u8Data;
  return 0;
}

WEAK int camera_set_wdr(int level, int enable) {
  (void)level; (void)enable;
  return 0;
}
