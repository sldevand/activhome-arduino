#define VERSION "2.2.3"


#define MAX_BUF_LEN 64

/****OBJECTS*****/
RF24Sensors *sensors24;
MyCC1101 *cc1101;
Utils *utils;
Timer t;

void displayFreemem();
void serialEvent();
void checkBluetoothBuffer();
void checkCommands(char *buf);
void RTCTherPut();
void RTCPut();
void dayPlanPut();
void sensors24RadioReceive();
void bindThermostat();
void bindThermostat2();
void bindMode();
void bindDayplan();
void bindRTC();
void print2digits(int number);
void bindMessage();
void sensors433RadioReceive();
void displaySensor(Sensor sensor);
