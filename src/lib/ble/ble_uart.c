/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ble_uart.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 BLE NUS.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>

#include <bluetooth/services/nus.h>

#include <stdio.h>

#include <lib/ble/ble_uart.h>
#include <lib/uart/uart_lib.h>

LOG_MODULE_REGISTER(LOG_BLE_UART);


#define STACKSIZE    CONFIG_BT_NUS_THREAD_STACK_SIZE
#define PRIORITY     7


static void ble_uart_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
   int32_t ret = 0;
   char addr[BT_ADDR_LE_STR_LEN] = { 0 };
   struct uart_data_t *tx = NULL;

   bt_addr_le_to_str(bt_conn_get_dst(conn), addr, ARRAY_SIZE(addr));

   LOG_INF("Received data from: %s", addr);

   for (uint16_t pos = 0; pos != len;)
   {
      tx = k_malloc(sizeof(*tx));

      if (!tx)
      {
         LOG_WRN("Not able to allocate UART send data buffer");
         return;
      }

      // Keep the last byte of TX buffer for potential LF char
      size_t tx_data_size = sizeof(tx->data) - 1;

      if ((len - pos) > tx_data_size) {
         tx->len = tx_data_size;
      }
      else {
         tx->len = (len - pos);
      }

      memcpy(tx->data, &data[pos], tx->len);

      pos += tx->len;

      // Append the LF character when the CR character triggered transmission from the peer
      if ((pos == len) && (data[len - 1] == '\r'))
      {
         tx->data[tx->len] = '\n';
         tx->len++;
      }
   }

   // Execute shell command with RX string; send back error msg if invalid command
   if (tx == NULL) {
      return;
   }
   else
   {
      tx->data[tx->len] = '\0';
      LOG_INF("> %s", tx->data);
      ret = shell_execute_cmd(shell_backend_uart_get_ptr(),  tx->data);
      if (ret != 0) {
         bt_nus_send(NULL, "Invalid cmd\n\r", 14);
      }
      k_free(tx);
   }
}

static struct bt_nus_cb nus_cb = {
   .received = ble_uart_receive_cb,
};

int32_t ble_uart_init(void)
{
   int err = 0;

   err = bt_nus_init(&nus_cb);
   if (err)
   {
      LOG_ERR("Failed to initialize UART service (err: %d)", err);
      return err;
   }

   return 0;
}

void ble_write_thread(void)
{
   for (;;)
   {
      // Wait indefinitely for data to be sent over bluetooth
      struct uart_data_t *buf = uart_lib_get();

      if (bt_nus_send(NULL, buf->data, buf->len)) {
         LOG_WRN("Failed to send data over BLE connection");
      }

      k_free(buf);
   }
}

K_THREAD_DEFINE(ble_write_thread_id, STACKSIZE, ble_write_thread, NULL, NULL,
      NULL, PRIORITY, 0, 0);
