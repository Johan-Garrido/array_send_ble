#include <sys/printk.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include "bluetooth.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <logging/log.h>

static struct bt_dataService_cb dataService_cb;
static                         uint8_t data;
static bool                   notify_enabled;
static void lbslc_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				  uint16_t value)
{
	notify_enabled = (value == BT_GATT_CCC_NOTIFY);
}

static ssize_t read_button(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr,
			  void *buf,
			  uint16_t len,
			  uint16_t offset)
{
	const char *value = attr->user_data;

//	LOG_DBG("Attribute read, handle: %u, conn: %p", attr->handle, conn);

	if (dataService_cb.data_cb) {
		data = dataService_cb.data_cb();
		//memcpy(data, buf, len);
		return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
					 sizeof(*value));
	}

	return 0;
}

BT_GATT_SERVICE_DEFINE(data_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),
	BT_GATT_CHARACTERISTIC(BT_UUID_DATA,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, read_button, NULL,
			       &data),
        BT_GATT_CCC(lbslc_ccc_cfg_changed,
                               BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

int bt_dataService_init(struct bt_dataService_cb *callbacks)
{
	if (callbacks) {
		dataService_cb.data_cb = callbacks->data_cb;
	}

	return 0;
}
int bt_send_data_value(uint8_t data)
{
	if (!notify_enabled) {
		return -EACCES;
	}

	return bt_gatt_notify(NULL, &data_svc.attrs[2],
			      &data,
			      sizeof(data));
}
