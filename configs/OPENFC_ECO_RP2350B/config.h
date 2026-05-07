/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// OpenFC ECO V0.3 — stripped variant of OpenFC.
// 30x30, 6-layer, 3S-6S. RP2354B (RP2350B-pin-compatible, 2MB internal flash).
// No baro, no onboard ELRS, no onboard WS2812, no SPI flash.
// Blackbox is on a microSD slot (TF-021B-H265) on SPI1.

#define FC_TARGET_MCU     RP2350B
#define BOARD_NAME        OPENFC_ECO_RP2350B
#define MANUFACTURER_ID   OPFC

// Overclock to 250 MHz @ VDD_CORE 1.15V.
// RP2350 stock is 150 MHz; 250 MHz is widely reported stable in this band,
// stays inside the pico-sdk VREG range (1.30V hard limit), and is a clean
// 5x50 multiple so PLL_USB stays untouched. Cuts CPU load ~40%.
#define PICO_OVERCLOCK_KHZ 250000

// Enable USE_CHIRP — the BF chirp signal-injection path used for system ID.
// Pilot triggers via BOXCHIRP aux switch; setpoint is excited with an
// exponential frequency sweep on one axis at a time, gyro response captured
// to blackbox via DEBUG_CHIRP debug mode. Foundation for OpenFC's on-board
// post-flight PID identifier (Phase 2). Bench-safe when disarmed (no motor
// output); in-flight chirp is intentional per upstream BF design. Set
// `set debug_mode = CHIRP` and assign BOXCHIRP to an aux switch to use.
#define USE_CHIRP

// ------------------
// Motor outputs (PIO DSHOT)
// V0.3 hardware wires the ESC connector M1..M4 to GPIO31..GPIO28
// (reversed vs the full OpenFC). Mirror it here so the config file
// stays canonical and Configurator does not need a remap.
// ------------------

#define MOTOR1_PIN           PA31
#define MOTOR2_PIN           PA30
#define MOTOR3_PIN           PA29
#define MOTOR4_PIN           PA28

// ------------------
// Status LED + LED strip
// ------------------

#define LED0_PIN             PA12
#define LED_STRIP_PIN        PA23

// ------------------
// Buzzer
// ------------------

#define USE_BEEPER
#define BEEPER_PWM_HZ        1971
#define BEEPER_PIN           PA6

// ------------------
// Serial ports
// UART0: digital VTX / MSP DisplayPort
// UART1: external serial RX (CRSF/SBUS), no onboard receiver
// PIOUART0: GPS (default)
// PIOUART1: spare
// ------------------

#define UART0_TX_PIN         PA0
#define UART0_RX_PIN         PA1
#define MSP_DISPLAYPORT_UART SERIAL_PORT_UART0

// HD digital OSD canvas via MSP DisplayPort (DJI O4, HDZero, Walksnail).
// PICO RP2350B target.h gates `#undef USE_OSD_HD` on BOARD_USE_OSD_HD; we
// opt in here because UART0 is wired to a digital VTX.
#define BOARD_USE_OSD_HD

#define UART1_TX_PIN         PA22
#define UART1_RX_PIN         PA21

#define PIOUART0_TX_PIN      PA2
#define PIOUART0_RX_PIN      PA3
#define GPS_UART             SERIAL_PORT_PIOUART0

#define PIOUART1_TX_PIN      PA26
#define PIOUART1_RX_PIN      PA27

// ------------------
// SPI0: IMU (ICM-42688-P)
// LSM6DSV16XTR was originally specified but found to have a MEMS resonance
// vulnerability in the ~25 kRPM motor band on this airframe (the chip
// internal gyro saturates from acoustic/vibration energy at the resonator's
// drive frequency, producing bursts of false rotation reads up to chip
// full-scale ±2000 dps). Swapping to ICM-42688-P (drop-in pin-compatible
// per ST/TDK datasheets and confirmed by multiple BF targets that support
// both chips on the same pads) eliminated the resonance issue. V0.4 PCB
// will be designed with ICM-42688-P only.
// ------------------

#define SPI0_SCK_PIN         PA18
#define SPI0_SDI_PIN         PA20 // MISO
#define SPI0_SDO_PIN         PA19 // MOSI

#define USE_GYRO
#define USE_ACC
#define USE_GYRO_SPI_ICM42688P
#define USE_ACC_SPI_ICM42688P
#define GYRO_1_SPI_INSTANCE  SPI0
#define GYRO_1_CS_PIN        PA14
#define GYRO_1_EXTI_PIN      PA13

// IMU footprint on V0.3 is rotated 180 degrees about the pitch axis
// relative to BF's default body frame. Bake the correction in so the
// board boots correctly out of the box (verified on V0.3 hardware).
#define DEFAULT_ALIGN_BOARD_PITCH 180

// ------------------
// SPI1: microSD blackbox (TF-021B-H265)
// Replaces the SPI flash chip on the full OpenFC.
// ------------------

#define SPI1_SCK_PIN         PA42
#define SPI1_SDI_PIN         PA44 // MISO
#define SPI1_SDO_PIN         PA43 // MOSI

#define USE_SDCARD
#define USE_SDCARD_SPI
#define SDCARD_SPI_INSTANCE  SPI1
#define SDCARD_SPI_CS_PIN    PA46
#define DEFAULT_BLACKBOX_DEVICE BLACKBOX_DEVICE_SDCARD

// ------------------
// I2C0: external expansion only (SDA/SCL on J7/J8 pads).
// No onboard baro/mag on the ECO variant.
// ------------------

#define I2C0_SDA_PIN         PA16
#define I2C0_SCL_PIN         PA17

// ------------------
// ADC (RP2350B bank = PA40..PA47)
// VBAT divider on the board is 100k/10k => 11:1.
// ------------------

#define USE_ADC
#define ADC_VBAT_PIN         PA41
#define ADC_CURR_PIN         PA40
#define ADC_RSSI_PIN         PA45
#define ADC_EXTERNAL1_PIN    PA47

#define DEFAULT_VOLTAGE_METER_SOURCE VOLTAGE_METER_ADC
#define DEFAULT_VOLTAGE_METER_SCALE  110

// ------------------
// PINIO: 10V VTX rail enable (active high, LMR51420 EN on GPIO11).
// Lets the user gate the 10V rail from CLI / OSD / mode.
// ------------------

#define USE_PINIO
#define PINIO1_PIN           PA11
#define PINIO1_CONFIG        1      // OUT_PP, active high, default off (LMR51420 EN is active high)
#define PINIO1_BOX           40     // USER1

// ------------------
// FB_OSD (analog OSD via PIO + sync-sep + SPDT mux).
// Hardware: TLV3201 sync sep -> PA35, TLV9061 driver -> PA33,
// SN74LVC1G3157 white/black select via PA34.
// PIO platform requires OSD_W/EN/SYNC to be on three consecutive GPIOs.
// Disabled until the analog OSD chain is verified on hardware.
// Uncomment the ENABLE_FB_OSD line to turn it on once validated.
// ------------------
//
// #define ENABLE_FB_OSD        1
// #define OSD_W_PIN            PA33
// #define OSD_EN_PIN           PA34
// #define OSD_SYNC_PIN         PA35
