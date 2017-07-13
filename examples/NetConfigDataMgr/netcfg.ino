void netcfgSetChecksum (NetConfigData *ncd)
{
  ncd->checksum = 0U;

  for (uint16_t i = 0U; i < (NETCFG_NET_CONFIG_DATA_SIZE - NETCFG_CHECKSUM_SIZE); ++i)
    ncd->checksum += ncd->rawData[i];
}

uint8_t netcfgCheckDataOnly (NetConfigData *ncd)
{
   if (ncd->revision != NETCFG_CONFIG_DATA_REVISION)
      return 0;

   for (uint8_t i = 0U; i < NETCFG_PASSWORD_SIZE; ++i)
      if ((ncd->password[i] < ' ') || (ncd->password[i] > 0x7E))
         return 0;

   return 1;
}

uint8_t netcfgCheckData (NetConfigData *ncd)
{
   if (netcfgCheckDataOnly(ncd))
   {
      uint8_t checksum = 0U;

      for (uint16_t i = 0U; i < (NETCFG_NET_CONFIG_DATA_SIZE - NETCFG_CHECKSUM_SIZE); ++i)
         checksum += ncd->rawData[i];

      if (checksum != ncd->checksum)
         return 0;

      return 1;
   }

   return 0;
}

uint8_t netcfgReadDataFromMemory (NetConfigData *ncd)
{
   return Fram.read(NETCFG_NET_CONFIG_DATA_ADDR, ncd->rawData, NETCFG_NET_CONFIG_DATA_SIZE);
}

uint8_t netcfgWriteDataToMemory (NetConfigData *ncd)
{
   netcfgSetChecksum(ncd);

   if (netcfgCheckDataOnly(ncd))
      return Fram.write(NETCFG_NET_CONFIG_DATA_ADDR, ncd->rawData, NETCFG_NET_CONFIG_DATA_SIZE);

   return 0U;
}

void netcfgEraseData (void)
{
  uint8_t zero = 0U;

  for (uint16_t i = 0; i < NETCFG_NET_CONFIG_DATA_SIZE; ++i)
    Fram.write(NETCFG_NET_CONFIG_DATA_ADDR + i, &zero, 1U);
}

