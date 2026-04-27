# MMC5616WA

Generic I2C driver for the MEMSIC MMC5616WA 3-axis magnetometer.

## Files

- `mmc5616wa_reg.h`: Register map, bit masks, constants, and callback context type.
- `mmc5616wa.h`: Public API, driver data types, and conversion helpers.
- `mmc5616wa.c`: Driver implementation.
- `MMC5616WADatasheetv1.4.pdf`: Reference datasheet used for register definitions.

## Driver Model

The driver is platform-independent. The platform provides three callbacks through
`mmc5616wa_ctx_t`:

```c
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len);
int32_t platform_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len);
void platform_delay_ms(uint32_t delay_ms);
```

All read and write callbacks are expected to return `0` on success.

## Implemented Functionality

- Register map definitions for the documented I2C-visible registers.
- Device identification and software reset.
- Base initialization flow.
- Sensor configuration for bandwidth, ODR, automatic set/reset, and continuous mode.
- On-demand magnetic and temperature measurements.
- Self-test using the datasheet threshold procedure.
- FIFO configuration, FIFO status, and FIFO sample reads.
- Raw-to-gauss and raw-to-celsius conversion helpers.

## Quick Start

```c
#include "mmc5616wa.h"

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len);
static void platform_delay_ms(uint32_t delay_ms);

void mmc_example(void)
{
	uint8_t i2c_address = MMC5616WA_I2C_ADDR_7BIT;
	mmc5616wa_ctx_t dev = {
		.write_reg = platform_write,
		.read_reg = platform_read,
		.delay_ms = platform_delay_ms,
		.handle = &i2c_address,
	};
	mmc5616wa_config_t config;
	mmc5616wa_mag_raw_t sample;

	if (MMC5616WA_OK != mmc5616wa_init(&dev))
	{
		return;
	}

	mmc5616wa_default_config(&config);
	config.bandwidth = MMC5616WA_BW_3_5_MS;
	config.odr = 50U;
	config.auto_set_reset = true;
	config.continuous_mode = true;

	if (MMC5616WA_OK != mmc5616wa_config_set(&dev, &config))
	{
		return;
	}

	if (MMC5616WA_OK == mmc5616wa_magnetic_measurement_get(&dev, &sample))
	{
		float x_gauss = mmc5616wa_magnetic_20bit_to_gauss(sample.x);
		float y_gauss = mmc5616wa_magnetic_20bit_to_gauss(sample.y);
		float z_gauss = mmc5616wa_magnetic_20bit_to_gauss(sample.z);

		(void)x_gauss;
		(void)y_gauss;
		(void)z_gauss;
	}
}
```

## Self-Test

The self-test implementation follows the datasheet procedure:

1. Read the factory self-test values from registers `0x27`-`0x29`.
2. Program the thresholds at 80% of the factory values into `0x1E`-`0x20`.
3. Trigger self-test with `TM_M | AUTO_ST_EN`.
4. Read `Status1` and treat `Sat_sensor == 0` as pass.

Example:

```c
bool passed = false;

if (MMC5616WA_OK == mmc5616wa_self_test(&dev, &passed) && passed)
{
	/* Sensor self-test passed. */
}
```

## FIFO Notes

FIFO samples are exposed as 16-bit centered values because the datasheet defines
FIFO transfers as 6-byte `X0/X1/Y0/Y1/Z0/Z1` frames. Convert them to gauss with
`mmc5616wa_magnetic_16bit_to_gauss()`.

```c
mmc5616wa_fifo_config_t fifo_cfg = {
	.enable = true,
	.interrupt_enable = false,
	.address_loop_enable = false,
	.watermark = 4U,
};
mmc5616wa_fifo_sample_t fifo_samples[4];
size_t samples_read = 0U;

mmc5616wa_fifo_config_set(&dev, &fifo_cfg);
mmc5616wa_fifo_read(&dev, fifo_samples, 4U, &samples_read);
```

## Integration Notes

- `mmc5616wa_init()` performs a software reset, verifies chip and product IDs,
  applies a conservative default configuration, and disables FIFO.
- Continuous mode requires a non-zero `odr`.
- The driver preserves the ST-style callback pattern, but uses its own
  `mmc5616wa_ctx_t` type so it can stay standalone.
- The status bit layout in `Status1` follows the datasheet examples, which call
  `Meas_M_Done` bit 1 and `Sat_sensor` bit 2.
