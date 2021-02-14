/***************************************************************************//**
 * @file
 * @brief Compass GUI.
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

#ifndef GUI_H
#define GUI_H

void gui_init(int argc, char *argv[]);
void gui_main(void);
void gui_update(const float azimuth, const int rssi, const float elevation);

#endif // GUI_H
