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
#include "inttypes.h"

#include "app.h"

/*! \brief convert little endian uint16_t to array of bytes. */
#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))
 /*! \brief convert little endian uint16_t to byte 0. */
#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
/*! \brief convert little endian uint16_t to byte 1. */
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))

/* Print boot message */
static void bootMessage(struct gecko_msg_system_boot_evt_t *bootevt);

/* Flag for indicating DFU Reset must be performed */
static uint8_t boot_to_dfu = 0;

// Advertisement handles
enum
{
  HANDLE_DEMO, HANDLE_IBEACON, HANDLE_EDDYSTONE, HANDLE_EXTENDED
};

// Struct used for periodic advertising data
typedef struct
{
  uint8 length; // Length of data in bytes after this
  uint8 type; // Type of adv data
  uint8 manuf_LO; // Company identifier LO. This value may be used in the internal and interoperability tests before a Company ID has been assigned.
  uint8 manuf_HI; //Company identifier HI. This value shall not be used in shipping end products.

  uint8 counter_LO; // Placeholder for lower byte of a 16-bit counter
  uint8 counter_HI; // Placeholder for higher byte of a 16-bit counter
} periodicData;

#define IBEACON 1

#define ADV_PKT                       (0)

#if IBEACON

void bcnSetupAdvBeaconing(void)
{

  /* This function sets up a custom advertisement package according to iBeacon specifications.
   * The advertisement package is 30 bytes long. See the iBeacon specification for further details.
   */

  static struct
  {
    uint8_t flagsLen; /* Length of the Flags field. */
    uint8_t flagsType; /* Type of the Flags field. */
    uint8_t flags; /* Flags field. */
    uint8_t mandataLen; /* Length of the Manufacturer Data field. */
    uint8_t mandataType; /* Type of the Manufacturer Data field. */
    uint8_t compId[2]; /* Company ID field. */
    uint8_t beacType[2]; /* Beacon Type field. */
    uint8_t uuid[16]; /* 128-bit Universally Unique Identifier (UUID). The UUID is an identifier for the company using the beacon*/
    uint8_t majNum[2]; /* Beacon major number. Used to group related beacons. */
    uint8_t minNum[2]; /* Beacon minor number. Used to specify individual beacons within a group.*/
    uint8_t txPower; /* The Beacon's measured RSSI at 1 meter distance in dBm. See the iBeacon specification for measurement guidelines. */
  }
  bcnBeaconAdvData
  = {
    /* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
    2, /* length  */
    0x01, /* type */
    0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */

    /* Manufacturer specific data */
    26, /* length of field*/
    0xFF, /* type of field */

    /* The first two data octets shall contain a company identifier code from
     * the Assigned Numbers - Company Identifiers document */
    /* 0x004C = Apple */
    { UINT16_TO_BYTES(0x004C)},

    /* Beacon type */
    /* 0x0215 is iBeacon */
    { UINT16_TO_BYTE1(0x0215), UINT16_TO_BYTE0(0x0215)},

    /* 128 bit / 16 byte UUID */
    { 0xE2,0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2,
      0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0},

    /* Beacon major number */
    /* Set to 34987 and converted to correct format */
    { UINT16_TO_BYTE1(34987), UINT16_TO_BYTE0(34987)},

    /* Beacon minor number */
    /* Set as 1025 and converted to correct format */
    { UINT16_TO_BYTE1(1025), UINT16_TO_BYTE0(1025)},

    /* The Beacon's measured RSSI at 1 meter distance in dBm */
    /* 0xC3 is -61dBm */
    0xC3
  };

  //
  uint8_t len = sizeof(bcnBeaconAdvData);
  uint8_t *pData = (uint8_t*)(&bcnBeaconAdvData);

  /* Set 0 dBm Transmit Power */
  gecko_cmd_le_gap_set_advertise_tx_power(HANDLE_IBEACON,0);

  /* Set custom advertising data */
  gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_IBEACON, ADV_PKT,len, pData);

  /* Set advertising parameters. 100ms advertisement interval. All channels used.
   * The first two parameters are minimum and maximum advertising interval, both in
   * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */

  //no timeout on time nor # events
  gecko_cmd_le_gap_set_advertise_timing(HANDLE_IBEACON,160,160,0,0);

  gecko_cmd_le_gap_set_advertise_tx_power(HANDLE_IBEACON,0);
  /* Start advertising in user mode and enable connections 2000 events before expiring ... just a random number*/
  gecko_cmd_le_gap_start_advertising(HANDLE_IBEACON,le_gap_user_data, le_gap_non_connectable);

}

#endif // end of IBEACON

/* LAB TASK MULTIPLE ADVT: modify your custom URL */
static uint8_t eddystone_data[] = {
    0x03,          //Length of service list
    0x03,          //service list
    0xAA, 0xFE,    //Eddystone ID
    0x12,          // length of service data + length of URL
    0x16,          //service data
    0xAA, 0xFE,   //Eddystone ID
    0x10,          //frame type Eddyston-URL
    0x00,          // tx power
    0x00,          //http://www.
    'b', 'l', 'u', 'e', 'c', 'a', 't', 'e', '.', 'c', 'o', 'm'

};

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

/* Main application */
void appMain(gecko_configuration_t *pconfig)
{
#if DISABLE_SLEEP > 0
  pconfig->sleep.flags = 0;
#endif

  /* LAB TASK MULTIPLE ADVT: max_advertisers */
  pconfig->bluetooth.max_advertisers = 4;

  /* Initialize debug prints. Note: debug prints are off by default. See DEBUG_LEVEL in app.h */
  initLog();

  /* Initialize stack */
  gecko_init(pconfig);

  /* LAB TASK PERIODIC ADVT: initialize periodic advertisement */
  gecko_init_periodic_advertising();

  /* LAB TASK PERIODIC ADVT: initialize periodic advertising data */
  static uint16 counter = 0;
  static periodicData myCounter = {
      .length = (uint8)5,
      .type = (uint8)0xFF,
      .manuf_HI = (uint8)0xFF,
      .manuf_LO = (uint8)0xFF,
      .counter_LO = (uint8)0,
      .counter_HI = (uint8)0 };
  periodicData *pData = &myCounter;


  while (1) {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;

    /* if there are no events pending then the next call to gecko_wait_event() may cause
     * device go to deep sleep. Make sure that debug prints are flushed before going to sleep */
    if (!gecko_event_pending()) {
      flushLog();
    }

    /* Check for stack event. This is a blocking event listener. If you want non-blocking please see UG136. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header)) {

      case gecko_evt_system_boot_id:
        bootMessage(&(evt->data.evt_system_boot));

        /* LAB TASK MULTIPLE ADVT: iBeacons */
        bcnSetupAdvBeaconing(); //iBeacons
        printLog("boot event - starting advertising\r\n");
        gecko_cmd_le_gap_set_advertise_timing(HANDLE_DEMO, 160, 160, 0, 0);

        /* LAB TASK 2M PHY connection: enable scan request */
        gecko_cmd_le_gap_set_advertise_report_scan_request(HANDLE_DEMO,true); //Stop continuous output

        gecko_cmd_le_gap_start_advertising(HANDLE_DEMO, le_gap_general_discoverable, le_gap_connectable_scannable);

        /* LAB TASK MULTIPLE ADVT: EddyStone */
        gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_EDDYSTONE, ADV_PKT, 30, eddystone_data);
		gecko_cmd_le_gap_set_advertise_configuration(HANDLE_EDDYSTONE,0x01);
		gecko_cmd_le_gap_set_advertise_timing(HANDLE_EDDYSTONE,160,160,0,0);
		gecko_cmd_le_gap_set_advertise_channel_map(HANDLE_EDDYSTONE,0x07);
		gecko_cmd_le_gap_start_advertising(HANDLE_EDDYSTONE,le_gap_user_data, le_gap_non_connectable);

        /* LAB TASK EXTENDED ADVT: 4th advertisement */
		printLog("LAB TASK EXTENDED ADVT \n");
		gecko_cmd_le_gap_set_advertise_timing(HANDLE_EXTENDED,160,160,0,0);

		gecko_cmd_le_gap_set_advertise_configuration(HANDLE_EXTENDED,0x02); //Anonymous Advertising
//		uint16_t start_advertising = (unsigned int)gecko_cmd_le_gap_start_advertising(0,le_gap_general_discoverable, le_gap_undirected_connectable)->result;
		gecko_cmd_le_gap_start_advertising(0,le_gap_general_discoverable, le_gap_connectable_non_scannable);

//		gecko_cmd_le_gap_set_advertise_phy(HANDLE_EXTENDED,le_gap_phy_coded, le_gap_phy_2m);
		gecko_cmd_le_gap_set_advertise_phy(HANDLE_EXTENDED,le_gap_phy_coded, le_gap_phy_coded); //125k coded PHY

//		gecko_cmd_le_gap_clear_advertise_configuration(HANDLE_EXTENDED,0x01);
		gecko_cmd_le_gap_set_advertise_configuration(HANDLE_EXTENDED,0x08);
		gecko_cmd_le_gap_set_advertise_tx_power(HANDLE_EXTENDED,100);

        break;


      /* LAB TASK PERIODIC ADVT
       * Use this case when testing periodic advertisements
       * */
//      case gecko_evt_system_boot_id:
//          gecko_cmd_system_set_tx_power(100);
//          gecko_cmd_le_gap_set_advertise_tx_power(HANDLE_DEMO,30);
//
//          gecko_cmd_le_gap_set_advertise_timing(HANDLE_DEMO, 160, 160, 0, 0);
//          gecko_cmd_le_gap_clear_advertise_configuration(HANDLE_DEMO,1);
//          gecko_cmd_le_gap_start_advertising(HANDLE_DEMO,le_gap_general_discoverable, le_gap_non_connectable);
//          gecko_cmd_le_gap_start_periodic_advertising(HANDLE_DEMO,160,160,1);
//
//          /* LAB TASK PERIODIC ADVT: set advertising data and soft timer */
//          gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_DEMO, 8, sizeof(periodicData), (const uint8 *)pData);
//          printLog("\r\n Counter: %d \r\n", counter);
//          gecko_cmd_hardware_set_soft_timer(32768, 0, 0);
//
//      break;


      /* LAB TASK PERIODIC ADVT */
      case gecko_evt_hardware_soft_timer_id:
          counter++;
          pData->counter_HI = (uint8) (counter >> 8);
          pData->counter_LO = (uint8) (counter & 0xFF);

          //Reset the advertising data pointers to reflect the change in data
          gecko_cmd_le_gap_bt5_set_adv_data(0, 8, sizeof(periodicData), (const uint8 *)pData);
          printLog("\r\n Counter: %d \r\n", counter);
      break;

      case gecko_evt_le_connection_opened_id:

        printLog("connection opened\r\n");

        break;

      case gecko_evt_le_connection_closed_id:

        printLog("connection closed, reason: 0x%2.2x\r\n", evt->data.evt_le_connection_closed.reason);

        /* Check if need to boot to OTA DFU mode */
        if (boot_to_dfu) {
          /* Enter to OTA DFU mode */
          gecko_cmd_system_reset(2);
        }
        else {
          /* Restart advertising after client has disconnected */
          gecko_cmd_le_gap_start_advertising(HANDLE_DEMO, le_gap_general_discoverable, le_gap_connectable_scannable);
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

        /* LAB TASK 2M PHY connection: phy status handler */
	case gecko_evt_le_connection_phy_status_id:
		printLog("now using the %s PHY\r\n", getPhyName(evt->data.evt_le_connection_phy_status.phy).string);
		break;

        /*LAB TASK 2M PHY connection: scan request reporting */
	case gecko_evt_le_gap_scan_request_id:
	    printLog("received scan request from:   %02x:%02x:%02x:%02x:%02x:%02x\r\n",
	    evt->data.evt_le_gap_scan_request.address.addr[5],
	    evt->data.evt_le_gap_scan_request.address.addr[4],
	    evt->data.evt_le_gap_scan_request.address.addr[3],
	    evt->data.evt_le_gap_scan_request.address.addr[2],
	    evt->data.evt_le_gap_scan_request.address.addr[1],
	    evt->data.evt_le_gap_scan_request.address.addr[0]);
	    break;

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
