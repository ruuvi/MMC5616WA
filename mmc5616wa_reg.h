#ifndef MMC5616WA_REG_H
#define MMC5616WA_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define MMC5616WA_I2C_ADDR_7BIT            (0x30U)

#define MMC5616WA_CHIP_ID_VALUE            (0xD2U)
#define MMC5616WA_PRODUCT_ID_VALUE         (0x11U)

#define MMC5616WA_REG_XOUT0                (0x00U)
#define MMC5616WA_REG_XOUT1                (0x01U)
#define MMC5616WA_REG_YOUT0                (0x02U)
#define MMC5616WA_REG_YOUT1                (0x03U)
#define MMC5616WA_REG_ZOUT0                (0x04U)
#define MMC5616WA_REG_ZOUT1                (0x05U)
#define MMC5616WA_REG_XOUT2                (0x06U)
#define MMC5616WA_REG_YOUT2                (0x07U)
#define MMC5616WA_REG_ZOUT2                (0x08U)
#define MMC5616WA_REG_TOUT                 (0x09U)
#define MMC5616WA_REG_TPH0                 (0x0AU)
#define MMC5616WA_REG_TPH1                 (0x0BU)
#define MMC5616WA_REG_TU                   (0x0CU)
#define MMC5616WA_REG_FIFO_CTRL            (0x0EU)
#define MMC5616WA_REG_STATUS1              (0x18U)
#define MMC5616WA_REG_STATUS0              (0x19U)
#define MMC5616WA_REG_ODR                  (0x1AU)
#define MMC5616WA_REG_INTERNAL_CTRL_0      (0x1BU)
#define MMC5616WA_REG_INTERNAL_CTRL_1      (0x1CU)
#define MMC5616WA_REG_INTERNAL_CTRL_2      (0x1DU)
#define MMC5616WA_REG_ST_X_TH              (0x1EU)
#define MMC5616WA_REG_ST_Y_TH              (0x1FU)
#define MMC5616WA_REG_ST_Z_TH              (0x20U)
#define MMC5616WA_REG_CHIP_ID              (0x21U)
#define MMC5616WA_REG_ANA_CTRL             (0x22U)
#define MMC5616WA_REG_ST_X                 (0x27U)
#define MMC5616WA_REG_ST_Y                 (0x28U)
#define MMC5616WA_REG_ST_Z                 (0x29U)
#define MMC5616WA_REG_PRODUCT_ID           (0x39U)

#define MMC5616WA_AXIS_COUNT               (3U)
#define MMC5616WA_FIFO_DEPTH               (16U)
#define MMC5616WA_FIFO_SAMPLE_SIZE         (6U)
#define MMC5616WA_FIFO_BURST_MAX_SAMPLES   (8U)

#define MMC5616WA_RESET_TIME_MS            (20U)
#define MMC5616WA_SET_RESET_TIME_MS        (1U)

#define MMC5616WA_16BIT_NULL_COUNTS        (32768)
#define MMC5616WA_18BIT_NULL_COUNTS        (131072)
#define MMC5616WA_20BIT_NULL_COUNTS        (524288)

#define MMC5616WA_16BIT_COUNTS_PER_GAUSS   (1024.0f)
#define MMC5616WA_18BIT_COUNTS_PER_GAUSS   (4096.0f)
#define MMC5616WA_20BIT_COUNTS_PER_GAUSS   (16384.0f)

#define MMC5616WA_TEMPERATURE_OFFSET_C     (-75.0f)
#define MMC5616WA_TEMPERATURE_SCALE_C      (0.8f)

#define MMC5616WA_XOUT2_AXIS_LSB_MASK      (0xF0U)
#define MMC5616WA_XOUT2_FIFO_LEVEL_MASK    (0x0FU)
#define MMC5616WA_YOUT2_AXIS_LSB_MASK      (0xF0U)
#define MMC5616WA_YOUT2_FIFO_FULL          (0x02U)
#define MMC5616WA_YOUT2_FIFO_EMPTY         (0x01U)
#define MMC5616WA_ZOUT2_AXIS_LSB_MASK      (0xF0U)

/* Datasheet examples describe Meas_M_Done as bit 1 and Sat_sensor as bit 2. */
#define MMC5616WA_STATUS1_MEAS_T_DONE      (0x01U)
#define MMC5616WA_STATUS1_MEAS_M_DONE      (0x02U)
#define MMC5616WA_STATUS1_SAT_SENSOR       (0x04U)
#define MMC5616WA_STATUS1_OTP_READ_DONE    (0x08U)
#define MMC5616WA_STATUS1_ST_FAIL          (0x10U)
#define MMC5616WA_STATUS1_MDT_FLAG_INT     (0x20U)
#define MMC5616WA_STATUS1_MEAS_T_DONE_INT  (0x40U)
#define MMC5616WA_STATUS1_MEAS_M_DONE_INT  (0x80U)

#define MMC5616WA_STATUS0_PENDING_INT_MASK   (0x03U)
#define MMC5616WA_STATUS0_MDT_FLAG           (0x04U)
#define MMC5616WA_STATUS0_PROTOCOL_ERROR     (0x20U)
#define MMC5616WA_STATUS0_ACTIVITY_MODE_MASK (0xC0U)

#define MMC5616WA_FIFO_CTRL_FIFO_EN        (0x01U)
#define MMC5616WA_FIFO_CTRL_FIFO_INT_EN    (0x02U)
#define MMC5616WA_FIFO_CTRL_FIFO_WM_MASK   (0x78U)
#define MMC5616WA_FIFO_CTRL_ADDR_LOOP_EN   (0x80U)

#define MMC5616WA_CTRL0_TAKE_MEAS_M        (0x01U)
#define MMC5616WA_CTRL0_TAKE_MEAS_T        (0x02U)
#define MMC5616WA_CTRL0_START_MDT          (0x04U)
#define MMC5616WA_CTRL0_DO_SET             (0x08U)
#define MMC5616WA_CTRL0_DO_RESET           (0x10U)
#define MMC5616WA_CTRL0_AUTO_SR_EN         (0x20U)
#define MMC5616WA_CTRL0_AUTO_ST_EN         (0x40U)
#define MMC5616WA_CTRL0_CMM_FREQ_EN        (0x80U)

#define MMC5616WA_CTRL1_BW_MASK            (0x03U)
#define MMC5616WA_CTRL1_X_INHIBIT          (0x04U)
#define MMC5616WA_CTRL1_Y_INHIBIT          (0x08U)
#define MMC5616WA_CTRL1_Z_INHIBIT          (0x10U)
#define MMC5616WA_CTRL1_ST_ENP             (0x20U)
#define MMC5616WA_CTRL1_ST_ENM             (0x40U)
#define MMC5616WA_CTRL1_SW_RESET           (0x80U)

#define MMC5616WA_CTRL2_PRD_SET_MASK       (0x07U)
#define MMC5616WA_CTRL2_EN_PRD_SET         (0x08U)
#define MMC5616WA_CTRL2_CMM_EN             (0x10U)
#define MMC5616WA_CTRL2_INT_MDT_EN         (0x20U)
#define MMC5616WA_CTRL2_INT_MEAS_DONE_EN   (0x40U)
#define MMC5616WA_CTRL2_HPOWER             (0x80U)

#define MMC5616WA_ANA_CTRL_EN_ODR2K        (0x01U)

typedef enum
{
    MMC5616WA_BW_6_6_MS = 0U,
    MMC5616WA_BW_3_5_MS = 1U,
    MMC5616WA_BW_2_0_MS = 2U,
    MMC5616WA_BW_1_2_MS = 3U,
} mmc5616wa_bandwidth_t;

typedef enum
{
    MMC5616WA_PERIODIC_SET_EVERY_1 = 0U,
    MMC5616WA_PERIODIC_SET_EVERY_25 = 1U,
    MMC5616WA_PERIODIC_SET_EVERY_75 = 2U,
    MMC5616WA_PERIODIC_SET_EVERY_100 = 3U,
    MMC5616WA_PERIODIC_SET_EVERY_250 = 4U,
    MMC5616WA_PERIODIC_SET_EVERY_500 = 5U,
    MMC5616WA_PERIODIC_SET_EVERY_1000 = 6U,
    MMC5616WA_PERIODIC_SET_EVERY_2000 = 7U,
} mmc5616wa_periodic_set_t;

typedef int32_t (*mmc5616wa_write_ptr)(void *handle, uint8_t reg,
                                       const uint8_t *data, uint16_t len);
typedef int32_t (*mmc5616wa_read_ptr)(void *handle, uint8_t reg,
                                      uint8_t *data, uint16_t len);
typedef void (*mmc5616wa_delay_ms_ptr)(uint32_t period_ms);

typedef struct
{
    mmc5616wa_write_ptr write_reg;
    mmc5616wa_read_ptr read_reg;
    mmc5616wa_delay_ms_ptr delay_ms;
    void *handle;
} mmc5616wa_ctx_t;

#ifdef __cplusplus
}
#endif

#endif
