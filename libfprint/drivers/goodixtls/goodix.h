/*
 * Goodix Tls driver for libfprint
 *
 * Copyright (C) 2021 Alexander Meiler <alex.meiler@protonmail.com>
 * Copyright (C) 2021 Matthieu CHARETTE <matthieu.charette@gmail.com>
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

#include <glib.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <stdint.h>
#include "drivers_api.h"
#include "goodix_proto.h"
#include "goodixtls.h"

#define GOODIX_INTERFACE (0)
#define GOODIX_EP_OUT (0x1)
#define GOODIX_EP_IN (0x81)

// 1 seconds USB timeout
#define GOODIX_TIMEOUT (1000)

#define GOODIX_FIRMWARE_VERSION_SUPPORTED "GF_ST411SEC_APP_12109"

#define GOODIX_PSK_LEN (32)

guint8 zero_psk_hash[] = {0xba, 0x1a, 0x86, 0x03, 0x7c, 0x1d, 0x3c, 0x71,
                          0xc3, 0xaf, 0x34, 0x49, 0x55, 0xbd, 0x69, 0xa9,
                          0xa9, 0x86, 0x1d, 0x9e, 0x91, 0x1f, 0xa2, 0x49,
                          0x85, 0xb6, 0x77, 0xe8, 0xdb, 0xd7, 0x2d, 0x43};

guint8 device_config[] = {
    0x70, 0x11, 0x60, 0x71, 0x2c, 0x9d, 0x2c, 0xc9, 0x1c, 0xe5, 0x18, 0xfd,
    0x00, 0xfd, 0x00, 0xfd, 0x03, 0xba, 0x00, 0x01, 0x80, 0xca, 0x00, 0x04,
    0x00, 0x84, 0x00, 0x15, 0xb3, 0x86, 0x00, 0x00, 0xc4, 0x88, 0x00, 0x00,
    0xba, 0x8a, 0x00, 0x00, 0xb2, 0x8c, 0x00, 0x00, 0xaa, 0x8e, 0x00, 0x00,
    0xc1, 0x90, 0x00, 0xbb, 0xbb, 0x92, 0x00, 0xb1, 0xb1, 0x94, 0x00, 0x00,
    0xa8, 0x96, 0x00, 0x00, 0xb6, 0x98, 0x00, 0x00, 0x00, 0x9a, 0x00, 0x00,
    0x00, 0xd2, 0x00, 0x00, 0x00, 0xd4, 0x00, 0x00, 0x00, 0xd6, 0x00, 0x00,
    0x00, 0xd8, 0x00, 0x00, 0x00, 0x50, 0x00, 0x01, 0x05, 0xd0, 0x00, 0x00,
    0x00, 0x70, 0x00, 0x00, 0x00, 0x72, 0x00, 0x78, 0x56, 0x74, 0x00, 0x34,
    0x12, 0x20, 0x00, 0x10, 0x40, 0x2a, 0x01, 0x02, 0x04, 0x22, 0x00, 0x01,
    0x20, 0x24, 0x00, 0x32, 0x00, 0x80, 0x00, 0x01, 0x00, 0x5c, 0x00, 0x80,
    0x00, 0x56, 0x00, 0x04, 0x20, 0x58, 0x00, 0x03, 0x02, 0x32, 0x00, 0x0c,
    0x02, 0x66, 0x00, 0x03, 0x00, 0x7c, 0x00, 0x00, 0x58, 0x82, 0x00, 0x80,
    0x15, 0x2a, 0x01, 0x82, 0x03, 0x22, 0x00, 0x01, 0x20, 0x24, 0x00, 0x14,
    0x00, 0x80, 0x00, 0x01, 0x00, 0x5c, 0x00, 0x00, 0x01, 0x56, 0x00, 0x04,
    0x20, 0x58, 0x00, 0x03, 0x02, 0x32, 0x00, 0x0c, 0x02, 0x66, 0x00, 0x03,
    0x00, 0x7c, 0x00, 0x00, 0x58, 0x82, 0x00, 0x80, 0x1f, 0x2a, 0x01, 0x08,
    0x00, 0x5c, 0x00, 0x80, 0x00, 0x54, 0x00, 0x10, 0x01, 0x62, 0x00, 0x04,
    0x03, 0x64, 0x00, 0x19, 0x00, 0x66, 0x00, 0x03, 0x00, 0x7c, 0x00, 0x01,
    0x58, 0x2a, 0x01, 0x08, 0x00, 0x5c, 0x00, 0x00, 0x01, 0x52, 0x00, 0x08,
    0x00, 0x54, 0x00, 0x00, 0x01, 0x66, 0x00, 0x03, 0x00, 0x7c, 0x00, 0x01,
    0x58, 0x00, 0x89, 0x2e};

G_DECLARE_FINAL_TYPE(FpiDeviceGoodixTls, fpi_device_goodixtls, FPI,
                     DEVICE_GOODIXTLS, FpImageDevice);

static const FpIdEntry id_table[] = {
    {.vid = 0x27c6, .pid = 0x5110},
    {.vid = 0, .pid = 0, .driver_data = 0},
};

gchar *data_to_str(gpointer data, gsize data_len);

/* ---- GOODIX SECTION START ---- */

static void goodix_receive_data(FpiSsm *ssm, FpDevice *dev, guint timeout);

static void goodix_cmd_done(FpiSsm *ssm, FpDevice *dev, guint8 cmd);

static void goodix_ack_handle(FpiSsm *ssm, FpDevice *dev, gpointer data,
                              gsize data_len, GDestroyNotify data_destroy);

static void goodix_protocol_handle(FpiSsm *ssm, FpDevice *dev, gpointer data,
                                   gsize data_len, GDestroyNotify data_destroy);

static void goodix_pack_handle(FpiSsm *ssm, FpDevice *dev, gpointer data,
                               gsize data_len, GDestroyNotify data_destroy);

static void goodix_receive_data_cb(FpiUsbTransfer *transfer, FpDevice *dev,
                                   gpointer user_data, GError *error);

static void goodix_send_pack(FpiSsm *ssm, FpDevice *dev, guint8 flags,
                             gpointer payload, guint16 payload_len,
                             GDestroyNotify destroy);

static void goodix_send_protocol(FpiSsm *ssm, FpDevice *dev, guint8 cmd,
                                 gboolean calc_checksum, gpointer payload,
                                 guint16 payload_len, GDestroyNotify destroy);

static void goodix_cmd_nop(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_mcu_get_image(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_mcu_switch_to_fdt_down(FpiSsm *ssm, FpDevice *dev,
                                              gpointer mode, guint16 mode_len,
                                              GDestroyNotify destroy);

static void goodix_cmd_mcu_switch_to_fdt_up(FpiSsm *ssm, FpDevice *dev,
                                            gpointer mode, guint16 mode_len,
                                            GDestroyNotify destroy);

static void goodix_cmd_mcu_switch_to_fdt_mode(FpiSsm *ssm, FpDevice *dev,
                                              gpointer mode, guint16 mode_len,
                                              GDestroyNotify destroy);

static void goodix_cmd_nav_0(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_mcu_switch_to_idle_mode(FpiSsm *ssm, FpDevice *dev,
                                               guint8 sleep_time);

static void goodix_cmd_write_sensor_register(FpiSsm *ssm, FpDevice *dev,
                                             guint16 address, guint16 value);

static void goodix_cmd_read_sensor_register(FpiSsm *ssm, FpDevice *dev,
                                            guint16 address, guint8 length);

static void goodix_cmd_upload_config_mcu(FpiSsm *ssm, FpDevice *dev,
                                         gpointer config, guint16 config_len,
                                         GDestroyNotify destroy);

static void goodix_cmd_set_powerdown_scan_frequency(
    FpiSsm *ssm, FpDevice *dev, guint16 powerdown_scan_frequency);

static void goodix_cmd_enable_chip(FpiSsm *ssm, FpDevice *dev, gboolean enable);

static void goodix_cmd_reset(FpiSsm *ssm, FpDevice *dev, gboolean reset_sensor,
                             gboolean soft_reset_mcu, guint8 sleep_time);

static void goodix_cmd_firmware_version(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_query_mcu_state(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_request_tls_connection(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_tls_successfully_established(FpiSsm *ssm, FpDevice *dev);

static void goodix_cmd_preset_psk_write_r(FpiSsm *ssm, FpDevice *dev,
                                          guint32 address, gpointer psk,
                                          guint32 psk_len,
                                          GDestroyNotify destroy);

static void goodix_cmd_preset_psk_read_r(FpiSsm *ssm, FpDevice *dev,
                                         guint32 address, guint32 length);

/* ---- GOODIX SECTION END ---- */

/* ------------------------------------------------------------------------- */

/* ---- TLS SECTION START ---- */

static void tls_run_state(FpiSsm *ssm, FpDevice *dev);

static void tls_complete(FpiSsm *ssm, FpDevice *dev, GError *error);

static void goodix_tls(FpImageDevice *dev);

/* ---- TLS SECTION END ---- */

/* ------------------------------------------------------------------------- */

/* ---- ACTIVE SECTION START ---- */

static void activate_run_state(FpiSsm *ssm, FpDevice *dev);

static void activate_complete(FpiSsm *ssm, FpDevice *dev, GError *error);

/* ---- ACTIVE SECTION END ---- */

/* ------------------------------------------------------------------------- */

/* ---- DEV SECTION START ---- */

static void dev_init(FpImageDevice *dev);

static void dev_deinit(FpImageDevice *dev);

static void dev_activate(FpImageDevice *dev);

static void dev_change_state(FpImageDevice *dev, FpiImageDeviceState state);

static void dev_deactivate(FpImageDevice *dev);

/* ---- DEV SECTION END ---- */
