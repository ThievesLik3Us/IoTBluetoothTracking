/***************************************************************************//**
 * @file app.c
 * @brief Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

#include "app.h"

/* Print boot message */
static void bootMessage(struct gecko_msg_system_boot_evt_t *bootevt);

/* Flag for indicating DFU Reset must be performed */
static uint8_t boot_to_dfu = 0;

/* LAB TASK 2MPHY CONNECTION:  service UUID */

/* LAB TASK PERIODIC ADVT: periodic advertising service UUID*/
static const uint8 periodic_sync_uuid[16] = { };

// Parse advertisements looking for advertised periodicSync Service.
static bool serviceMatch(uint8_t *data, uint8_t len)
{
  uint8_t adFieldLength;
  uint8_t adFieldType;
  uint8_t i = 0, j;
  printLog("packet length %d\r\n", len);
  // Parse advertisement packet
  while (i < len) {
    adFieldLength = data[i];
    adFieldType = data[i + 1];
    printLog("adField type %d \r\n", adFieldType);
    if (adFieldType == 0x06 || adFieldType == 0x07) {

      // Look through all the UUID looking for periodic sync
      j = i + 2; // j pointing to where the data starts (just after adFieldType)
      do {
        if (memcmp(&data[j], periodic_sync_uuid, 16) == 0) {
          return true;
        }
        j = j + 16;
      }
      while (j < i + adFieldLength);
    }
    // advance to the next AD struct
    i = i + adFieldLength + 1;
  }
  return false;
}

struct phyName
{
  uint8 flag;
  uint8 string[5];
};

struct phyName getPhyName(uint8 flag)
{

  static const struct phyName PHYs[3] = { {1, "1M"}, {2, "2M"}, {4, "125k"}};
  for (int i = 0; i < 3; i++) {
    if (PHYs[i].flag == flag) {
      return PHYs[i];
    }
  }

  return PHYs[0]; // If not found, return index 0 i.e. 1M
}

static bool syncOpened = false;


bool check_uuid(uint8 *uuid, uint8 len)
{

  bool match = false;
  int i;

  /* uuid is in reverse order in advertising packet*/
  for (i = 0; i < len; i++) {
    if (silabs_2mphy_uuid[len - i - 1] != *(uuid + i)) {
      return match;
    }
  }
  match = true;
  return match;
}

struct bd_addr_as_str
{
  char addressStr[18];
};

struct bd_addr_as_str bdaddrToStr(bd_addr address)
{
  struct bd_addr_as_str bdaddrStr;

  sprintf(bdaddrStr.addressStr, "%02x:%02x:%02x:%02x:%02x:%02x", address.addr[5], address.addr[4], address.addr[3], address.addr[2], address.addr[1], address.addr[0]);
  return bdaddrStr;
}

void ButtonHandler(uint8_t pin)
{
  //int level;
  //static int rise_count = 0, fall_count=0;

  if (pin == BSP_BUTTON0_PIN) {
    gecko_external_signal(1);
  }
  else if (pin == BSP_BUTTON1_PIN) {
    gecko_external_signal(2);
  }
}

void setup_ext_interrupts(void)
{
  //SYSTEM_ChipRevision_TypeDef *revision;
  /* config Button0 as input*/
  GPIO_PinModeSet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN, gpioModeInput, 0);

  /* set up button 0 to generate an interrupt when it is either pressed
   or released */

  // SYSTEM_ChipRevisionGet(revision);
  /*printf("Chip revision major %d minor %d\r\n",
   revision->major,
   revision->minor); */

  GPIO_IntConfig(BSP_BUTTON0_PORT,
  BSP_BUTTON0_PIN,
  false, //rising edge trigger
      true, //falling edge trigger
      true);
  GPIO_IntConfig(BSP_BUTTON1_PORT,
  BSP_BUTTON1_PIN,
  false, //rising edge trigger
      true, //falling edge trigger
      true);

  GPIOINT_Init();
  GPIOINT_CallbackRegister(BSP_BUTTON0_PIN, ButtonHandler);
  GPIO_IntEnable(1 << BSP_BUTTON0_PIN);
  GPIOINT_CallbackRegister(BSP_BUTTON1_PIN, ButtonHandler);
  GPIO_IntEnable(1 << BSP_BUTTON1_PIN);
}

const uint8 supported_phys[4] = {1, 2, 4};

/* Main application */
void appMain(gecko_configuration_t *pconfig)
{
#if DISABLE_SLEEP > 0
  pconfig->sleep.flags = 0;
#endif

  /* LAB TASK PERIODIC ADVT: Set the maximum number of periodic sync allowed */

  /* Initialize debug prints. Note: debug prints are off by default. See DEBUG_LEVEL in app.h */
  initLog();

  setup_ext_interrupts();

  /* Initialize stack */
  gecko_init(pconfig);

  /* LAB TASK PERIODIC ADVT: initialize periodic sync feature */


  uint8 connection;

  /* variables used for periodic advertising */
  struct gecko_msg_sync_open_rsp_t* syncOpenResponse;
  static uint8 sync_handle;
  static uint16 counter;

  uint8 desired_phy = le_gap_phy_1m;
  enum le_gap_phy_type scanning_phy = le_gap_phy_1m;
  uint8 phy_index = 0;

  while (1) {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;
    uint16 result;

    if (!gecko_event_pending()) {
      flushLog();
    }

    /* Check for stack event. This is a blocking event listener. If you want non-blocking please see UG136. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header)) {

      case gecko_evt_system_boot_id:

        /* LAB TASK 2M PHY CONNECTION: start scanning */
        break;

      case gecko_evt_le_connection_opened_id:
        printLog("connection opened\r\n");
        connection = evt->data.evt_le_connection_opened.connection;
        break;

      case gecko_evt_le_connection_phy_status_id:
        printLog("now using the %s PHY\r\n", getPhyName(evt->data.evt_le_connection_phy_status.phy).string);
        break;

      case gecko_evt_le_connection_parameters_id:
        break;

      case gecko_evt_gatt_mtu_exchanged_id:
        break;

      case gecko_evt_le_connection_closed_id:

        printLog("connection closed, reason: 0x%2.2x\r\n", evt->data.evt_le_connection_closed.reason);
        gecko_cmd_le_gap_start_discovery(scanning_phy, le_gap_discover_generic);
        /* Check if need to boot to OTA DFU mode */
        if (boot_to_dfu) {
          /* Enter to OTA DFU mode */
          gecko_cmd_system_reset(2);
        }
        else {
          /* Restart advertising after client has disconnected */
          /* Restart advertising after client has disconnected */
          // gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable); L,L
        }
        break;

        /* Events related to OTA upgrading
         ----------------------------------------------------------------------------- */

        /* Check if the user-type OTA Control Characteristic was written.
         * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
      case gecko_evt_gatt_server_user_write_request_id:

        if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
          /* Set flag to enter to OTA mode */
          boot_to_dfu = 1;
          /* Send response to Write Request */
          gecko_cmd_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control, bg_err_success);

          /* Close connection to enter to DFU OTA mode */
          gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
        }
        break;

        /* Add additional event handlers as your application requires */

        /* LAB TASK: 2M PHY CONNECTION: scan response event handler */

        /* LAB TASK: 2M PHY CONNECTION: external signal handling event*/

      default:
        break;
    }
  }
}

/* Print stack version and local Bluetooth address as boot message */
static void bootMessage(struct gecko_msg_system_boot_evt_t *bootevt)
{
#if DEBUG_LEVEL
  bd_addr local_addr;
  int i;

  printLog("stack version: %u.%u.%u\r\n", bootevt->major, bootevt->minor, bootevt->patch);
  local_addr = gecko_cmd_system_get_bt_address()->address;

  printLog("local BT device address: ");
  for (i = 0; i < 5; i++) {
    printLog("%2.2x:", local_addr.addr[5 - i]);
  }
  printLog("%2.2x\r\n", local_addr.addr[0]);
#endif
}
