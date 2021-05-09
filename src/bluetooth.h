#include <zephyr/types.h>

//UUID service declaration
#define BT_UUID_SERVICE_VAL BT_UUID_128_ENCODE(0x00001526, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
#define BT_UUID_SERVICE BT_UUID_DECLARE_128 (BT_UUID_SERVICE_VAL)

//UUID data characteristic declaration
#define BT_UUID_DATA_VAL BT_UUID_128_ENCODE(0x00001527, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
#define BT_UUID_DATA BT_UUID_DECLARE_128 (BT_UUID_DATA_VAL)

//Callback declaration
typedef uint8_t* (*data_cb_t) (void);
struct bt_dataService_cb{data_cb_t data_cb;};

int bt_dataService_init (struct bt_dataService_cb *callbacks);
int bt_send_data_value(uint8_t* data);