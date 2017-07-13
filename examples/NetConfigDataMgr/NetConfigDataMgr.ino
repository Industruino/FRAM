//  Network configuration data manager
//  Copyright (C) 2017  Industruino <connect@industruino.com>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Developed by Claudio Indellicati <bitron.it@gmail.com>

#include "Fram.h"

// Uncomment only one of the following three directives
#define MODE_READ
//#define MODE_WRITE
//#define MODE_ERASE

#define SERIAL Serial
//#define SERIAL SerialUSB

#define SERIAL_BAUDRATE 9600

// Default settings
#define NETCFG_DEFAULT_MAC_ADDR       0xDE,0xAD,0xBE,0xEF,0xFE,0xED
#define NETCFG_DEFAULT_IP_ADDR        192,168,0,200
#define NETCFG_DEFAULT_NETWORK_MASK   255,255,255,0
#define NETCFG_DEFAULT_GATEWAY_ADDR   192,168,0,1
#define NETCFG_DEFAULT_TFTP_DATA_PORT 46969
//#define NETCFG_DEFAULT_PASSWORD       ' ',' ',' ',' ',' ',' ',' ',' '
#define NETCFG_DEFAULT_PASSWORD       'p','a','s','s','w','o','r','d'

////////////////////////////////////////////////////////////////////////////////

typedef uint8_t MACAddr[6];
typedef uint8_t IPAddr[4];

#define NETCFG_CHECKSUM_SIZE       1
#define NETCFG_CHECKSUM_ADDR       ((uint16_t)(FRAM_SIZE-NETCFG_CHECKSUM_SIZE))
#define NETCFG_DATA_REV_SIZE       1
#define NETCFG_DATA_REV_ADDR       ((uint16_t)(NETCFG_CHECKSUM_ADDR-NETCFG_DATA_REV_SIZE))
#define NETCFG_MAC_ADDR_SIZE       (sizeof(MACAddr))
#define NETCFG_MAC_ADDR_ADDR       ((uint16_t)(NETCFG_DATA_REV_ADDR-NETCFG_MAC_ADDR_SIZE))
#define NETCFG_IP_ADDR_SIZE        (sizeof(IPAddr))
#define NETCFG_IP_ADDR_ADDR        ((uint16_t)(NETCFG_MAC_ADDR_ADDR-NETCFG_IP_ADDR_SIZE))
#define NETCFG_NETWORK_MASK_SIZE   NETCFG_IP_ADDR_SIZE
#define NETCFG_NETWORK_MASK_ADDR   ((uint16_t)(NETCFG_IP_ADDR_ADDR-NETCFG_NETWORK_MASK_SIZE))
#define NETCFG_GATEWAY_ADDR_SIZE   NETCFG_IP_ADDR_SIZE
#define NETCFG_GATEWAY_ADDR_ADDR   ((uint16_t)(NETCFG_NETWORK_MASK_ADDR-NETCFG_GATEWAY_ADDR_SIZE))
#define NETCFG_TFTP_DATA_PORT_SIZE (sizeof(uint16_t))
#define NETCFG_TFTP_DATA_PORT_ADDR ((uint16_t)(NETCFG_GATEWAY_ADDR_ADDR-NETCFG_TFTP_DATA_PORT_SIZE))
#define NETCFG_PASSWORD_SIZE       8
#define NETCFG_PASSWORD_ADDR       ((uint16_t)(NETCFG_TFTP_DATA_PORT_ADDR-NETCFG_PASSWORD_SIZE))

#define NETCFG_NET_CONFIG_DATA_SIZE ((uint16_t)(FRAM_SIZE-NETCFG_PASSWORD_ADDR))
#define NETCFG_NET_CONFIG_DATA_ADDR ((uint16_t)(FRAM_SIZE-NETCFG_NET_CONFIG_DATA_SIZE))

#define NETCFG_CONFIG_DATA_REVISION ((uint8_t)(1))

typedef union _NetConfigData
{
  uint8_t rawData[NETCFG_NET_CONFIG_DATA_SIZE];
  struct
  {
    char password[NETCFG_PASSWORD_SIZE];
    uint16_t tftpDataPort;
    IPAddr gatewayAddr;
    IPAddr networkMask;
    IPAddr ipAddr;
    MACAddr macAddr;
    uint8_t revision;
    uint8_t checksum;
  };
} NetConfigData;

NetConfigData defaultNetConfig =
{ .rawData =
  {
    NETCFG_DEFAULT_PASSWORD,
    (NETCFG_DEFAULT_TFTP_DATA_PORT & 0xFF),
    (NETCFG_DEFAULT_TFTP_DATA_PORT >> 8),
    NETCFG_DEFAULT_GATEWAY_ADDR,
    NETCFG_DEFAULT_NETWORK_MASK,
    NETCFG_DEFAULT_IP_ADDR,
    NETCFG_DEFAULT_MAC_ADDR,
    NETCFG_CONFIG_DATA_REVISION,
    0U
  }
};


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  SERIAL.begin(SERIAL_BAUDRATE);
  while (!SERIAL);

  Fram.begin();

  digitalWrite(LED_BUILTIN, HIGH);

#if defined(MODE_READ)

  NetConfigData memoryNetConfig;

  SERIAL.print("Reading network configuration data from FRAM... ");
  if (netcfgReadDataFromMemory(&memoryNetConfig)) SERIAL.println("ERROR: data out of bound");
  SERIAL.println("done");

  if (netcfgCheckData(&memoryNetConfig))
    dumpNetConfigData(&memoryNetConfig, "FRAM network configuration data");
  else
    SERIAL.println("ERROR: memory data not valid");

#elif defined(MODE_WRITE)

  SERIAL.print("Writing network configuration data to FRAM... ");
  if (netcfgWriteDataToMemory(&defaultNetConfig)) SERIAL.println("ERROR: data out of bound");
  SERIAL.println("done");

  NetConfigData memoryNetConfig;

  SERIAL.print("Reading back network configuration data from FRAM... ");
  if (netcfgReadDataFromMemory(&memoryNetConfig)) SERIAL.println("ERROR: data out of bound");
  SERIAL.println("done");

  if (netcfgCheckData(&memoryNetConfig))
    dumpNetConfigData(&memoryNetConfig, "FRAM network configuration data");
  else
    SERIAL.println("ERROR: memory data not valid");

#elif defined(MODE_ERASE)

  SERIAL.print("Erasing network configuration data... ");
  netcfgEraseData();
  SERIAL.println("done");

#endif
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100UL);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100UL);
}

void dumpNetConfigData (NetConfigData *ncd, char *msg)
{
  SERIAL.println(msg);
  SERIAL.print("  Checksum ............ "); SERIAL.println(ncd->checksum);
  SERIAL.print("  Revision ............ "); SERIAL.println(ncd->revision);
  SERIAL.print("  MAC address ......... "); dumpArray(ncd->macAddr, NETCFG_MAC_ADDR_SIZE, HEX); SERIAL.println();
  SERIAL.print("  IP address .......... "); dumpArray(ncd->ipAddr, NETCFG_IP_ADDR_SIZE, DEC); SERIAL.println();
  SERIAL.print("  Network mask ........ "); dumpArray(ncd->networkMask, NETCFG_IP_ADDR_SIZE, DEC); SERIAL.println();
  SERIAL.print("  Gateway address ..... "); dumpArray(ncd->gatewayAddr, NETCFG_IP_ADDR_SIZE, DEC); SERIAL.println();
  SERIAL.print("  TFTP data port ...... "); SERIAL.println(ncd->tftpDataPort);
  SERIAL.print("  Password ............ "); dumpCharArray(ncd->password, NETCFG_PASSWORD_SIZE); SERIAL.println();
}

void dumpArray (uint8_t *a, uint16_t s, int mode)
{
  for (uint16_t i = 0; i < s; ++i)
  {
    if (i != 0U)
      SERIAL.print(", ");

    SERIAL.print(a[i], mode);
  }
}

void dumpCharArray (char *a, uint16_t s)
{
  for (uint16_t i = 0; i < s; ++i)
    SERIAL.print(a[i]);
}

