/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include "em_common.h"
#include "sl_app_assert.h"
#include "sl_bluetooth.h"
#include "sl_app_log.h"
#include "gatt_db.h"
#include "app.h"


// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * KASS app-server.c: Initial Code
 *****************************************************************************/

/* Print boot message */
static void bootMessage(struct sl_bt_evt_system_boot_s *bootevt);

/* Flag for indicating DFU Reset must be performed */
static uint8_t boot_to_dfu = 0;

static uint8_t adv_h = 255; //adv. handle

static uint16_t power;

// Advertisement handles
enum
{
  HANDLE_DEMO, HANDLE_IBEACON, HANDLE_EDDYSTONE, HANDLE_EXTENDED
};

// Struct used for periodic advertising data
typedef struct
{
  uint8_t length; // Length of data in bytes after this
  uint8_t type; // Type of adv data
  uint8_t manuf_LO; // Company identifier LO. This value may be used in the internal and interoperability tests before a Company ID has been assigned.
  uint8_t manuf_HI; //Company identifier HI. This value shall not be used in shipping end products.

  uint8_t counter_LO; // Placeholder for lower byte of a 16-bit counter
  uint8_t counter_HI; // Placeholder for higher byte of a 16-bit counter
} periodicData;

#ifndef MAX_ADVERTISERS
#define MAX_ADVERTISERS 3
#endif

/*! \brief convert little endian uint16_t to byte 0. */
#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
/*! \brief convert little endian uint16_t to byte 1. */
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
/*! \brief convert little endian uint16_t to array of bytes. */
#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))

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
  sl_bt_advertiser_set_tx_power(HANDLE_IBEACON,0,30);

  /* Set custom advertising data */
  sl_bt_advertiser_set_data(HANDLE_IBEACON, ADV_PKT, len, pData);

  /* Set advertising parameters. 100ms advertisement interval. All channels used.
   * The first two parameters are minimum and maximum advertising interval, both in
   * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */

  //no timeout on time nor # events
  sl_bt_advertiser_set_timing(HANDLE_IBEACON, 160, 160, 0, 0);

  sl_bt_advertiser_set_tx_power(HANDLE_IBEACON,0,30);
  /* Start advertising in user mode and enable connections 2000 events before expiring ... just a random number*/
  sl_bt_advertiser_start(HANDLE_IBEACON, advertiser_user_data, advertiser_non_connectable);

}

#endif // end of IBEACON

/* LAB TASK MULTIPLE ADVT: modify your custom URL */
static uint8_t eddystone_data[] = {
    0x02,
    0x01,
    0x06,
    0x03,          //Length of service list
    0x03,          //service list
    0xAA, 0xFE,    //Eddystone ID
    0x11,          // length of service data + length of URL
    0x16,          //service data
    0xAA, 0xFE,   //Eddystone ID
    0x10,          //frame type Eddyston-URL
    0x00,          // tx power
    0x00,          //http://www.
    'b', 'l', 'u', 'e', 'c', 'a', 't', 'e', '.', 'c', 'o', 'm'

};


struct phyName
{
  uint8_t flag;
  uint8_t string[5];
};
struct phyName getPhyName(uint8_t flag)
{

  static const struct phyName PHYs[3] = { {1, "1M"}, {2, "2M"}, {4, "125k"}};
  for (int i = 0; i < 3; i++) {
    if (PHYs[i].flag == flag) {
      return PHYs[i];
    }
  }

  return PHYs[0]; // If not found, return index 0 i.e. 1M
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
  uint8_t system_id[8];

  /* LAB TASK PERIODIC ADVT: initialize periodic advertisement */
//  gecko_init_periodic_advertising();

  /* LAB TASK PERIODIC ADVT: initialize periodic advertising data */
  static uint16_t counter = 0;
  static periodicData myCounter = {
      .length = (uint8_t)5,
      .type = (uint8_t)0xFF,
      .manuf_HI = (uint8_t)0xFF,
      .manuf_LO = (uint8_t)0xFF,
      .counter_LO = (uint8_t)0,
      .counter_HI = (uint8_t)0 };
  periodicData *pData = &myCounter;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!


    case sl_bt_evt_system_boot_id:
      bootMessage(&(evt->data.evt_system_boot));

      /* LAB TASK MULTIPLE ADVT: iBeacons */
       bcnSetupAdvBeaconing();
      sl_app_log("boot event - starting advertising\r\n");
      sc = sl_bt_advertiser_set_timing(HANDLE_DEMO, 160, 160, 0, 0);
//      sl_app_assert(sc == SL_STATUS_OK, "[E: 0x%04x] Failed to set advertising timing\n", (int)sc);

      /* LAB TASK 2M PHY connection: enable scan request */
      sl_app_log("enable scan requests\n");
      sl_bt_advertiser_set_report_scan_request(HANDLE_DEMO,1);

      sc = sl_bt_advertiser_start(
          HANDLE_DEMO,
        advertiser_general_discoverable,
        advertiser_connectable_scannable);
      sl_app_log("advertising started\n");

//      sl_app_assert(sc == SL_STATUS_OK, "[E: 0x%04x] Failed to start advertising\n", (int)sc);

      /* LAB TASK MULTIPLE ADVT: EddyStone */
      sl_bt_advertiser_set_data(HANDLE_EDDYSTONE, ADV_PKT, 30, eddystone_data);
      sl_bt_advertiser_set_configuration(HANDLE_EDDYSTONE,0x01);
      sl_bt_advertiser_set_timing(HANDLE_EDDYSTONE,160,160,0,0);
      sl_bt_advertiser_set_channel_map(HANDLE_EDDYSTONE,0x07);
      sl_bt_advertiser_start(HANDLE_EDDYSTONE,advertiser_user_data, advertiser_non_connectable);
      sl_app_log("eddystone started\n");

      /* LAB TASK EXTENDED ADVT: 4th advertisement */
      sl_bt_advertiser_set_timing(HANDLE_EXTENDED,160,160,0,0);
      sl_bt_advertiser_set_configuration(HANDLE_EXTENDED,0x02); //Anonymous advt
      sl_bt_advertiser_start(0,advertiser_general_discoverable, advertiser_connectable_non_scannable);

      sl_bt_advertiser_set_phy(HANDLE_EXTENDED,gap_coded_phy,gap_2m_phy);
      sl_bt_advertiser_set_phy(HANDLE_EXTENDED,gap_coded_phy,gap_coded_phy); //125k coded PHY

      sl_bt_advertiser_clear_configuration(HANDLE_EXTENDED,0x01);
      sl_bt_advertiser_set_configuration(HANDLE_EXTENDED,0x08);
      sl_bt_advertiser_set_tx_power(HANDLE_EXTENDED,100, power);

      break;

    /* LAB TASK PERIODIC ADVT */
//    case sl_bt_evt_system_boot_id:
//
//       sl_bt_system_set_max_tx_power(100, power);
//       sl_bt_advertiser_set_tx_power(HANDLE_DEMO,30,power);
//
//      sl_app_log("periodic advertising started\n");

//       sl_bt_advertiser_set_timing(HANDLE_DEMO, 160, 160, 0, 0);
//       sl_bt_advertiser_clear_configuration(HANDLE_DEMO,1);
//       sl_bt_advertiser_start(HANDLE_DEMO,advertiser_general_discoverable, advertiser_non_connectable);
//       sl_bt_advertiser_start_periodic_advertising(HANDLE_DEMO,160,160,1);
//
//       /* LAB TASK PERIODIC ADVT: set advertising data and soft timer */
//       sl_bt_advertiser_set_data(HANDLE_DEMO, ADV_PKT, sizeof(periodicData), (const uint8_t *)pData);
//       sl_app_log("\r\n Counter: %d \r\n", counter);
//       sl_bt_system_set_soft_timer(32768, 0, 0);

//
//       break;
//
//    /* LAB TASK PERIODIC ADVT: event for expired timer */
//    case sl_bt_evt_system_soft_timer_id:
//        counter++;
//        pData->counter_HI = (uint8_t) (counter >> 8);
//        pData->counter_LO = (uint8_t) (counter & 0xFF);
//
//        //Reset the advertising data pointers to reflect the change in data
//        sl_bt_advertiser_set_data(0,ADV_PKT, sizeof(periodicData), (const uint8_t *)pData);
//        sl_app_log("\r\n Counter: %d \r\n", counter);
//    break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:

      sl_app_log("connection opened\r\n");

      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      sl_app_log("Connection closed\n");

      sl_bt_advertiser_start(HANDLE_DEMO, advertiser_general_discoverable, advertiser_connectable_scannable);

      sl_app_log("Started advertising\n");

      break;

      /* Events related to OTA upgrading
       ----------------------------------------------------------------------------- */

      /* Check if the user-type OTA Control Characteristic was written.
       * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
    case sl_bt_evt_gatt_server_user_write_request_id:

      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
        /* Set flag to enter to OTA mode */
        boot_to_dfu = 1;
        /* Send response to Write Request */
        sl_bt_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection,
        gattdb_ota_control, SL_STATUS_OK);

        /* Close connection to enter to DFU OTA mode */
        sl_bt_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
      }
      break;

      /* Add additional event handlers as your application requires */

      /* LAB TASK 2M PHY connection: phy status handler */
      case sl_bt_evt_connection_phy_status_id:
          sl_app_log("now using the %s PHY\r\n", getPhyName(evt->data.evt_connection_phy_status.phy).string);
          break;

      /*LAB TASK 2M PHY connection: scan request reporting */
      case sl_bt_evt_advertiser_scan_request_id:
          sl_app_log("received scan request from:   %02x:%02x:%02x:%02x:%02x:%02x\r\n",
          evt->data.evt_advertiser_scan_request.address.addr[5],
          evt->data.evt_advertiser_scan_request.address.addr[4],
          evt->data.evt_advertiser_scan_request.address.addr[3],
          evt->data.evt_advertiser_scan_request.address.addr[2],
          evt->data.evt_advertiser_scan_request.address.addr[1],
          evt->data.evt_advertiser_scan_request.address.addr[0]);
          break;

    default:
      break;
  }
}


/* Print stack version and local Bluetooth address as boot message */
static void bootMessage(struct sl_bt_evt_system_boot_s *bootevt)
{
#if DEBUG_LEVEL
  bd_addr local_addr;
  sl_status_t sc;
  int i;

  sl_app_log("stack version: %u.%u.%u\r\n", bootevt->major, bootevt->minor, bootevt->patch);
  sc = sl_bt_system_get_identity_address(local_addr, 0)->address;

  sl_app_log("local BT device address: ");
  for (i = 0; i < 5; i++) {
    sl_app_log("%2.2x:", local_addr.addr[5 - i]);
  }
  sl_app_log("%2.2x\r\n", local_addr.addr[0]);
#endif
}
