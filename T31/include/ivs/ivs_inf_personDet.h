#ifndef __IVS_INF_PERSONDET_H__
#define __IVS_INF_PERSONDET_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "ivs_common.h"

#define NUM_OF_PERSONS 20
#define IVS_PERSONDET_PERM_MAX_ROI  4
#define IVS_PERSONDET_PERM_MAX_RECT 4

/*
 * 人形信息结构体
 */
typedef struct {
    IVSRect box; /**reserved*/
    IVSRect show_box; /**< 人形区域坐标 */
    float confidence; /**< 人形检测结果的置信度 */
}person_info;
/*
 * 
 */
typedef struct {
  IVSPoint *p;    /**< 周界各个顶点信息,不能有线交叉 */
  int pcnt;      /**<  周界顶点数量,最多8个点,*/
  uint64_t alarm_last_time;	/**<  reserved*/
}persondet_perm_t;


/*
 * 人形检测输入结构体
 */
typedef struct {
    bool ptime; /**< 是否打印检测时间 */
    int skip_num; /**< 跳帧数目 */
    IVSFrameInfo frameInfo; /**< 帧信息 */
    int sense; /**< 检测灵敏度 0~5 0:最不灵敏 5:最灵敏 default:4 */
    int detdist; /**< 检测距离 0~4  0:6米 max(img_w, img_h) >= 320 ;  1:8米  max(img_w, img_h) >= 416 \
                    2:10米 max(img_w, img_h) >= 512   3:11米 max(img_w, img_h) >= 640 \
                    4:13米  max(img_w, img_h) >= 800 default:2 */
    bool enable_move; /**<true: 开启移动侦测触发人形检测 false:关闭移动侦测触发人形检测 default: false>*/
    bool enable_perm; /**<true: 开启周界功能 false: 关闭周界功能 default: false>*/
    persondet_perm_t perms[IVS_PERSONDET_PERM_MAX_ROI];  /**< 周界信息 */
    int permcnt;                             /**<  周界数量,最多4个*/


}persondet_param_input_t;

/*
 * 人形检测输出结构体
 */
typedef struct {
    int count; /**< 识别出人形的个数 */
    int count_move;/**< 识别出移动目标的个数 */
    person_info person[NUM_OF_PERSONS]; /**< 识别出的人形信息 */
    int64_t timeStamp; /**< 时间戳 */
}persondet_param_output_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __IVS_INF_PERSONDET_H__ */
