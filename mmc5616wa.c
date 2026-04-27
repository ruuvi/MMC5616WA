#include "mmc5616wa.h"

#define MMC5616WA_MEASUREMENT_TIME_MAX_MS (10U)
#define MMC5616WA_FIFO_WATERMARK_SHIFT    (3U)

static int32_t mmc5616wa_delay(const mmc5616wa_ctx_t *ctx,
                               const uint32_t delay_ms)
{
    if ((ctx == NULL) || (ctx->delay_ms == NULL))
    {
        return MMC5616WA_E_NULL;
    }

    ctx->delay_ms(delay_ms);
    return MMC5616WA_OK;
}

static int32_t mmc5616wa_read_byte(const mmc5616wa_ctx_t *ctx,
                                   const uint8_t reg,
                                   uint8_t *value)
{
    return mmc5616wa_read_reg(ctx, reg, value, 1U);
}

static int32_t mmc5616wa_write_byte(const mmc5616wa_ctx_t *ctx,
                                    const uint8_t reg,
                                    const uint8_t value)
{
    return mmc5616wa_write_reg(ctx, reg, &value, 1U);
}

static void mmc5616wa_decode_raw_20bit(const uint8_t *buffer,
                                       mmc5616wa_mag_raw_t *sample)
{
    const int32_t x_raw = ((int32_t)buffer[0] << 12)
                        | ((int32_t)buffer[1] << 4)
                        | (((int32_t)buffer[6] & (int32_t)MMC5616WA_XOUT2_AXIS_LSB_MASK) >> 4);
    const int32_t y_raw = ((int32_t)buffer[2] << 12)
                        | ((int32_t)buffer[3] << 4)
                        | (((int32_t)buffer[7] & (int32_t)MMC5616WA_YOUT2_AXIS_LSB_MASK) >> 4);
    const int32_t z_raw = ((int32_t)buffer[4] << 12)
                        | ((int32_t)buffer[5] << 4)
                        | (((int32_t)buffer[8] & (int32_t)MMC5616WA_ZOUT2_AXIS_LSB_MASK) >> 4);

    sample->x = x_raw;
    sample->y = y_raw;
    sample->z = z_raw;
}

static void mmc5616wa_decode_fifo_16bit(const uint8_t *buffer,
                                        mmc5616wa_fifo_sample_t *sample)
{
    const uint16_t x = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
    const uint16_t y = ((uint16_t)buffer[2] << 8) | (uint16_t)buffer[3];
    const uint16_t z = ((uint16_t)buffer[4] << 8) | (uint16_t)buffer[5];

    sample->x = (int16_t)((int32_t)x - MMC5616WA_16BIT_NULL_COUNTS);
    sample->y = (int16_t)((int32_t)y - MMC5616WA_16BIT_NULL_COUNTS);
    sample->z = (int16_t)((int32_t)z - MMC5616WA_16BIT_NULL_COUNTS);
}

int32_t mmc5616wa_read_reg(const mmc5616wa_ctx_t *ctx, const uint8_t reg,
                           uint8_t *data, const uint16_t len)
{
    if ((ctx == NULL) || (ctx->read_reg == NULL) || (data == NULL))
    {
        return MMC5616WA_E_NULL;
    }

    return ctx->read_reg(ctx->handle, reg, data, len);
}

int32_t mmc5616wa_write_reg(const mmc5616wa_ctx_t *ctx, const uint8_t reg,
                            const uint8_t *data, const uint16_t len)
{
    if ((ctx == NULL) || (ctx->write_reg == NULL) || (data == NULL))
    {
        return MMC5616WA_E_NULL;
    }

    return ctx->write_reg(ctx->handle, reg, data, len);
}

void mmc5616wa_default_config(mmc5616wa_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    config->bandwidth = MMC5616WA_BW_6_6_MS;
    config->odr = 0U;
    config->auto_set_reset = true;
    config->continuous_mode = false;
    config->high_power = false;
    config->enable_2k_odr = false;
    config->periodic_set_enable = false;
    config->periodic_set_rate = MMC5616WA_PERIODIC_SET_EVERY_1;
}

void mmc5616wa_default_fifo_config(mmc5616wa_fifo_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    config->enable = false;
    config->interrupt_enable = false;
    config->address_loop_enable = false;
    config->watermark = 0U;
}

int32_t mmc5616wa_device_id_get(const mmc5616wa_ctx_t *ctx,
                                uint8_t *chip_id,
                                uint8_t *product_id)
{
    int32_t ret;

    if ((chip_id == NULL) || (product_id == NULL))
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_CHIP_ID, chip_id);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    return mmc5616wa_read_byte(ctx, MMC5616WA_REG_PRODUCT_ID, product_id);
}

int32_t mmc5616wa_reset(const mmc5616wa_ctx_t *ctx)
{
    const int32_t ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_1,
                                       MMC5616WA_CTRL1_SW_RESET);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    return mmc5616wa_delay(ctx, MMC5616WA_RESET_TIME_MS);
}

int32_t mmc5616wa_init(const mmc5616wa_ctx_t *ctx)
{
    mmc5616wa_config_t config;
    mmc5616wa_fifo_config_t fifo_config;
    uint8_t chip_id = 0U;
    uint8_t product_id = 0U;
    int32_t ret;

    if (ctx == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_reset(ctx);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_device_id_get(ctx, &chip_id, &product_id);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    if ((chip_id != MMC5616WA_CHIP_ID_VALUE)
     || (product_id != MMC5616WA_PRODUCT_ID_VALUE))
    {
        return MMC5616WA_E_NOT_FOUND;
    }

    mmc5616wa_default_config(&config);
    ret = mmc5616wa_config_set(ctx, &config);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    mmc5616wa_default_fifo_config(&fifo_config);
    return mmc5616wa_fifo_config_set(ctx, &fifo_config);
}

int32_t mmc5616wa_do_set(const mmc5616wa_ctx_t *ctx)
{
    const int32_t ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0,
                                       MMC5616WA_CTRL0_DO_SET);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    return mmc5616wa_delay(ctx, MMC5616WA_SET_RESET_TIME_MS);
}

int32_t mmc5616wa_do_reset(const mmc5616wa_ctx_t *ctx)
{
    const int32_t ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0,
                                       MMC5616WA_CTRL0_DO_RESET);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    return mmc5616wa_delay(ctx, MMC5616WA_SET_RESET_TIME_MS);
}

int32_t mmc5616wa_config_set(const mmc5616wa_ctx_t *ctx,
                             const mmc5616wa_config_t *config)
{
    uint8_t ctrl0 = 0U;
    uint8_t ctrl1 = 0U;
    uint8_t ctrl2 = 0U;
    uint8_t ana_ctrl = 0U;
    int32_t ret;

    if (config == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    if ((uint8_t)config->bandwidth > (uint8_t)MMC5616WA_BW_1_2_MS)
    {
        return MMC5616WA_E_INVALID_PARAM;
    }

    ctrl0 = config->auto_set_reset ? MMC5616WA_CTRL0_AUTO_SR_EN : 0U;
    ctrl1 = (uint8_t)config->bandwidth & MMC5616WA_CTRL1_BW_MASK;
    ctrl2 = config->periodic_set_enable ? MMC5616WA_CTRL2_EN_PRD_SET : 0U;
    ctrl2 |= (uint8_t)config->periodic_set_rate & MMC5616WA_CTRL2_PRD_SET_MASK;

    if (config->high_power)
    {
        ctrl2 |= MMC5616WA_CTRL2_HPOWER;
    }

    if (config->enable_2k_odr)
    {
        ana_ctrl |= MMC5616WA_ANA_CTRL_EN_ODR2K;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_2,
                               (uint8_t)(ctrl2 & (uint8_t)(~MMC5616WA_CTRL2_CMM_EN)));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_ANA_CTRL, ana_ctrl);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_1, ctrl1);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_ODR, config->odr);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0, ctrl0);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    if (config->continuous_mode && (config->odr != 0U))
    {
        ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0,
                                   (uint8_t)(ctrl0 | MMC5616WA_CTRL0_CMM_FREQ_EN));

        if (ret != MMC5616WA_OK)
        {
            return ret;
        }

        ctrl2 |= MMC5616WA_CTRL2_CMM_EN;
        ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_2, ctrl2);
    }

    return ret;
}

int32_t mmc5616wa_config_get(const mmc5616wa_ctx_t *ctx,
                             mmc5616wa_config_t *config)
{
    uint8_t ctrl0 = 0U;
    uint8_t ctrl1 = 0U;
    uint8_t ctrl2 = 0U;
    uint8_t ana_ctrl = 0U;
    int32_t ret;

    if (config == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0, &ctrl0);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_1, &ctrl1);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_2, &ctrl2);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_ANA_CTRL, &ana_ctrl);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_ODR, &config->odr);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    config->bandwidth = (mmc5616wa_bandwidth_t)(ctrl1 & MMC5616WA_CTRL1_BW_MASK);
    config->auto_set_reset = (ctrl0 & MMC5616WA_CTRL0_AUTO_SR_EN) != 0U;
    config->continuous_mode = (ctrl2 & MMC5616WA_CTRL2_CMM_EN) != 0U;
    config->high_power = (ctrl2 & MMC5616WA_CTRL2_HPOWER) != 0U;
    config->enable_2k_odr = (ana_ctrl & MMC5616WA_ANA_CTRL_EN_ODR2K) != 0U;
    config->periodic_set_enable = (ctrl2 & MMC5616WA_CTRL2_EN_PRD_SET) != 0U;
    config->periodic_set_rate = (mmc5616wa_periodic_set_t)(ctrl2 & MMC5616WA_CTRL2_PRD_SET_MASK);

    return MMC5616WA_OK;
}

int32_t mmc5616wa_magnetic_measurement_get(const mmc5616wa_ctx_t *ctx,
                                           mmc5616wa_mag_raw_t *sample)
{
    uint8_t ctrl0 = 0U;
    uint8_t buffer[9] = {0U};
    int32_t ret;

    if (sample == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0, &ctrl0);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0,
                               (uint8_t)((ctrl0 & MMC5616WA_CTRL0_AUTO_SR_EN)
                               | MMC5616WA_CTRL0_TAKE_MEAS_M));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_delay(ctx, MMC5616WA_MEASUREMENT_TIME_MAX_MS);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_reg(ctx, MMC5616WA_REG_XOUT0, buffer, (uint16_t)sizeof(buffer));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    mmc5616wa_decode_raw_20bit(buffer, sample);
    return MMC5616WA_OK;
}

int32_t mmc5616wa_temperature_measurement_get(const mmc5616wa_ctx_t *ctx,
                                              uint8_t *raw_temperature)
{
    uint8_t ctrl0 = 0U;
    int32_t ret;

    if (raw_temperature == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0, &ctrl0);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0,
                               (uint8_t)((ctrl0 & MMC5616WA_CTRL0_AUTO_SR_EN)
                               | MMC5616WA_CTRL0_TAKE_MEAS_T));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_delay(ctx, MMC5616WA_MEASUREMENT_TIME_MAX_MS);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    return mmc5616wa_read_byte(ctx, MMC5616WA_REG_TOUT, raw_temperature);
}

int32_t mmc5616wa_self_test(const mmc5616wa_ctx_t *ctx, bool *passed)
{
    uint8_t factory_values[3] = {0U};
    uint8_t thresholds[3] = {0U};
    uint8_t status1_before = 0U;
    uint8_t status1_after_set = 0U;
    uint8_t status1_after_reset = 0U;
    uint8_t status1 = 0U;
    int32_t ret;
    size_t index;

    if (passed == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    *passed = false;

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_STATUS1, &status1_before);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    // 1) Read out the selftest signal stored at register 27H, 28H, and 29H.
    ret = mmc5616wa_read_reg(ctx, MMC5616WA_REG_ST_X, factory_values,
                             (uint16_t)sizeof(factory_values));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    // 2) Calculate the selftest signal threshold with 80% of the data readout from above registers.
    for (index = 0; index < sizeof(factory_values); index++)
    {
        uint16_t threshold = 0U;
        const uint8_t trim_delta = (factory_values[index] >= 128U)
                                   ? (uint8_t)(factory_values[index] - 128U)
                                   : (uint8_t)(128U - factory_values[index]);

        threshold = ((uint16_t)trim_delta * 16U) / 5U;
        thresholds[index] = (threshold > UINT8_MAX) ? UINT8_MAX : (uint8_t)threshold;
    }

    // 3)  Write the threshold in to the register 1EH, 1FH, and 20H.
    ret = mmc5616wa_write_reg(ctx, MMC5616WA_REG_ST_X_TH, thresholds,
                              (uint16_t)sizeof(thresholds));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    /* Recondition the sensor before self-test so Sat_sensor is evaluated after
     * an explicit set/reset cycle instead of whatever magnetic state the part
     * happened to be left in. */
    ret = mmc5616wa_do_set(ctx);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_STATUS1, &status1_after_set);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_do_reset(ctx);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_STATUS1, &status1_after_reset);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    // 4) Write [01000001] (TM_M and auto_st_en high) to Internal Control Register 1BH to initiate a selftest.
    ret = mmc5616wa_write_byte(ctx, MMC5616WA_REG_INTERNAL_CTRL_0,
                               (uint8_t)(MMC5616WA_CTRL0_AUTO_ST_EN
                               | MMC5616WA_CTRL0_TAKE_MEAS_M));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    ret = mmc5616wa_delay(ctx, MMC5616WA_MEASUREMENT_TIME_MAX_MS);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    // 5) Read out value of Sat_sensor bit at the Device Status register 18H.
    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_STATUS1, &status1);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    // 6) Sat_sensor=0, PASS selftest.
    *passed = ((status1 & MMC5616WA_STATUS1_SAT_SENSOR) == 0U);

    return MMC5616WA_OK;
}

int32_t mmc5616wa_fifo_config_set(const mmc5616wa_ctx_t *ctx,
                                  const mmc5616wa_fifo_config_t *config)
{
    uint8_t fifo_ctrl = 0U;

    if (config == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    if (config->watermark >= MMC5616WA_FIFO_DEPTH)
    {
        return MMC5616WA_E_INVALID_PARAM;
    }

    if (config->enable)
    {
        fifo_ctrl |= MMC5616WA_FIFO_CTRL_FIFO_EN;
    }

    if (config->interrupt_enable)
    {
        fifo_ctrl |= MMC5616WA_FIFO_CTRL_FIFO_INT_EN;
    }

    if (config->address_loop_enable)
    {
        fifo_ctrl |= MMC5616WA_FIFO_CTRL_ADDR_LOOP_EN;
    }

    fifo_ctrl |= (uint8_t)(config->watermark << MMC5616WA_FIFO_WATERMARK_SHIFT);
    return mmc5616wa_write_byte(ctx, MMC5616WA_REG_FIFO_CTRL, fifo_ctrl);
}

int32_t mmc5616wa_fifo_config_get(const mmc5616wa_ctx_t *ctx,
                                  mmc5616wa_fifo_config_t *config)
{
    uint8_t fifo_ctrl = 0U;
    int32_t ret;

    if (config == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_read_byte(ctx, MMC5616WA_REG_FIFO_CTRL, &fifo_ctrl);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    config->enable = (fifo_ctrl & MMC5616WA_FIFO_CTRL_FIFO_EN) != 0U;
    config->interrupt_enable = (fifo_ctrl & MMC5616WA_FIFO_CTRL_FIFO_INT_EN) != 0U;
    config->address_loop_enable = (fifo_ctrl & MMC5616WA_FIFO_CTRL_ADDR_LOOP_EN) != 0U;
    config->watermark = (uint8_t)((fifo_ctrl & MMC5616WA_FIFO_CTRL_FIFO_WM_MASK)
                                >> MMC5616WA_FIFO_WATERMARK_SHIFT);

    return MMC5616WA_OK;
}

int32_t mmc5616wa_fifo_status_get(const mmc5616wa_ctx_t *ctx,
                                  mmc5616wa_fifo_status_t *status)
{
    uint8_t buffer[2] = {0U};
    int32_t ret;

    if (status == NULL)
    {
        return MMC5616WA_E_NULL;
    }

    ret = mmc5616wa_read_reg(ctx, MMC5616WA_REG_XOUT2, buffer, (uint16_t)sizeof(buffer));

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    status->level = buffer[0] & MMC5616WA_XOUT2_FIFO_LEVEL_MASK;
    status->full = (buffer[1] & MMC5616WA_YOUT2_FIFO_FULL) != 0U;
    status->empty = (buffer[1] & MMC5616WA_YOUT2_FIFO_EMPTY) != 0U;
    return MMC5616WA_OK;
}

int32_t mmc5616wa_fifo_read(const mmc5616wa_ctx_t *ctx,
                            mmc5616wa_fifo_sample_t *samples,
                            const size_t max_samples,
                            size_t *samples_read)
{
    mmc5616wa_fifo_status_t status;
    uint8_t buffer[MMC5616WA_FIFO_SAMPLE_SIZE] = {0U};
    size_t count;
    size_t index;
    int32_t ret;

    if ((samples == NULL) || (samples_read == NULL))
    {
        return MMC5616WA_E_NULL;
    }

    *samples_read = 0U;

    ret = mmc5616wa_fifo_status_get(ctx, &status);

    if (ret != MMC5616WA_OK)
    {
        return ret;
    }

    if (status.empty || (max_samples == 0U))
    {
        return MMC5616WA_OK;
    }

    count = status.level;

    if (count > max_samples)
    {
        count = max_samples;
    }

    for (index = 0U; index < count; index++)
    {
        ret = mmc5616wa_read_reg(ctx, MMC5616WA_REG_XOUT0, buffer,
                                 (uint16_t)sizeof(buffer));

        if (ret != MMC5616WA_OK)
        {
            return ret;
        }

        mmc5616wa_decode_fifo_16bit(buffer, &samples[index]);
        *samples_read = index + 1U;
    }

    return MMC5616WA_OK;
}

float mmc5616wa_magnetic_20bit_to_gauss(const int32_t raw)
{
    return ((float)(raw - MMC5616WA_20BIT_NULL_COUNTS)
          / MMC5616WA_20BIT_COUNTS_PER_GAUSS);
}

float mmc5616wa_magnetic_18bit_to_gauss(const int32_t raw)
{
    return ((float)(raw - MMC5616WA_18BIT_NULL_COUNTS)
          / MMC5616WA_18BIT_COUNTS_PER_GAUSS);
}

float mmc5616wa_magnetic_16bit_to_gauss(const int16_t raw)
{
    return ((float)raw / MMC5616WA_16BIT_COUNTS_PER_GAUSS);
}

float mmc5616wa_temperature_to_celsius(const uint8_t raw)
{
    return MMC5616WA_TEMPERATURE_OFFSET_C
         + ((float)raw * MMC5616WA_TEMPERATURE_SCALE_C);
}