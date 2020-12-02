# Scripts
This folder is a collection of useful python scripts for different automation that we are doing so if you make
a pyhon script to automate something, throw it in here a write a short description on how to use it.

## convert_sdk2_to_sdk3.py
This python script will take any SDK 2 file written for Simplicity Studio 4 and will convert it to SDK3 for
Simplicity Studio 5. It is a basic script that only does the 1:1 conversions but this will cover about 90-95%
of the code. The rest is up to the developer to switch and decide what is the best function to use. The headers will likely not be correct, you need the following on SDK 3.x:  
• **sl_status.h** (in SDK_DIR/platform/common/inc)  
• **sl_bt_types.h** (in SDK_DIR/protocol/bluetooth/inc)  
• **sl_bt_api.h** (in SDK_DIR/protocol/bluetooth/inc)  
• **sl_bt_ncp_host.h** (in SDK_DIR/protocol/bluetooth/inc)  
• **sl_bt_stack_config.h** (in SDK_DIR/protocol/bluetooth/inc) -  needs to be included for sl_bt_msg_t  
and the following source files:  
• **sl_bt_ncp_host_api.c** (in SDK_DIR/protocol/bluetooth/src)  
• **sl_bt_ncp_host.c** (in SDK_DIR/protocol/bluetooth/src)  
• **if uint_8 is missing then add stdint.h**


Below is an example of how to use the script
```
$python3 convert_sdk2_to_sdk3.py app.c
OR
$python3 convert_sdk2_to_sdk3.py C:\Path\to\file\app.c
```
