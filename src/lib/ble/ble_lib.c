/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ble_lib.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 BLE.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <bluetooth/services/nus.h>

#include <stdio.h>

#include <lib/ble/ble_lib.h>
#include <lib/ble/ble_uart.h>

LOG_MODULE_REGISTER(LOG_BLE_LIB);


#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN	(sizeof(DEVICE_NAME) - 1)


static const struct bt_data ad[] = {
   BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
   BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
   BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

static struct bt_conn *current_conn;
static struct bt_conn *auth_conn;
static bool connection_status = false;


static void ble_lib_connected(struct bt_conn *conn, uint8_t ret)
{
   char addr[BT_ADDR_LE_STR_LEN];

   if (ret)
   {
      LOG_ERR("Connection failed, ret %d", (int32_t)ret);
      return;
   }

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
   LOG_INF("Connected %s", addr);

   current_conn = bt_conn_ref(conn);

   connection_status = true;
}

static void ble_lib_disconnected(struct bt_conn *conn, uint8_t reason)
{
   char addr[BT_ADDR_LE_STR_LEN];

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   LOG_INF("Disconnected: %s, reason %d", addr, (int32_t)reason);

   if (auth_conn)
   {
      bt_conn_unref(auth_conn);
      auth_conn = NULL;
   }

   if (current_conn)
   {
      bt_conn_unref(current_conn);
      current_conn = NULL;
   }

   connection_status = false;
}

#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
static void ble_lib_security_changed(struct bt_conn *conn, bt_security_t level,
   enum bt_security_err ret)
{
   char addr[BT_ADDR_LE_STR_LEN];

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   if (!ret) {
      LOG_INF("Security changed: %s level %u", addr, level);
   }
   else {
      LOG_WRN("Security failed: %s level %u ret %d", addr, level, ret);
   }
}
#endif

BT_CONN_CB_DEFINE(conn_callbacks) = {
   .connected    = ble_lib_connected,
   .disconnected = ble_lib_disconnected,
#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
   .security_changed = ble_lib_security_changed,
#endif
};

#if defined(CONFIG_BT_NUS_SECURITY_ENABLED)
static void ble_lib_auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
   char addr[BT_ADDR_LE_STR_LEN];

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   LOG_INF("Passkey for %s: %06u", addr, passkey);
}

static void ble_lib_auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
   char addr[BT_ADDR_LE_STR_LEN];

   auth_conn = bt_conn_ref(conn);

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   LOG_INF("Passkey for %s: %06u", addr, passkey);
   LOG_INF("Press Button 1 to confirm, Button 2 to reject.");
}

static void ble_lib_auth_cancel(struct bt_conn *conn)
{
   char addr[BT_ADDR_LE_STR_LEN];

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   LOG_INF("Pairing cancelled: %s", addr);
}

static void ble_lib_pairing_complete(struct bt_conn *conn, bool bonded)
{
   char addr[BT_ADDR_LE_STR_LEN];

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   LOG_INF("Pairing completed: %s, bonded: %d", addr, bonded);
}

static void ble_lib_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
   char addr[BT_ADDR_LE_STR_LEN];

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

   LOG_INF("Pairing failed conn: %s, reason %d", addr, reason);
}

static struct bt_conn_auth_cb conn_auth_callbacks = {
   .passkey_display = ble_lib_auth_passkey_display,
   .passkey_confirm = ble_lib_auth_passkey_confirm,
   .cancel = ble_lib_auth_cancel,
};

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {
   .pairing_complete = ble_lib_pairing_complete,
   .pairing_failed = ble_lib_pairing_failed
};
#else
static struct bt_conn_auth_cb conn_auth_callbacks;
static struct bt_conn_auth_info_cb conn_auth_info_callbacks;
#endif

bool ble_lib_get_connection_status(void)
{
   return connection_status;
}

int32_t ble_lib_adv_start(void)
{
   int32_t ret;

   ret = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
   if (ret)
   {
      LOG_ERR("Failed to start advertising, err %d", ret);
      return ret;
   }

   return 0;
}

int32_t ble_lib_adv_stop(void)
{
   int32_t ret;

   ret = bt_le_adv_stop();
   if (ret)
   {
      LOG_ERR("Failed to stop advertising, err %d", ret);
      return ret;
   }

   return 0;
}

int32_t ble_lib_init(void)
{
   int ret;

   if (IS_ENABLED(CONFIG_BT_NUS_SECURITY_ENABLED))
   {
      ret = bt_conn_auth_cb_register(&conn_auth_callbacks);
      if (ret)
      {
         printk("Failed to register authorization callbacks.\n");
         return ret;
      }

      ret = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
      if (ret)
      {
         printk("Failed to register authorization info callbacks.\n");
         return ret;
      }
   }

   ret = bt_enable(NULL);
   if (ret)
   {
      LOG_ERR("Failed to initialize BLE (ret: %d)", ret);
      return ret;
   }

   LOG_INF("Bluetooth initialized");

   if (IS_ENABLED(CONFIG_SETTINGS)) {
      settings_load();
   }

   ret = ble_uart_init();
   if (ret != 0) {
      return ret;
   }

   ret = ble_lib_adv_start();
   if (ret != 0) {
      return ret;
   }

   return 0;
}
