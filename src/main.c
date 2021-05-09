/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <zephyr/types.h>
#include <settings/settings.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/byteorder.h>
#include <soc.h>
#include "bluetooth.h"
#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LENGTH     (sizeof(DEVICE_NAME) -1)
uint8_t data[2] = {0, 10};

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LENGTH),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_DATA_VAL),
};


static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err %u)\n", err);
		return;
	}

	printk("Connected\n");

}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason %u)\n", reason);

}
static struct bt_conn_cb conn_callbacks = {
	.connected        = connected,
	.disconnected     = disconnected,
};


static uint8_t* app_data_cb(void)
{
	return &data;
}

static struct bt_dataService_cb data_callbacs = {
	.data_cb    = app_data_cb,
};

void main(void)
{
  
	int err;
  
  bt_conn_cb_register(&conn_callbacks);

    err = bt_enable(NULL);
    if (err) {
      printk("Bluetooth init failed (err %d)\n", err);
      return;
    }

    printk("Bluetooth initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
      settings_load();
    }

    err = bt_dataService_init(&data_callbacs);
    if (err) {
      printk("Failed to init LBS (err:%d)\n", err);
      return;
    }

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
              sd, ARRAY_SIZE(sd));
    if (err) {
      printk("Advertising failed to start (err %d)\n", err);
      return;
    }

    printk("Advertising successfully started\n");

 while(1){
     data[0]++;
     data[1]++;
     printk("Data %i %i\n", data[0], data[1]);
     bt_send_data_value(data);
     k_sleep(K_SECONDS(2));
 }
}