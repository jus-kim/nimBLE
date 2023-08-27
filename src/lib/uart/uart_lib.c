/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       uart_lib.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 UART.
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <stdio.h>

#include <lib/uart/uart_lib.h>
#include <lib/uart/uart_async_adapter.h>

LOG_MODULE_REGISTER(LOG_UART_LIB);


#define UART_BUF_SIZE CONFIG_BT_NUS_UART_BUFFER_SIZE
#define UART_WAIT_FOR_BUF_DELAY K_MSEC(50)
#define UART_WAIT_FOR_RX CONFIG_BT_NUS_UART_RX_WAIT_TIME


static const struct device *dev_uart = DEVICE_DT_GET(DT_CHOSEN(nordic_uart0));
static struct k_work_delayable uart_work;

static K_FIFO_DEFINE(fifo_uart_tx_data);
static K_FIFO_DEFINE(fifo_uart_rx_data);

#if CONFIG_BT_NUS_UART_ASYNC_ADAPTER
UART_ASYNC_ADAPTER_INST_DEFINE(async_adapter);
#else
static const struct device *const async_adapter;
#endif


int32_t uart_lib_put(struct uart_data_t *tx)
{
   int32_t err;

   err = uart_tx(dev_uart, tx->data, tx->len, SYS_FOREVER_MS);
   if (err != 0) {
      k_fifo_put(&fifo_uart_tx_data, tx);
   }
   else {
      LOG_WRN("Nothing to send via UART");
   }

   return 0;
}

struct uart_data_t *uart_lib_get(void)
{
   return k_fifo_get(&fifo_uart_rx_data, K_FOREVER);
}

static void uart_lib_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
   ARG_UNUSED(dev);

   static size_t aborted_len;
   struct uart_data_t *buf;
   static uint8_t *aborted_buf;
   static bool disable_req;

   switch (evt->type) {
   case UART_TX_DONE:
      LOG_DBG("UART_TX_DONE");
      if ((evt->data.tx.len == 0) || (!evt->data.tx.buf)) {
         return;
      }

      if (aborted_buf)
        {
         buf = CONTAINER_OF(aborted_buf, struct uart_data_t,
                  data);
         aborted_buf = NULL;
         aborted_len = 0;
      }
      else
      {
         buf = CONTAINER_OF(evt->data.tx.buf, struct uart_data_t, data);
      }
      k_free(buf);

      buf = k_fifo_get(&fifo_uart_tx_data, K_NO_WAIT);
      if (!buf) {
         return;
      }
      if (uart_tx(dev_uart, buf->data, buf->len, SYS_FOREVER_MS)) {
         LOG_WRN("Failed to send data over UART");
      }

      break;

   case UART_RX_RDY:
      LOG_DBG("UART_RX_RDY");
      buf = CONTAINER_OF(evt->data.rx.buf, struct uart_data_t, data);
      buf->len += evt->data.rx.len;

      if (disable_req) {
         return;
      }

      if ((evt->data.rx.buf[buf->len - 1] == '\n') ||
          (evt->data.rx.buf[buf->len - 1] == '\r')) {
         disable_req = true;
         uart_rx_disable(dev_uart);
      }

      break;

   case UART_RX_DISABLED:
      LOG_DBG("UART_RX_DISABLED");
      disable_req = false;

      buf = k_malloc(sizeof(*buf));
      if (buf) {
         buf->len = 0;
      } else {
         LOG_WRN("Not able to allocate UART receive buffer");
         k_work_reschedule(&uart_work, UART_WAIT_FOR_BUF_DELAY);
         return;
      }

      uart_rx_enable(dev_uart, buf->data, sizeof(buf->data),
                UART_WAIT_FOR_RX);

      break;

   case UART_RX_BUF_REQUEST:
      LOG_DBG("UART_RX_BUF_REQUEST");
      buf = k_malloc(sizeof(*buf));
      if (buf) {
         buf->len = 0;
         uart_rx_buf_rsp(dev_uart, buf->data, sizeof(buf->data));
      } else {
         LOG_WRN("Not able to allocate UART receive buffer");
      }

      break;

   case UART_RX_BUF_RELEASED:
      LOG_DBG("UART_RX_BUF_RELEASED");
      buf = CONTAINER_OF(evt->data.rx_buf.buf, struct uart_data_t,
               data);

      if (buf->len > 0) {
         k_fifo_put(&fifo_uart_rx_data, buf);
      } else {
         k_free(buf);
      }

      break;

   case UART_TX_ABORTED:
      LOG_DBG("UART_TX_ABORTED");
      if (!aborted_buf) {
         aborted_buf = (uint8_t *)evt->data.tx.buf;
      }

      aborted_len += evt->data.tx.len;
      buf = CONTAINER_OF(aborted_buf, struct uart_data_t,
               data);

      uart_tx(dev_uart, &buf->data[aborted_len],
         buf->len - aborted_len, SYS_FOREVER_MS);

      break;

   default:
      break;
   }
}

static void uart_lib_work_handler(struct k_work *item)
{
   struct uart_data_t *buf;

   buf = k_malloc(sizeof(*buf));
   if (buf) {
      buf->len = 0;
   } else {
      LOG_WRN("Not able to allocate UART receive buffer");
      k_work_reschedule(&uart_work, UART_WAIT_FOR_BUF_DELAY);
      return;
   }

   uart_rx_enable(dev_uart, buf->data, sizeof(buf->data), UART_WAIT_FOR_RX);
}

static bool uart_lib_test_async_api(const struct device *dev)
{
   const struct uart_driver_api *api = (const struct uart_driver_api *)dev->api;

   return (api->callback_set != NULL);
}

int32_t uart_lib_init(void)
{
   int32_t err, pos;
   struct uart_data_t *tx;
   struct uart_data_t *rx;

   if (!device_is_ready(dev_uart)) {
      return -ENODEV;
   }

   rx = k_malloc(sizeof(*rx));
   if (rx) {
      rx->len = 0;
   }
   else {
      return -ENOMEM;
   }

   k_work_init_delayable(&uart_work, uart_lib_work_handler);


   if (IS_ENABLED(CONFIG_BT_NUS_UART_ASYNC_ADAPTER) && !uart_lib_test_async_api(dev_uart))
   {
      uart_async_adapter_init(async_adapter, dev_uart);
      dev_uart = async_adapter;
   }

   err = uart_callback_set(dev_uart, uart_lib_cb, NULL);
   if (err)
   {
      k_free(rx);
      LOG_ERR("Cannot initialize UART callback");
      return err;
   }

   if (IS_ENABLED(CONFIG_UART_LINE_CTRL))
   {
      LOG_INF("Wait for DTR");
      while (true)
      {
         uint32_t dtr = 0;

         uart_line_ctrl_get(dev_uart, UART_LINE_CTRL_DTR, &dtr);
         if (dtr) {
            break;
         }
         // Give CPU resources to low priority threads
         k_sleep(K_MSEC(100));
      }
      LOG_INF("DTR set");
      err = uart_line_ctrl_set(dev_uart, UART_LINE_CTRL_DCD, 1);
      if (err) {
         LOG_WRN("Failed to set DCD, ret code %d", err);
      }
      err = uart_line_ctrl_set(dev_uart, UART_LINE_CTRL_DSR, 1);
      if (err) {
         LOG_WRN("Failed to set DSR, ret code %d", err);
      }
   }

   tx = k_malloc(sizeof(*tx));

   if (tx)
   {
      pos = snprintf(tx->data, sizeof(tx->data),
                "Starting Nordic UART service example\r\n");

      if ((pos < 0) || (pos >= sizeof(tx->data))) {
         k_free(rx);
         k_free(tx);
         LOG_ERR("snprintf returned %d", pos);
         return -ENOMEM;
      }

      tx->len = pos;
   }
   else
   {
      k_free(rx);
      return -ENOMEM;
   }

   err = uart_tx(dev_uart, tx->data, tx->len, SYS_FOREVER_MS);
   if (err)
   {
      k_free(rx);
      k_free(tx);
      LOG_ERR("Cannot display welcome message (err: %d)", err);
      return err;
   }

   err = uart_rx_enable(dev_uart, rx->data, sizeof(rx->data), 50);
   if (err)
   {
      LOG_ERR("Cannot enable dev_uart reception (err: %d)", err);
      // Free the rx buffer only because the tx buffer will be handled in the callback
      k_free(rx);
   }

   return 0;
}
