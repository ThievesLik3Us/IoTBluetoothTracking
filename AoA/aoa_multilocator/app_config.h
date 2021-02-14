/***************************************************************************//**
 * @file
 * @brief Application configuration values.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// Maximum number of locators handled by the application.
#define MAX_NUM_LOCATORS        6

// Maximum number of asset tags handled by the application.
#define MAX_NUM_TAGS            50

// Location estimation mode.
#define ESTIMATION_MODE         SL_RTL_LOC_ESTIMATION_MODE_THREE_DIM_HIGH_ACCURACY

// Estimation interval in seconds.
// This value should approximate the time interval between two consecutive CTEs.
#define ESTIMATION_INTERVAL_SEC 0.1f

// Filter weight applied on the location coordinates. Ranges from 0 to 1.
#define FILTERING_AMOUNT        0.1f

#endif // APP_CONFIG_H
