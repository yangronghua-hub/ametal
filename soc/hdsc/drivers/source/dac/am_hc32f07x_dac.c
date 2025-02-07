/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief DAC驱动实现
 *
 * \internal
 * \par Modification history
 * - 1.00 19-09-24
 * \endinternal
 */

#include "am_int.h"
#include "am_hc32_dac.h"
#include "hw/amhw_hc32f07x_dac.h"
#include "hc32_periph_map.h"

/*******************************************************************************
* 私有定义
*******************************************************************************/

#define __ZLG_DAC_BITS_GET(p_drv)             \
        (((am_hc32_dac_dev_t *)p_drv)->p_devinfo->bits)

#define __ZLG_DAC_VREF_GET(p_drv)             \
        (((am_hc32_dac_dev_t *)p_drv)->p_devinfo->vref)

/*******************************************************************************
* 函数声明
*******************************************************************************/
static uint32_t __hc32_dac_get_bits (void *p_drv);

static uint32_t __hc32_dac_get_vref (void *p_drv);

static int __hc32_dac_val_set (void *p_drv, int chan, uint32_t value);

static int __hc32_dac_enable (void *p_drv, int chan );

static int __hc32_dac_disable (void *p_drv, int chan );

/**
 * \brief DAC服务函数
 */
static const struct am_dac_drv_funcs __g_dac_drvfuncs = {
       __hc32_dac_get_bits,
       __hc32_dac_get_vref,
       __hc32_dac_val_set,
       __hc32_dac_enable,
       __hc32_dac_disable
};

/******************************************************************************/

/**
 * \brief 获取DAC转换精度。
 */
static uint32_t __hc32_dac_get_bits (void *p_drv)
{
    return (uint32_t)__ZLG_DAC_BITS_GET(p_drv);
}

/**
 * \brief 获取DAC参考电压。
 */
static uint32_t __hc32_dac_get_vref (void *p_drv)
{
    return (uint32_t)__ZLG_DAC_VREF_GET(p_drv);
}

/**
 * \brief 设置通道的DAC转换值。
 */
static int __hc32_dac_val_set (void *p_drv, int chan, uint32_t value)
{
    am_hc32_dac_dev_t *p_dev    = (am_hc32_dac_dev_t *)p_drv;
    amhw_hc32f07x_dac_t   *p_hw_dac =
                         (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);

    if (p_dev == NULL ) {
        return -AM_EINVAL;
    }

    if ((chan != AMHW_HC32F07X_DAC_CHAN_0) && (chan != AMHW_HC32F07X_DAC_CHAN_1)) {
        return -AM_ENXIO;       /* 无效的通道值 */
    }

    p_dev->chan = chan;
    if(chan == AMHW_HC32F07X_DAC_CHAN_0) {
        if (p_dev->p_devinfo->bits == 8) {

            amhw_hc32f07x_dac_chan0_8bit_right_aligned_data(p_hw_dac, value);
        } else {

            if (p_dev->p_devinfo->align_way == AM_HC32_DAC_ALIGN_WAY_12_RIGHT){

                amhw_hc32f07x_dac_chan0_12bit_right_aligned_data(p_hw_dac, value);
            }else{

                amhw_hc32f07x_dac_chan0_12bit_left_aligned_data(p_hw_dac, value);
            }
        }
	}else {
		if (p_dev->p_devinfo->bits == 8) {

            amhw_hc32f07x_dac_chan1_8bit_right_aligned_data(p_hw_dac, value);
        } else {

            if (p_dev->p_devinfo->align_way == AM_HC32_DAC_ALIGN_WAY_12_RIGHT){

                amhw_hc32f07x_dac_chan1_12bit_right_aligned_data(p_hw_dac, value);

            }else{

                amhw_hc32f07x_dac_chan1_12bit_left_aligned_data(p_hw_dac, value);
            }
        }		
	}

    return AM_OK;
}

/**
 * \brief 启动DAC转换
 */
static int __hc32_dac_enable (void *p_drv, int chan)
{

    am_hc32_dac_dev_t *p_dev    = (am_hc32_dac_dev_t *)p_drv;
    amhw_hc32f07x_dac_t   *p_hw_dac =
                          (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);

    if (p_dev == NULL ) {
        return -AM_EINVAL;
    }

    if ((chan != AMHW_HC32F07X_DAC_CHAN_0) && (chan != AMHW_HC32F07X_DAC_CHAN_1)) {
        return -AM_ENXIO;       /* 无效的通道值 */
    }

    p_dev->chan = chan;

    if (chan == AMHW_HC32F07X_DAC_CHAN_0){
        /* 使能dac通道 */
        amhw_hc32f07x_dac_chan0_enable(p_hw_dac);

        /* 参考电压选择 */
        amhw_hc32f07x_dac_chan0_sref_sel(p_hw_dac, p_dev->p_devinfo->vref_rsc);

        /* 失能输出缓冲器*/
        amhw_hc32f07x_dac_chan0_output_buf_disable(p_hw_dac);
	  }else {
		   		/* 使能dac通道 */
        amhw_hc32f07x_dac_chan1_enable(p_hw_dac);

        /* 参考电压选择 */
        amhw_hc32f07x_dac_chan1_sref_sel(p_hw_dac, p_dev->p_devinfo->vref_rsc);
		
        /* 失能输出缓冲器*/
        amhw_hc32f07x_dac_chan1_output_buf_disable(p_hw_dac);
		

	  }

    return AM_OK;
}

/**
 * \brief 禁止DAC转换
 */
static int __hc32_dac_disable (void *p_drv, int chan )
{

    am_hc32_dac_dev_t     *p_dev    = (am_hc32_dac_dev_t *)p_drv;
    amhw_hc32f07x_dac_t   *p_hw_dac =
                          (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);

    if (p_dev == NULL ) {
        return -AM_EINVAL;
    }

   if ((chan != AMHW_HC32F07X_DAC_CHAN_0) && (chan != AMHW_HC32F07X_DAC_CHAN_1)) {
        return -AM_ENXIO;       /* 无效的通道值 */
    }

    p_dev->chan = chan;

	if (chan == AMHW_HC32F07X_DAC_CHAN_0) {
        amhw_hc32f07x_dac_chan0_disable(p_hw_dac);
	}else {
		    amhw_hc32f07x_dac_chan1_disable(p_hw_dac);
	}

    return AM_OK;
}

/**
 * \brief DAC初始化
 */
am_dac_handle_t am_hc32_dac_init (am_hc32_dac_dev_t           *p_dev,
                                  const am_hc32_dac_devinfo_t *p_devinfo)
{
    if ((p_devinfo == NULL) || (p_dev == NULL)) {
        return NULL;
    }

    p_dev->p_devinfo         = p_devinfo;
    p_dev->dac_serve.p_funcs = &__g_dac_drvfuncs;
    p_dev->dac_serve.p_drv   = p_dev;

    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    /* 使能BGR */
    amhw_hc32_bgr_enable(AM_TRUE); 

    return (am_dac_handle_t)(&(p_dev->dac_serve));
}

/**
 * \brief DAC去初始化
 */
void am_hc32_dac_deinit (am_dac_handle_t handle)
{
    am_hc32_dac_dev_t *p_dev  = (am_hc32_dac_dev_t *)handle;
    amhw_hc32f07x_dac_t *p_hw_dac =
                          (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);

    p_dev->dac_serve.p_funcs = NULL;
    p_dev->dac_serve.p_drv   = NULL;

    amhw_hc32f07x_dac_chan0_disable(p_hw_dac);

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

/**
 * \brief DAC 触发方式选择
 */
int am_hc32_dac_tri_way_sel (am_dac_handle_t handle, uint32_t tri_way)
{
    am_hc32_dac_dev_t *p_dev  = (am_hc32_dac_dev_t *)handle;
    amhw_hc32f07x_dac_t *p_hw_dac =
                          (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);

    if (handle == NULL){

        return AM_ERROR;
    }

	if (p_dev->chan == AMHW_HC32F07X_DAC_CHAN_0){
		
        /* 禁能dac通道*/
        amhw_hc32f07x_dac_chan0_disable(p_hw_dac);
	

        switch (tri_way){

            case AM_HC32_DAC_CHAN_TIM0_TRGO :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM0_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM1_TRGO :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM1_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM2_TRGO :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM2_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM3_TRGO :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM3_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM4_TRGO :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM4_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM5_TRGO :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM5_TRGO);
                break;
            case AM_HC32_DAC_CHAN_SOFTWARE_TRG :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_SOFTWARE_TRG);
                break;
            case AM_HC32_DAC_CHAN_EXTER :
                amhw_hc32f07x_dac_chan0_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_EXTER);amhw_hc32f07x_dac_chan_tri_port_sel (p_hw_dac,
                                            AMHW_HC32F07X_DAC_CHAN_TRI_POT_PB9);
                break;
            default:
                return AM_ERROR;
        }
		
		 /* 触发使能 */
        amhw_hc32f07x_dac_chan0_trg_enable(p_hw_dac);
	}else {
		
		/* 禁能dac通道*/
        amhw_hc32f07x_dac_chan1_disable(p_hw_dac);
	
        switch (tri_way){

            case AM_HC32_DAC_CHAN_TIM0_TRGO :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM0_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM1_TRGO :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM1_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM2_TRGO :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM2_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM3_TRGO :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM3_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM4_TRGO :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM4_TRGO);
                break;
            case AM_HC32_DAC_CHAN_TIM5_TRGO :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_TIM5_TRGO);
                break;
            case AM_HC32_DAC_CHAN_SOFTWARE_TRG :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_SOFTWARE_TRG);
                break;
            case AM_HC32_DAC_CHAN_EXTER :
                amhw_hc32f07x_dac_chan1_trg_sel(p_hw_dac,
                                             AMHW_HC32F07X_DAC_CHAN_EXTER);
                break;
            default:
                return AM_ERROR;
        }
		 /* 触发使能 */
        amhw_hc32f07x_dac_chan1_trg_enable(p_hw_dac);
	}

    return AM_OK;
}


/**
 * \brief DAC 软件触发
 */
void am_hc32_dac_soft_tri (am_dac_handle_t handle)
{
    am_hc32_dac_dev_t *p_dev  = (am_hc32_dac_dev_t *)handle;
    amhw_hc32f07x_dac_t *p_hw_dac =
                          (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);
    if (p_dev->chan == AMHW_HC32F07X_DAC_CHAN_0){
        amhw_hc32f07x_dac_chan0_software_trg_enable(p_hw_dac);
	}else {
		amhw_hc32f07x_dac_chan1_software_trg_enable(p_hw_dac);
	}
}

/**
 * \brief DAC 转换数据获取
 */
void am_hc32_dac_convt_data_get (am_dac_handle_t handle, uint16_t *p_out_data)
{
    am_hc32_dac_dev_t *p_dev  = (am_hc32_dac_dev_t *)handle;
    amhw_hc32f07x_dac_t *p_hw_dac =
                          (amhw_hc32f07x_dac_t *)(p_dev->p_devinfo->dac_reg_base);
    if(p_dev->chan == AMHW_HC32F07X_DAC_CHAN_0){
        *p_out_data = amhw_hc32f07x_dac_chan0_output_data(p_hw_dac);
	}else {
		*p_out_data = amhw_hc32f07x_dac_chan1_output_data(p_hw_dac);
	}
}

/* end of file */
