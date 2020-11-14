#include "main.h"

void setup()
{
    utils = new Utils();
    pinMode(CSN_PIN, OUTPUT);
    digitalWrite(CSN_PIN, HIGH);
    pinMode(CSNCC11_PIN, OUTPUT);
    digitalWrite(CSNCC11_PIN, HIGH);

    //Serial and pins init
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.print("PiAssist ");
    Serial.print("v");
    Serial.println(VERSION);
    delay(SHORT_DELAY);

    delay(DEFAULT_DELAY);
    sensors24 = new RF24Sensors(CE_PIN, CSN_PIN);
    delay(SHORT_DELAY);

    cc1101 = new MyCC1101();
    delay(SHORT_DELAY);
    cc1101->initChacon(14549858);
    delay(LONG_DELAY);
    cc1101->initRF433Sensors();
    cc1101->SetReceive();
    delay(LONG_DELAY);
    digitalWrite(CSNCC11_PIN, HIGH);
    Serial.println("OK!");
    delay(DEFAULT_DELAY);
}

void loop()
{
    sensors433RadioReceive();
    sensors24RadioReceive();
    t.update();
}

void serialEvent()
{
    String inputString = String("");
    while (Serial.available())
    {
        inputString += (char)Serial.read();
        delay(1);
    }
    char buf[MAX_BUF_LEN];
    inputString.toCharArray(buf, MAX_BUF_LEN);
    checkCommands(buf);
}

void checkCommands(char *buf)
{
    char separator[] = "/";

    utils->splitString(buf, separator);

    //--------->CC1101<-------------
    if (strcmp(utils->m_tabString[0], MDL_CC1101) == 0)
    {
        //--------->CHACON<-------------
        // cc1101/chacon/14549858/2/0/
        if (strcmp(utils->m_tabString[1], PRT_CHACON) == 0)
        {
            long sender = atol(utils->m_tabString[2]);
            int interruptor = atoi(utils->m_tabString[3]);
            byte onoff = (byte)atoi(utils->m_tabString[4]);
            cc1101->stopRadio();
            cc1101->sendDataChacon(sender, interruptor, onoff);
            cc1101->startRadio();
        }
    }

    //--------->NRF24L01<-----------
    if (strcmp(utils->m_tabString[0], MDL_NRF24) == 0)
    {
        ComStruct command;
        if (strcmp(utils->m_tabString[1], KEY_INIT) == 0)
        {
            sensors24->setupRadio();
        }
        if (strcmp(utils->m_tabString[1], KEY_TEST) == 0)
        {
            printf_begin();
            sensors24->printDetails();
        }
        if (strcmp(utils->m_tabString[1], PRT_NODE) == 0 && utils->m_counter > 4)
        {
            byte writingPipe[6];
            for (int i = 0; i < 6; i++)
            {
                writingPipe[i] = utils->m_tabString[2][i];
            }

            sensors24->openWritingPipe(writingPipe);
            //nrf24/node/2Nodw/ther/
            if (strcmp(utils->m_tabString[3], PRT_THERM) == 0)
            {
                //nrf24/node/2Nodw/ther/set/
                if (strcmp(utils->m_tabString[4], METH_SET) == 0)
                {
                    strcpy(command.meth, METH_SET);
                    //nrf24/node/2Nodw/ther/set/cons/19/
                    if (strcmp(utils->m_tabString[5], KEY_CONSIGN) == 0)
                    {
                        sensors24->m_thermostat.consigne = atof(utils->m_tabString[6]);
                        sensors24->radioTransmit(sensors24->m_thermostat);
                    }
                    //nrf24/node/2Nodw/ther/set/delta/0.7/
                    if (strcmp(utils->m_tabString[5], KEY_DELTA) == 0)
                    {
                        sensors24->m_thermostat.delta = atof(utils->m_tabString[6]);
                        sensors24->radioTransmit(sensors24->m_thermostat);
                    }
                    //nrf24/node/2Nodw/ther/set/temp/19.5/
                    if (strcmp(utils->m_tabString[5], KEY_TEMP) == 0)
                    {
                        sensors24->m_thermostat.tempExt = atof(utils->m_tabString[6]);
                        sensors24->radioTransmit(sensors24->m_thermostat);
                    }
                    //nrf24/node/2Nodw/ther/set/int/1/
                    if (strcmp(utils->m_tabString[5], KEY_INTERNE) == 0)
                    {
                        sensors24->m_thermostat.interne = atoi(utils->m_tabString[6]);
                        sensors24->radioTransmit(sensors24->m_thermostat);
                    }

                    //nrf24/node/2Nodw/ther/set/plan/1/
                    if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0)
                    {
                        sensors24->m_thermostat.plan = atoi(utils->m_tabString[6]);
                        sensors24->radioTransmit(sensors24->m_thermostat);
                    }

                    //nrf24/node/2Nodw/ther/set/pwr/1/
                    if (strcmp(utils->m_tabString[5], KEY_PWR) == 0)
                    {
                        strcpy(command.key, KEY_PWR);
                        strcpy(command.buf, utils->m_tabString[6]);
                        sensors24->radioTransmit(command);
                    }
                }
                //nrf24/node/2Nodw/ther/get/
                if (strcmp(utils->m_tabString[4], METH_GET) == 0)
                {
                    strcpy(command.meth, METH_GET);

                    //nrf24/node/2Nodw/ther/get/info/
                    if (strcmp(utils->m_tabString[5], KEY_INFO) == 0)
                    {
                        strcpy(command.key, KEY_INFO);
                        sensors24->radioTransmit(command);
                    }
                    //nrf24/node/2Nodw/ther/get/mode/1/
                    if (strcmp(utils->m_tabString[5], KEY_MODE) == 0)
                    {
                        strcpy(command.key, KEY_MODE);
                        strcpy(command.buf, utils->m_tabString[6]);
                        sensors24->radioTransmit(command);
                    }

                    //nrf24/node/2Nodw/ther/get/pwr/
                    if (strcmp(utils->m_tabString[5], KEY_PWR) == 0)
                    {
                        strcpy(command.key, KEY_PWR);
                        sensors24->radioTransmit(command);
                    }

                    //nrf24/node/2Nodw/ther/get/plan/1/
                    if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0)
                    {
                        strcpy(command.key, KEY_PLAN);
                        strcpy(command.buf, utils->m_tabString[6]);
                        sensors24->radioTransmit(command);
                    }
                    //nrf24/node/2Nodw/ther/get/rtc/
                    if (strcmp(utils->m_tabString[5], KEY_RTC) == 0)
                    {
                        strcpy(command.key, KEY_RTC);
                        sensors24->radioTransmit(command);
                    }
                }
                //nrf24/node/2Nodw/ther/sel/
                if (strcmp(utils->m_tabString[4], METH_SEL) == 0)
                {
                    strcpy(command.meth, METH_SEL);
                    //nrf24/node/2Nodw/ther/sel/mode/1/
                    if (strcmp(utils->m_tabString[5], KEY_MODE) == 0)
                    {
                        strcpy(command.key, KEY_MODE);
                        strcpy(command.buf, utils->m_tabString[6]);
                        sensors24->radioTransmit(command);
                    }
                }

                //nrf24/node/2Nodw/ther/put/
                if (strcmp(utils->m_tabString[4], METH_PUT) == 0)
                {
                    //nrf24/node/2Nodw/ther/put/mode/4/23.5/0.7/
                    if (strcmp(utils->m_tabString[5], KEY_MODE) == 0)
                    {
                        Mode mode;
                        mode.id = atoi(utils->m_tabString[6]);
                        mode.consigne = atof(utils->m_tabString[7]);
                        mode.delta = atof(utils->m_tabString[8]);
                        sensors24->radioTransmit(mode);
                    }

                    //nrf24/node/2Nodw/ther/put/plan/2/1/3/06:00/23:00/XX:XX/XX:XX/
                    if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0)
                    {
                        dayPlanPut();
                    }

                    //nrf24/node/2Nodw/ther/put/rtc/6/2018/02/15/15/34/22/
                    if (strcmp(utils->m_tabString[5], KEY_RTC) == 0)
                    {
                        RTCTherPut();
                    }
                }
                //nrf24/node/2Nodw/ther/save/
                if (strcmp(utils->m_tabString[4], METH_SAVE) == 0)
                {
                    strcpy(command.meth, METH_SAVE);
                    //nrf24/node/2Nodw/ther/save/plan/
                    if (strcmp(utils->m_tabString[5], KEY_PLAN) == 0)
                    {
                        strcpy(command.key, KEY_PLAN);
                        sensors24->radioTransmit(command);
                    }

                    //nrf24/node/2Nodw/ther/save/mode/
                    if (strcmp(utils->m_tabString[5], KEY_MODE) == 0)
                    {
                        strcpy(command.key, KEY_MODE);
                        sensors24->radioTransmit(command);
                    }
                }
            }
        }
    }
}

void RTCTherPut()
{
    tmElements_t tm;
    tm.Wday = atoi(utils->m_tabString[6]);
    tm.Year = CalendarYrToTm(atoi(utils->m_tabString[7]));
    tm.Month = atoi(utils->m_tabString[8]);
    tm.Day = atoi(utils->m_tabString[9]);
    tm.Hour = atoi(utils->m_tabString[10]);
    tm.Minute = atoi(utils->m_tabString[11]);
    tm.Second = atoi(utils->m_tabString[12]);
    sensors24->radioTransmit(tm);
}

void dayPlanPut()
{
    sensors24->m_dayplan.jour = atoi(utils->m_tabString[6]);
    sensors24->m_dayplan.modeId = atoi(utils->m_tabString[7]);
    sensors24->m_dayplan.defaultModeId = atoi(utils->m_tabString[8]);
    strcpy(sensors24->m_dayplan.heure1Start, utils->m_tabString[9]);
    strcpy(sensors24->m_dayplan.heure1Stop, utils->m_tabString[10]);
    strcpy(sensors24->m_dayplan.heure2Start, utils->m_tabString[11]);
    strcpy(sensors24->m_dayplan.heure2Stop, utils->m_tabString[12]);

    sensors24->radioTransmit(sensors24->m_dayplan);
}

void sensors24RadioReceive()
{
    uint8_t payloadSize = sensors24->radioReceive();
    switch (payloadSize)
    {
    case sizeof(Sensor):
        displaySensor(sensors24->m_sensor);
        break;
    case sizeof(Thermostat):
        bindThermostat();
        break;
    case sizeof(Mode):
        bindMode();
        break;
    case sizeof(DayPlan):
        bindDayplan();
        break;
    case sizeof(tmElements_t):
        bindRTC();
        break;
    case sizeof(Message):
        bindMessage();
        break;
    }
}

void bindThermostat()
{
    String mergedString = "thermostat ";
    mergedString += String(sensors24->m_thermostat.consigne);
    mergedString += " ";
    mergedString += String(sensors24->m_thermostat.delta);
    mergedString += " ";
    mergedString += String(sensors24->m_thermostat.tempExt);
    mergedString += " ";
    mergedString += String(sensors24->m_thermostat.interne);
    mergedString += " ";
    mergedString += String(sensors24->m_thermostat.etat);
    Serial.println(mergedString);

    t.after(300, bindThermostat2);
}

void bindThermostat2()
{
    String mergedString = "thersel ";
    mergedString += String(sensors24->m_thermostat.mode);
    mergedString += " ";
    mergedString += String(sensors24->m_thermostat.plan);
    Serial.println(mergedString);

    delay(100);
}

void bindMode()
{
    String mergedString = "thermode ";
    mergedString += String(sensors24->m_mode.id);
    mergedString += " ";
    mergedString += String(sensors24->m_mode.consigne);
    mergedString += " ";
    mergedString += String(sensors24->m_mode.delta);
    Serial.println(mergedString);

    delay(100);
}

void bindDayplan()
{

    String mergedString = "therplan ";
    mergedString += String(sensors24->m_dayplan.jour);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.modeId);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.defaultModeId);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.heure1Start);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.heure1Stop);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.heure2Start);
    mergedString += " ";
    mergedString += String(sensors24->m_dayplan.heure2Stop);

    Serial.println(mergedString);

    delay(100);
}

void bindRTC()
{
    tmElements_t tm = sensors24->m_rtc;
    Serial.print("therclock ");
    Serial.print(tm.Wday);
    Serial.write(' ');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.write('/');
    print2digits(tm.Month);
    Serial.write('/');
    print2digits(tm.Day);
    Serial.write(' ');
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.println("");
    delay(100);
}
void print2digits(int number)
{
    if (number >= 0 && number < 10)
    {
        Serial.print('0');
    }
    Serial.print(number);
}
void bindMessage()
{
    String mergedString = "message ";
    mergedString += String(sensors24->m_message.buf);

    Serial.println(mergedString);

    delay(100);
}

void sensors433RadioReceive()
{
    if (!cc1101->hasMessage())
    {
        return;
    }
    Sensor sensor = cc1101->radioRawReceive();
    displaySensor(sensor);
}

void displaySensor(Sensor sensor)
{
    if (strlen(sensor.id) != 16)
    {
        return;
    }

    String mergedString = sensor.id;
    mergedString += " ";
    mergedString += String(sensor.temp);

    if (sensor.hygro >= 0)
    {
        mergedString += " ";
        mergedString += String(sensor.hygro);
    }
    Serial.println(mergedString);

    delay(100);
}
