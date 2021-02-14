/***************************************************************************//**
 * @file
 * @brief AoA locator application with GUI.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "system.h"
#include "sl_bt_api.h"
#include "sl_bt_ncp_host.h"
#include "app_log.h"
#include "app_assert.h"
#include "uart.h"
#include "app.h"
#include "tcp.h"
#include "aoa_util.h"

#include "conn.h"
#include "aoa.h"
#include "gui.h"

#define USAGE "\nUsage: %s -t <wstk_address> | -u <serial_port> [-b <baud_rate>] [-f <flow control: 1(on, default) or 0(off)>]\n"
#define DEFAULT_UART_PORT             NULL
#define DEFAULT_UART_BAUD_RATE        115200
#define DEFAULT_UART_FLOW_CONTROL     1
#define DEFAULT_UART_TIMEOUT          100
#define DEFAULT_TCP_PORT              "4901"
#define MAX_OPT_LEN                   255
#define INVALID_BT_ADDRESS            { { 0, 0, 0, 0, 0, 0 } }

SL_BT_API_DEFINE();

static int serial_port_init(char* uartPort, uint32_t uartBaudRate, uint32_t uartFlowControl, int32_t timeout);
static void uart_tx_wrapper(uint32_t len, uint8_t *data);
static void tcp_tx_wrapper(uint32_t len, uint8_t *data);

// Verbose output
uint32_t verbose_level;

static char uart_target_port[MAX_OPT_LEN]; // Serail port name of the NCP target
static char tcp_target_address[MAX_OPT_LEN]; // IP address or host name of the NCP target using TCP connection

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(int argc, char *argv[])
{
  int opt;
  uint32_t target_baud_rate = DEFAULT_UART_BAUD_RATE;
  uint32_t target_flow_control = DEFAULT_UART_FLOW_CONTROL;

  uart_target_port[0] = '\0';
  tcp_target_address[0] = '\0';

  verbose_level = 0;

  //Parse command line arguments
  while ((opt = getopt(argc, argv, "t:u:b:f:i:h")) != -1) {
    switch (opt) {
      case 'u': //Target port or address.
        strncpy(uart_target_port, optarg, MAX_OPT_LEN);
        break;
      case 't': //Target TCP address
        strncpy(tcp_target_address, optarg, MAX_OPT_LEN);
        break;
      case 'f': //Target flow control
        target_flow_control = atol(optarg);
        break;
      case 'b': //Target baud rate
        target_baud_rate = atol(optarg);
        break;
      case 'h': //Help!
        app_log(USAGE, argv[0]);
        exit(EXIT_SUCCESS);
      default: /* '?' */
        app_log(USAGE, argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if (uart_target_port[0] != '\0') {
    // Initialise serial communication as non-blocking.
    SL_BT_API_INITIALIZE_NONBLOCK(uart_tx_wrapper, uartRx, uartRxPeek);
    if (serial_port_init(uart_target_port, target_baud_rate, target_flow_control, DEFAULT_UART_TIMEOUT) < 0) {
      app_log("Non-blocking serial port init failure\n");
      exit(EXIT_FAILURE);
    }
  } else if (tcp_target_address[0] != '\0') {
    // Initialise socket communication
    SL_BT_API_INITIALIZE_NONBLOCK(tcp_tx_wrapper, tcp_rx, tcp_rx_peek);
    if (tcp_open(tcp_target_address, DEFAULT_TCP_PORT) < 0) {
      app_log("Non-blocking TCP connection init failure\n");
      exit(EXIT_FAILURE);
    }
  } else {
    app_log("Either uart port or TCP address shall be given.\n");
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  aoa_whitelist_init();

  app_log("AoA NCP-host initialised\n");
  app_log("Resetting NCP...\n");
  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(0);

  // AoA specific init
  aoa_init_buffers();

  init_connection();
  gui_init(argc, argv);
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;

  // Catch boot event...
  if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_system_boot_id) {
    // Print boot message.
    app_log("Bluetooth stack booted: v%d.%d.%d-b%d\n",
            evt->data.evt_system_boot.major,
            evt->data.evt_system_boot.minor,
            evt->data.evt_system_boot.patch,
            evt->data.evt_system_boot.build);
    // Extract unique ID from BT Address.
    sc = sl_bt_system_get_identity_address(&address, &address_type);
    app_assert(sc == SL_STATUS_OK,
               "[E: 0x%04x] Failed to get Bluetooth address\n",
               (int)sc);
    app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
            address_type ? "static random" : "public device",
            address.addr[5],
            address.addr[4],
            address.addr[3],
            address.addr[2],
            address.addr[1],
            address.addr[0]);
  }
  // ...then call the connection specific event handler.
  app_bt_on_event(evt);
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
  gui_main();
}

/**************************************************************************//**
 * UART TX Wrapper.
 *****************************************************************************/
static void uart_tx_wrapper(uint32_t len, uint8_t *data)
{
  if (0 > uartTx(len, data)) {
    app_log("Failed to write to serial port\n");
    exit(EXIT_FAILURE);
  }
}

/**************************************************************************//**
 * TCP TX Wrapper.
 *****************************************************************************/
static void tcp_tx_wrapper(uint32_t len, uint8_t *data)
{
  if (0 > tcp_tx(len, data)) {
    app_log("Failed to write to TCP port\n");
    tcp_close();
    exit(EXIT_FAILURE);
  }
}

/**************************************************************************//**
 * Initialise serial port.
 *****************************************************************************/
static int serial_port_init(char* uartPort, uint32_t uartBaudRate, uint32_t uartFlowControl, int32_t timeout)
{
  int ret;

  // Sanity check of arguments.
  if (!uartPort || !uartBaudRate || (uartFlowControl > 1)) {
    app_log("Serial port setting error.\n");
    ret = -1;
  } else {
    ret = uartOpen((int8_t*)uartPort, uartBaudRate, uartFlowControl, timeout);
  }

  // Initialise the serial port with RTS/CTS enabled.
  return ret;
}

uint8_t find_service_in_advertisement(uint8_t *advdata, uint8_t advlen, uint8_t *service_uuid)
{
  uint8_t ad_field_length;
  uint8_t ad_field_type;
  uint8_t *ad_uuid_field;
  uint32_t i = 0;
  uint32_t next_ad_structure;
  uint8_t ret = 0;

  // Parse advertisement packet
  while (i < advlen) {
    ad_field_length = advdata[i];
    ad_field_type = advdata[i + 1];
    next_ad_structure = i + ad_field_length + 1;
    // incomplete or complete UUIDs
    if (ad_field_type == AD_FIELD_I || ad_field_type == AD_FILED_C) {
      // compare UUID to the service UUID to be found
      for (ad_uuid_field = advdata + i + 2; ad_uuid_field < advdata + next_ad_structure; ad_uuid_field += SERVICE_UUID_LEN) {
        if (memcmp(ad_uuid_field, service_uuid, SERVICE_UUID_LEN) == 0) {
          ret = 1;
          break;
        }
      }
      if (ret == 1) {
        break;
      }
    }
    // advance to the next AD struct
    i = next_ad_structure;
  }
  return ret;
}

void app_deinit(void)
{
  app_log("Shutting down.\n");
  if (uart_target_port[0] != '\0') {
    uartClose();
  } else if (tcp_target_address[0] != '\0') {
    tcp_close();
  }
}

void app_on_iq_report(conn_properties_t *tag, aoa_iq_report_t *iq_report)
{
  static const bd_addr invalid_tag_address = INVALID_BT_ADDRESS;
  static bd_addr displayed_tag_address = INVALID_BT_ADDRESS;
  aoa_angle_t angle;

  // Check if there is a tag to display.
  if (0 == memcmp(displayed_tag_address.addr, invalid_tag_address.addr, sizeof(displayed_tag_address.addr))) {
    // Select the first tag to display.
    displayed_tag_address = tag->address;
  }

  // Filter for one specific tag.
  if (0 == memcmp(displayed_tag_address.addr, tag->address.addr, sizeof(displayed_tag_address.addr))) {
    if (aoa_calculate(&tag->aoa_states, iq_report, &angle) != SL_STATUS_OK) {
      return;
    }

    // Flip azimuth to match the GUI with the radio board.
    gui_update(-angle.azimuth, angle.rssi, angle.elevation);
  }
}
