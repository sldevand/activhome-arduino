#include "MyCC1101.h"

MyCC1101::MyCC1101()
{
  m_pinTx = GDO0;
  m_pinRx = GDO2;
  Init();
}

void MyCC1101::initChacon(unsigned long p_sender)
{
  Init();
  setParameters(m_pinTx, m_pinRx, p_sender);
  configChacon();
  delay(2);
  coeffDelay = 1.0f;
#ifdef LISTENER_MODE
  startListeningChacon();
#endif
}

boolean MyCC1101::sendDataChacon(long sender, int interruptor, byte data)
{
  // CCA enabled: will enter TX state only if the channel is clear
  SetTransmit();

  while (readMARCState() != RF_TX)
  {
  }

  rfState = RFSTATE_TX;
  //Sending data to Chacon
  go(sender, interruptor, data);
  
  SetIdle(); // Enter IDLE state
  // Enter back into RX state
  SetReceive();
  // Declare to be in Rx state
  rfState = RFSTATE_RX;

  return true;
}

#ifdef LISTENER_MODE
void MyCC1101::startListeningChacon()
{
  byte marcState = readMARCState();
  SetIdle();    // Enter IDLE state
  SetReceive(); // Enter RX state

  rfState = RFSTATE_RX;
}

void MyCC1101::readDataChacon()
{
  listenSignal();
}
#endif

void MyCC1101::initRF433Sensors()
{
  setupRadio(m_pinTx, m_pinRx, 2000);
}

void MyCC1101::RF433Transmit(uint8_t *buf, uint8_t buflen)
{
  SetTransmit();
  radioTransmit(buf, buflen);
  SetReceive();
}

void MyCC1101::setCarrierFreq(byte freq)
{
  switch (freq)
  {
  case CFREQ_915:
    SpiWriteReg(CC1101_FREQ2, CC1101_DEFVAL_FREQ2_915);
    SpiWriteReg(CC1101_FREQ1, CC1101_DEFVAL_FREQ1_915);
    SpiWriteReg(CC1101_FREQ0, CC1101_DEFVAL_FREQ0_915);
    break;
  case CFREQ_433:
    SpiWriteReg(CC1101_FREQ2, CC1101_DEFVAL_FREQ2_433);
    SpiWriteReg(CC1101_FREQ1, CC1101_DEFVAL_FREQ1_433);
    SpiWriteReg(CC1101_FREQ0, CC1101_DEFVAL_FREQ0_433);
    break;
  default:
    SpiWriteReg(CC1101_FREQ2, CC1101_DEFVAL_FREQ2_868);
    SpiWriteReg(CC1101_FREQ1, CC1101_DEFVAL_FREQ1_868);
    SpiWriteReg(CC1101_FREQ0, CC1101_DEFVAL_FREQ0_868);
    break;
  }

  carrierFreq = freq;
}

void MyCC1101::setChannel(byte channel)
{
  if (channel < 150)
    SpiWriteReg(CC1101_CHANNR, channel);
}

void MyCC1101::configChacon()
{

  SpiWriteReg(CC1101_IOCFG2, 0x0D);
  SpiWriteReg(CC1101_IOCFG1, 0x2E);
  SpiWriteReg(CC1101_IOCFG0, 0x2E);
  SpiWriteReg(CC1101_FIFOTHR, 0x47);
  SpiWriteReg(CC1101_PKTCTRL0, 0x32);
  SpiWriteReg(CC1101_CHANNR, 0x00);
  SpiWriteReg(CC1101_FSCTRL1, 0x06);
  SpiWriteReg(CC1101_FREQ2, 0x10);
  SpiWriteReg(CC1101_FREQ1, 0xb1);
  SpiWriteReg(CC1101_FREQ0, 0x3b);
  SpiWriteReg(CC1101_MDMCFG4, 0xC9);
  SpiWriteReg(CC1101_MDMCFG3, 0x93);
  SpiWriteReg(CC1101_MDMCFG2, 0x33);
  SpiWriteReg(CC1101_DEVIATN, 0x15);
  SpiWriteReg(CC1101_MCSM0, 0x18);
  SpiWriteReg(CC1101_FOCCFG, 0x16);
  SpiWriteReg(CC1101_WORCTRL, 0xFB);
  SpiWriteReg(CC1101_FREND0, 0x11);
  SpiWriteReg(CC1101_FSCAL3, 0xE9);
  SpiWriteReg(CC1101_FSCAL2, 0x2A);
  SpiWriteReg(CC1101_FSCAL1, 0x00);
  SpiWriteReg(CC1101_FSCAL0, 0x1F);
  SpiWriteReg(CC1101_TEST2, 0x81);
  SpiWriteReg(CC1101_TEST1, 0x35);
  SpiWriteReg(CC1101_TEST0, 0x09);

  byte PA_TABLE[] = {0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  //notice the burst mode to write beyong the first position (0) ...
  SpiWriteBurstReg(CC1101_PATABLE, PA_TABLE, 8);
}

byte MyCC1101::readMARCState()
{

  return SpiReadStatus(CC1101_MARCSTATE) & 0x1F;
}

void MyCC1101::SetTransmit()
{
  SpiStrobe(CC1101_STX);
}

void MyCC1101::SetIdle()
{
  SpiStrobe(CC1101_SIDLE);
}

void MyCC1101::flushRxFIFO()
{

  SpiStrobe(CC1101_SFRX);
}

void MyCC1101::setTxPin(uint8_t txPin) { m_pinTx = txPin; }
void MyCC1101::setRxPin(uint8_t rxPin) { m_pinRx = rxPin; }
