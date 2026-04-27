#ifndef MMC5616WA_H
#define MMC5616WA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mmc5616wa_reg.h"

typedef enum
{
    MMC5616WA_OK = 0,
    MMC5616WA_E_NULL = -1,
    MMC5616WA_E_IO = -2,
    MMC5616WA_E_INVALID_PARAM = -3,
    MMC5616WA_E_TIMEOUT = -4,
    MMC5616WA_E_NOT_FOUND = -5,
} mmc5616wa_status_t;

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} mmc5616wa_mag_raw_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} mmc5616wa_fifo_sample_t;

typedef struct
{
    uint8_t level;
    bool full;
    bool empty;
} mmc5616wa_fifo_status_t;

typedef struct
{
    bool enable;
    bool interrupt_enable;
    bool address_loop_enable;
    uint8_t watermark;
} mmc5616wa_fifo_config_t;

typedef struct
{
    mmc5616wa_bandwidth_t bandwidth;
    uint8_t odr;
    bool auto_set_reset;
    bool continuous_mode;
    bool high_power;
    bool enable_2k_odr;
    bool periodic_set_enable;
    mmc5616wa_periodic_set_t periodic_set_rate;
} mmc5616wa_config_t;

int32_t mmc5616wa_read_reg(const mmc5616wa_ctx_t *ctx, const uint8_t reg,
                           uint8_t *data, const uint16_t len);
int32_t mmc5616wa_write_reg(const mmc5616wa_ctx_t *ctx, const uint8_t reg,
                            const uint8_t *data, const uint16_t len);

void mmc5616wa_default_config(mmc5616wa_config_t *config);
void mmc5616wa_default_fifo_config(mmc5616wa_fifo_config_t *config);

int32_t mmc5616wa_device_id_get(const mmc5616wa_ctx_t *ctx,
                                uint8_t *chip_id,
                                uint8_t *product_id);
int32_t mmc5616wa_reset(const mmc5616wa_ctx_t *ctx);
int32_t mmc5616wa_init(const mmc5616wa_ctx_t *ctx);

int32_t mmc5616wa_do_set(const mmc5616wa_ctx_t *ctx);
int32_t mmc5616wa_do_reset(const mmc5616wa_ctx_t *ctx);

int32_t mmc5616wa_config_set(const mmc5616wa_ctx_t *ctx,
                             const mmc5616wa_config_t *config);
int32_t mmc5616wa_config_get(const mmc5616wa_ctx_t *ctx,
                             mmc5616wa_config_t *config);

int32_t mmc5616wa_magnetic_measurement_get(const mmc5616wa_ctx_t *ctx,
                                           mmc5616wa_mag_raw_t *sample);
int32_t mmc5616wa_temperature_measurement_get(const mmc5616wa_ctx_t *ctx,
                                              uint8_t *raw_temperature);
int32_t mmc5616wa_self_test(const mmc5616wa_ctx_t *ctx, bool *passed);

int32_t mmc5616wa_fifo_config_set(const mmc5616wa_ctx_t *ctx,
                                  const mmc5616wa_fifo_config_t *config);
int32_t mmc5616wa_fifo_config_get(const mmc5616wa_ctx_t *ctx,
                                  mmc5616wa_fifo_config_t *config);
int32_t mmc5616wa_fifo_status_get(const mmc5616wa_ctx_t *ctx,
                                  mmc5616wa_fifo_status_t *status);
int32_t mmc5616wa_fifo_read(const mmc5616wa_ctx_t *ctx,
                            mmc5616wa_fifo_sample_t *samples,
                            const size_t max_samples,
                            size_t *samples_read);

float mmc5616wa_magnetic_20bit_to_gauss(const int32_t raw);
float mmc5616wa_magnetic_18bit_to_gauss(const int32_t raw);
float mmc5616wa_magnetic_16bit_to_gauss(const int16_t raw);
float mmc5616wa_temperature_to_celsius(const uint8_t raw);

#ifdef __cplusplus
}
#endif

#endif