/*
 * Elan SPI driver for libfprint
 *
 * Copyright (C) 2021 Matthew Mirvish <matthew@mm12.xyz>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#pragma once

#include <config.h>

#ifndef HAVE_UDEV
#error "elanspi requires udev"
#endif

#include <fp-device.h>
#include <fpi-device.h>

#define ELANSPI_TP_PID 0x04f3

/* Sensor ID information copied from the windows driver */

struct elanspi_sensor_entry
{
  unsigned char sensor_id, height, width, ic_version;
  gboolean      is_otp_model;
  const gchar * name;
};

static const struct elanspi_sensor_entry elanspi_sensor_table[] = {
  {0x0, 0x78, 0x78, 0x0, 0x0, "eFSA120S"},
  {0x1, 0x78, 0x78, 0x1, 0x1, "eFSA120SA"},
  {0x2, 0xA0, 0xA0, 0x0, 0x0, "eFSA160S"},
  {0x3, 0xd0, 0x50, 0x0, 0x0, "eFSA820R"},
  {0x4, 0xC0, 0x38, 0x0, 0x0, "eFSA519R"},
  {0x5, 0x60, 0x60, 0x0, 0x0, "eFSA96S"},
  {0x6, 0x60, 0x60, 0x1, 0x1, "eFSA96SA"},
  {0x7, 0x60, 0x60, 0x2, 0x1, "eFSA96SB"},
  {0x8, 0xa0, 0x50, 0x1, 0x1, "eFSA816RA"},
  {0x9, 0x90, 0x40, 0x1, 0x1, "eFSA614RA"},
  {0xA, 0x90, 0x40, 0x2, 0x1, "eFSA614RB"},
  {0xB, 0x40, 0x58, 0x1, 0x1, "eFSA688RA"},
  {0xC, 0x50, 0x50, 0x1, 0x0, "eFSA80SA"},
  {0xD, 0x47, 0x80, 0x1, 0x1, "eFSA712RA"},
  {0xE, 0x50, 0x50, 0x2, 0x0, "eFSA80SC"},
  {0,   0,    0,    0,   0,   NULL}
};

struct elanspi_reg_entry
{
  unsigned char addr, value;
  /* terminates with 0xFF, 0xFF since register 0x0 is valid */
};

struct elanspi_regtable
{
  const struct elanspi_reg_entry *other;
  struct
  {
    unsigned char                   sid;
    const struct elanspi_reg_entry *table;
  } entries[];
};

static const struct elanspi_reg_entry elanspi_calibration_table_default[] = {
  {0x05, 0x60},
  {0x06, 0xc0},
  {0x07, 0x80},
  {0x08, 0x04},
  {0x0a, 0x97},
  {0x0b, 0x72},
  {0x0c, 0x69},
  {0x0f, 0x2a},
  {0x11, 0x2a},
  {0x13, 0x27},
  {0x15, 0x67},
  {0x18, 0x04},
  {0x21, 0x20},
  {0x22, 0x36},
  {0x2a, 0x5f},
  {0x2b, 0xc0},
  {0x2e, 0xff},

  {0xff, 0xff}
};

static const struct elanspi_reg_entry elanspi_calibration_table_id567[] = {
  {0x2A, 0x07},
  {0x5,  0x60},
  {0x6,  0xC0},
  {0x7,  0x80},
  {0x8,  0x04},
  {0xA,  0x97},
  {0xB,  0x72},
  {0xC,  0x69},
  {0xF,  0x2A},
  {0x11, 0x2A},
  {0x13, 0x27},
  {0x15, 0x67},
  {0x18, 0x04},
  {0x21, 0x20},
  {0x22, 0x36},
  {0x2A, 0x5F},
  {0x2B, 0xC0},
  {0x2E, 0xFF},

  {0xff, 0xff}
};

static const struct elanspi_reg_entry elanspi_calibration_table_id0[] = {
  {0x5,  0x60},
  {0x6,  0xC0},
  {0x8,  0x04},
  {0xA,  0x97},
  {0xB,  0x72},
  {0xC,  0x69},
  {0xF,  0x2B},
  {0x11, 0x2B},
  {0x13, 0x28},
  {0x15, 0x28},
  {0x18, 0x04},
  {0x21, 0x20},
  {0x2A, 0x4B},

  {0xff, 0xff}
};

// old style sensor calibration, with only one page of registers
static const struct elanspi_regtable elanspi_calibration_table_old = {
  .other = elanspi_calibration_table_default,
  .entries = {
    { .sid = 0x0, .table = elanspi_calibration_table_id0 },
    { .sid = 0x5, .table = elanspi_calibration_table_id567 },
    { .sid = 0x6, .table = elanspi_calibration_table_id567 },
    { .sid = 0x7, .table = elanspi_calibration_table_id567 },
    { .sid = 0x0, .table = NULL }
  }
};

// new style sensor calibration, with two pages of registers
static const struct elanspi_reg_entry elanspi_calibration_table_page0_id14[] = {
  {0x00, 0x5a},
  {0x01, 0x00},
  {0x02, 0x4f},
  {0x03, 0x00},
  {0x04, 0x4f},
  {0x05, 0xa0},
  {0x06, 0x00},
  {0x07, 0x00},
  {0x08, 0x00},
  {0x09, 0x04},
  {0x0a, 0x74},
  {0x0b, 0x05},
  {0x0c, 0x08},
  {0x0d, 0x00},
  {0x0e, 0x00},
  {0x0f, 0x14},
  {0x10, 0x3c},
  {0x11, 0x41},
  {0x12, 0x0c},
  {0x13, 0x00},
  {0x14, 0x00},
  {0x15, 0x04},
  {0x16, 0x02},
  {0x17, 0x00},
  {0x18, 0x01},
  {0x19, 0xf4},
  {0x1a, 0x00},
  {0x1b, 0x00},
  {0x1c, 0x00},
  {0x1d, 0x00},
  {0x1e, 0x00},
  {0x1f, 0x00},
  {0x20, 0x00},
  {0x21, 0x80},
  {0x22, 0x06},
  {0x23, 0x00},
  {0x24, 0x00},
  {0x25, 0x00},
  {0x26, 0x00},
  {0x27, 0x00},
  {0x28, 0x00},
  {0x29, 0x04},
  {0x2a, 0x5f},
  {0x2b, 0xe2},
  {0x2c, 0xa0},
  {0x2d, 0x00},
  {0x2e, 0xff},
  {0x2f, 0x40},
  {0x30, 0x01},
  {0x31, 0x38},
  {0x32, 0x00},
  {0x33, 0x00},
  {0x34, 0x00},
  {0x35, 0x1f},
  {0x36, 0xff},
  {0x37, 0x00},
  {0x38, 0x00},
  {0x39, 0x00},
  {0x3a, 0x00},
  {0xff, 0xff}
};

static const struct elanspi_reg_entry elanspi_calibration_table_page1_id14[] = {
  {0x00, 0x7b},
  {0x01, 0x7f},
  {0x02, 0x77},
  {0x03, 0xd4},
  {0x04, 0x7d},
  {0x05, 0x19},
  {0x06, 0x80},
  {0x07, 0x40},
  {0x08, 0x11},
  {0x09, 0x00},
  {0x0a, 0x00},
  {0x0b, 0x14},
  {0x0c, 0x00},
  {0x0d, 0x00},
  {0x0e, 0x32},
  {0x0f, 0x02},
  {0x10, 0x08},
  {0x11, 0x6c},
  {0x12, 0x00},
  {0x13, 0x00},
  {0x14, 0x32},
  {0x15, 0x01},
  {0x16, 0x16},
  {0x17, 0x01},
  {0x18, 0x14},
  {0x19, 0x01},
  {0x1a, 0x16},
  {0x1b, 0x01},
  {0x1c, 0x17},
  {0x1d, 0x01},
  {0x1e, 0x0a},
  {0x1f, 0x01},
  {0x20, 0x0a},
  {0x21, 0x02},
  {0x22, 0x08},
  {0x23, 0x29},
  {0x24, 0x00},
  {0x25, 0x0c},
  {0x26, 0x1a},
  {0x27, 0x30},
  {0x28, 0x1a},
  {0x29, 0x30},
  {0x2a, 0x00},
  {0x2b, 0x00},
  {0x2c, 0x01},
  {0x2d, 0x16},
  {0x2e, 0x01},
  {0x2f, 0x17},
  {0x30, 0x03},
  {0x31, 0x2d},
  {0x32, 0x03},
  {0x33, 0x2d},
  {0x34, 0x14},
  {0x35, 0x00},
  {0x36, 0x00},
  {0x37, 0x00},
  {0x38, 0x00},
  {0x39, 0x03},
  {0x3a, 0xfe},
  {0x3b, 0x00},
  {0x3c, 0x00},
  {0x3d, 0x02},
  {0x3e, 0x00},
  {0x3f, 0x00},
  {0xff, 0xff}
};

static const struct elanspi_regtable elanspi_calibration_table_new_page0 = {
  .other = NULL,
  .entries = {
    { .sid = 0xe, .table = elanspi_calibration_table_page0_id14 },
    { .sid = 0x0, .table = NULL }
  }
};

static const struct elanspi_regtable elanspi_calibration_table_new_page1 = {
  .other = NULL,
  .entries = {
    { .sid = 0xe, .table = elanspi_calibration_table_page1_id14 },
    { .sid = 0x0, .table = NULL }
  }
};

#define ELANSPI_NO_ROTATE 0
#define ELANSPI_90LEFT_ROTATE 1
#define ELANSPI_180_ROTATE 2
#define ELANSPI_90RIGHT_ROTATE 3

#define ELANSPI_HV_FLIPPED 1

#define ELANSPI_UDEV_TYPES FPI_DEVICE_UDEV_SUBTYPE_SPIDEV | FPI_DEVICE_UDEV_SUBTYPE_HIDRAW
#define ELANSPI_TP_VID 0x04f3

// using checkargs ACPI:HIDPID
static const FpIdEntry elanspi_id_table[] = {
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN7001", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x3057}, .driver_data = ELANSPI_180_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN7001", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x3087}, .driver_data = ELANSPI_180_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN7001", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x30c6}, .driver_data = ELANSPI_180_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN70A1", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x3134}, .driver_data = ELANSPI_90LEFT_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN7001", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x3148}, .driver_data = ELANSPI_180_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN7001", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x30b2}, .driver_data = ELANSPI_NO_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN70A1", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x30b2}, .driver_data = ELANSPI_NO_ROTATE},
  {.udev_types = ELANSPI_UDEV_TYPES, .spi_acpi_id = "ELAN7001", .hid_id = {.vid = ELANSPI_TP_VID, .pid = 0x309f}, .driver_data = ELANSPI_180_ROTATE},
  {.udev_types = 0}
};

#define ELANSPI_MAX_OLD_STAGE1_CALIBRATION_MEAN 1000

#define ELANSPI_MIN_OLD_STAGE2_CALBIRATION_MEAN 3000
#define ELANSPI_MAX_OLD_STAGE2_CALBIRATION_MEAN 8000

#define ELANSPI_HV_CALIBRATION_TARGET_MEAN 3000

#define ELANSPI_MIN_EMPTY_INVALID_PERCENT 6
#define ELANSPI_MAX_REAL_INVALID_PERCENT 3

#define ELANSPI_MIN_REAL_STDDEV (592 * 592)
#define ELANSPI_MAX_EMPTY_STDDEV (350 * 350)

#define ELANSPI_MIN_FRAMES_DEBOUNCE 2

#define ELANSPI_SWIPE_FRAMES_DISCARD 1
#define ELANSPI_MIN_FRAMES_SWIPE (7 + ELANSPI_SWIPE_FRAMES_DISCARD)
#define ELANSPI_MAX_FRAMES_SWIPE (20 + ELANSPI_SWIPE_FRAMES_DISCARD)

#define ELANSPI_MAX_FRAME_HEIGHT 43
#define ELANSPI_MIN_FRAME_TO_FRAME_DIFF (250 * 250)

#define ELANSPI_HV_SENSOR_FRAME_DELAY 23

#define ELANSPI_OTP_TIMEOUT_USEC (12 * 1000)

#define ELANSPI_OLD_CAPTURE_TIMEOUT_USEC (100 * 1000)
#define ELANSPI_HV_CAPTURE_TIMEOUT_USEC (50 * 1000)
