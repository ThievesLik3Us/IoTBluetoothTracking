import sys

# left side is sdk 2 replaced with right side sdk 3
word_replacement = {
# BGAPI Classes Changed
"le_gap":"gap",
"le_connection":"connection",
"flash":"nvm",
# BGAPI Commands Changed
"gecko_cmd_le_gap_enable_whitelisting": "sl_bt_gap_enable_whitelisting",
"gecko_cmd_le_gap_set_data_channel_classification": "sl_bt_gap_set_data_channel_classification",
"gecko_cmd_le_gap_set_privacy_mode": "sl_bt_gap_set_privacy_mode",
"gecko_cmd_le_gap_bt5_set_adv_parameters": "sl_bt_advertiser_set_???",
"gecko_cmd_le_gap_set_mode": "sl_bt_advertiser_(start_OR_stop)",
"gecko_cmd_le_gap_bt5_set_mode": "sl_bt_advertiser_???",
"gecko_cmd_le_gap_set_adv_data": "sl_bt_advertiser_set_data OR sl_bt_ota_set_advertising_data",
"gecko_cmd_le_gap_set_adv_parameters": "sl_bt_advertiser_set_timing OR sl_bt_advertiser_set_channel_map",
"gecko_cmd_le_gap_set_adv_timeout": "sl_bt_advertiser_set_timing",
"gecko_cmd_le_gap_set_advertise_timing": "sl_bt_advertiser_set_timing",
"gecko_cmd_le_gap_set_advertise_report_scan_request": "sl_bt_advertiser_set_report_scan_request",
"gecko_cmd_le_gap_set_advertise_phy": "sl_bt_advertiser_set_phy",
"gecko_cmd_le_gap_set_advertise_channel_map": "sl_bt_advertiser_set_channel_map",
"gecko_cmd_le_gap_set_advertise_configuration": "sl_bt_advertiser_set_configuration",
"gecko_cmd_le_gap_clear_advertise_configuration": "sl_bt_advertiser_clear_configuration",
"gecko_cmd_le_gap_set_advertise_tx_power": "sl_bt_advertiser_set_tx_power",
"gecko_cmd_le_gap_set_long_advertising_data": "sl_bt_advertiser_set_long_data",
"gecko_cmd_le_gap_start_advertising": "sl_bt_advertiser_start",
"gecko_cmd_le_gap_stop_advertising": "sl_bt_advertiser_stop",
"gecko_cmd_le_gap_start_periodic_advertising": "sl_bt_advertiser_start_periodic_advertising",
"gecko_cmd_le_gap_stop_periodic_advertising": "sl_bt_advertiser_stop_periodic_advertising",
"gecko_cmd_le_gap_set_advertise_random_address": "sl_bt_advertiser_set_random_address",
"gecko_cmd_le_gap_clear_advertise_random_address": "sl_bt_advertiser_clear_random_address",
"gecko_cmd_le_gap_open": "sl_bt_connection_open",
"gecko_cmd_le_gap_connect": "sl_bt_connection_open",
"gecko_cmd_le_gap_set_scan_parameters": "sl_bt_scanner_set_(timing_OR_type)",
"gecko_cmd_le_gap_set_discovery_timing": "sl_bt_scanner_set_timing",
"gecko_cmd_le_gap_set_discovery_type": "sl_bt_scanner_set_mode",
"gecko_cmd_le_gap_start_discovery": "sl_bt_scanner_start",
"gecko_cmd_le_gap_discover": "sl_bt_scanner_start",
"gecko_cmd_le_gap_end_procedure": "sl_bt_scanner_stop",
"gecko_cmd_le_gap_set_conn_phy": "sl_bt_connection_set_default_preferred_phy",
"gecko_cmd_le_gap_set_conn_parameters": "sl_bt_connection_set_default_parameters",
"gecko_cmd_le_gap_set_conn_timing_parameters": "sl_bt_connection_set_default_parameters",
"gecko_cmd_le_connection_close": "sl_bt_connection_close",
"gecko_cmd_le_connection_disable_slave_latency": "sl_bt_connection_disable_slave_latency",
"gecko_cmd_le_connection_get_rssi": "sl_bt_connection_get_rssi",
"gecko_cmd_le_connection_read_channel_map": "sl_bt_connection_read_channel_map",
"gecko_cmd_le_connection_set_parameters": "sl_bt_connection_set_parameters",
"gecko_cmd_le_connection_set_timing_parameters": "sl_bt_connection_set_parameters",
"gecko_cmd_le_connection_set_phy": "sl_bt_connection_set_preferred_phy",
"gecko_cmd_system_get_bt_address": "sl_bt_system_get_identity_address",
"gecko_cmd_system_set_bt_address ": "sl_bt_system_set_identity_address",
"gecko_cmd_system_set_tx_power": "sl_bt_system_set_max_tx_power",
"gecko_cmd_system_set_device_name": "sl_bt_ota_set_device_name",
"gecko_cmd_hardware_set_lazy_soft_timer": "sl_bt_system_set_lazy_soft_timer",
"gecko_cmd_hardware_set_soft_timer": "sl_bt_system_set_soft_timer",
"gecko_cmd_flash_ps_erase": "sl_bt_nvm_erase",
"gecko_cmd_flash_ps_erase_all": "sl_bt_nvm_erase_all",
"gecko_cmd_flash_ps_load": "sl_bt_nvm_load",
"gecko_cmd_flash_ps_save": "sl_bt_nvm_save",
# BGAPI Events Changes
"gecko_evt_le_gap_scan_response_id": "sl_bt_evt_scanner_scan_report_id",
"struct gecko_msg_le_gap_scan_response_evt_t": "sl_bt_evt_scanner_scan_report_t",
"gecko_msg_le_gap_scan_response_evt_t": "sl_bt_evt_scanner_scan_report_t",
"gecko_evt_le_gap_extended_scan_response_id": "sl_bt_evt_scanner_scan_report_id",
"struct gecko_msg_le_gap_extended_scan_response_evt_t": "sl_bt_evt_scanner_scan_report_t",
"gecko_msg_le_gap_extended_scan_response_evt_t": "sl_bt_evt_scanner_scan_report_t",
"gecko_evt_le_gap_adv_timeout_id": "sl_bt_evt_advertiser_timeout_id",
"struct gecko_msg_le_gap_adv_timeout_evt_t": "sl_bt_evt_advertiser_timeout_t",
"gecko_msg_le_gap_adv_timeout_evt_t": "sl_bt_evt_advertiser_timeout_t",
"gecko_evt_le_gap_scan_request_id": "sl_bt_evt_advertiser_scan_request_id",
"struct gecko_msg_le_gap_scan_request_evt_t": "sl_bt_evt_advertiser_scan_request_t",
"gecko_msg_le_gap_scan_request_evt_t": "sl_bt_evt_advertiser_scan_request_t",
"gecko_evt_hardware_soft_timer_id": "sl_bt_evt_system_soft_timer_id",
"struct gecko_msg_evt_hardware_soft_timer_t": "sl_bt_evt_system_soft_timer_t",
"gecko_msg_evt_hardware_soft_timer_t": "sl_bt_evt_system_soft_timer_t",
"gecko_evt_le_connection_opened_id": "sl_bt_evt_connection_opened_id",
"struct gecko_msg_evt_le_connection_opened_t": "sl_bt_evt_connection_opened_t",
"gecko_msg_evt_le_connection_opened_t": "sl_bt_evt_connection_opened_t",
"gecko_evt_le_connection_closed_id": "sl_bt_evt_connection_closed_id",
"struct gecko_msg_evt_le_connection_closed_t": "sl_bt_evt_connection_closed_t",
"gecko_msg_evt_le_connection_closed_t": "sl_bt_evt_connection_closed_t",
"gecko_evt_le_connection_rssi_id": "sl_bt_evt_connection_rssi_id",
"struct gecko_msg_evt_le_connection_rssi_t": "sl_bt_evt_connection_rssi_t",
"gecko_msg_evt_le_connection_rssi_t": "sl_bt_evt_connection_rssi_t",
"gecko_evt_le_connection_parameters_id": "sl_bt_evt_connection_parameters_id",
"struct gecko_msg_evt_le_connection_parameters_t": "sl_bt_evt_connection_parameters_t",
"gecko_msg_evt_le_connection_parameters_t": "sl_bt_evt_connection_parameters_t",
"gecko_evt_le_connection_phy_status_id": "sl_bt_evt_connection_phy_status_id",
"struct gecko_msg_evt_le_connection_phy_status_t": "sl_bt_evt_connection_phy_status_t",
"gecko_msg_evt_le_connection_phy_status_t": "sl_bt_evt_connection_phy_status_t",
# BGAPI Enumerations and Definitions Changed
"le_gap_address_type_public": "gap_public_address",
"le_gap_address_type_random": "gap_static_address",
"le_gap_address_type_public_identity": "gap_random_resolvable_address",
"le_gap_address_type_random_identity": "gap_random_nonresolvable_address",
"le_gap_phy_1m": "gap_1m_phy",
"le_gap_phy_2m": "gap_2m_phy",
"le_gap_phy_coded": "gap_coded_phy",
"le_gap_non_discoverable": "advertiser_non_discoverable",
"le_gap_limited_discoverable": "advertiser_limited_discoverable",
"le_gap_general_discoverable": "advertiser_general_discoverable",
"le_gap_broadcast": "advertiser_broadcast",
"le_gap_user_data": "advertiser_user_data",
"le_gap_non_connectable": "advertiser_non_connectable",
"le_gap_directed_connectable": "advertiser_directed_connectable",
"le_gap_undirected_connectable": "advertiser_connectable_scannable",
"le_gap_connectable_scannable": "advertiser_connectable_scannable",
"le_gap_scannable_non_connectable": "advertiser_scannable_non_connectable",
"le_gap_connectable_non_scannable": "advertiser_connectable_non_scannable",
"le_gap_discover_limited": "scanner_discover_limited",
"le_gap_discover_generic": "scanner_discover_generic",
"le_gap_discover_observation": "scanner_discover_observation",
"FLASH_PS_KEY_CTUNE": "NVM_KEY_CTUNE",
# C API Changes
"struct gecko_cmd_packet": "sl_bt_msg_t",
"gecko_cmd_packet": "sl_bt_msg_t",
"BGLIB_MSG_ID": "SL_BT_MSG_ID",
"struct gecko_cmd_packet* gecko_wait_event()": "sl_status_t sl_bt_wait_event(sl_bt_msg_t* evt)",
"gecko_cmd_packet* gecko_wait_event()": "sl_bt_wait_event(sl_bt_msg_t* evt)",
"gecko_wait_event()": "sl_bt_wait_event(sl_bt_msg_t* evt)",
"struct gecko_cmd_packet* gecko_peek_event()": "sl_status_t sl_bt_pop_event(sl_bt_msg_t* evt)",
"gecko_cmd_packet* gecko_peek_event()": "sl_bt_pop_event(sl_bt_msg_t* evt)",
"gecko_peek_event()": "sl_bt_pop_event(sl_bt_msg_t* evt)",
"int gecko_event_pending()": "bool sl_bt_event_pending()",
"gecko_event_pending()": "sl_bt_event_pending()",
"errorcode_t gecko_stack_init(const gecko_configuration_t *config)": "sl_status_t sl_bt_init_stack(const sl_bt_configuration_t *config)",
"gecko_stack_init(const gecko_configuration_t *config)": "sl_bt_init_stack(const sl_bt_configuration_t *config)",
"gecko_stack_init": "sl_bt_init_stack",
"gecko_can_sleep_ms()": "",
"gecko_can_sleep_ticks": "",
"gecko_sleep_for_ms": "",
"gecko_priority_handle": "sl_bt_priority_handle",
"gecko_external_signal": "sl_bt_external_signal",
"gecko_send_system_awake": "sl_bt_send_system_awake",
"gecko_send_system_error": "sl_bt_send_system_error",
"gecko_send_evt_user_message_to_host": "sl_bt_send_evt_user_message_to_host",
"gecko_send_rsp_user_message_to_target": "sl_bt_send_rsp_user_message_to_target",
# Bluetooth Configuration Type Changes
"gecko_configuration_t": "sl_bt_configuration_t",
"gecko_bluetooth_config_t": "sl_bt_stack_config_t",
# Bluetooth init Functions Changes
"gecko_stack_init": "sl_bt_init_stack",
"gecko_bgapi_class_": "sl_bt_class_",
"gecko_init_": "sl_bt_init_",
# Headers Changed (may not be exact translation)
"bg_types.h": "sl_bt_types.h",
"bg_errorcodes.h": "sl_status.h",
"host_gecko.h": "sl_bt_api.h",
"gecko_bglib.h": "sl_bt_ncp_host.h",
"gecko_bglib.c": "sl_bt_ncp_host_api.c \n #include <sl_bt_ncp_host.c>",
"native_gecko.h": "sl_bt_api.h",
# Host API Changes
"BGLIB_DEFINE": "SL_BT_API_DEFINE",
"BGLIB_INITIALIZE": "SL_BT_API_INITIALIZE",
"BGLIB_INITIALIZE_NONBLOCK": "SL_BT_API_INITIALIZE_NONBLOCK",
"struct gecko_cmd_packet": "sl_bt_msg_t",
"gecko_cmd_packet": "sl_bt_msg_t",
"BGLIB_MSG_ID": "SL_BT_MSG_ID"
}

if len(sys.argv) == 1:
    print("Error: Input the file name to read from")
    exit()

# read all words from a file and then close the file
with open(sys.argv[1]) as document:
    words = document.readlines()

replaced = []
for word in words:
    # take the original word and replace with new word from list
    for original, replacement in word_replacement.items():
        word = word.replace(original, replacement)
    replaced.append(word)

# add replaced words to a full string to resemble the file
text = ''.join(word_replacement.get(y, y) for y in replaced)

# write the edited text to the file, overwriting everything
new_document = open(sys.argv[1], 'w')
new_document.write(text)
new_document.close()
