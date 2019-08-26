# TECHNIMAL_NB_NE866

This Library is designed for `NE866` to connect with your own server via CoAP over udp protocol. 

This version is recommended for ESP8266, ESP32, Arduino Mega, Arduino MKR boards, Arduino Zero, Arduino Due, Arduino 101, etc.

## Struct
```
struct TECHNIMAL_NB_NE866_MODULE_RES {
	unsigned char status; // 0=failed, 1=success, 2=timeout
	String data;
};

struct TECHNIMAL_NB_NE866_SIGNAL {
	String csq;
	String rssi;
	String ber;
};

struct TECHNIMAL_NB_NE866_PING_RES {
	bool status;
	String addr;
	String ttl;
	String rtt;
};

struct TECHNIMAL_NB_NE866_RAW_RES {
	unsigned char status; // 0=failed, 1=success, 2=timeout
	String ip;
	String port;
	unsigned int length;
	String data;
};

struct TECHNIMAL_NB_NE866_COAP_OPTION {
	int code;
	unsigned char valueType; // 0 = string, 1 = uint
	String valueStr;
	unsigned int valueUInt;
};

struct TECHNIMAL_NB_NE866_COAP_RES {
	unsigned char type;
	unsigned char status;
	String token;
	unsigned int msgID;
	String payload;
};
```

## Function List:

### General Function:
	void SetupModule(Stream* serial, String host, String port)
		○ Initial module
	void RebootModule()
		○ Reboot module
	void WaitModuleReady()
		○ Waiting module ready after reboot module
	String GetIMEI()
		○ Return : Module IMEI in string format
	String GetIMSI()
		○ Return : Module IMSI in string format
	String GetCCID()
		○ Return : Module CCID in string format
	String GetFirmwareVersion()
		○ Return : Module firmware version in string format
	String GetDeviceIP()
		○ Return : Module ip address. Need connect network
	bool Connect()
		○ Connect to NB-IoT network
		○ Return : `True` if success, `False` if failed
	TECHNIMAL_NB_NE866_PING_RES Ping()
		○ Ping
		○ Return : Ping result in TECHNIMAL_NB_NE866_PING_RES struct
	void SetToken(String token)
		○ Set CoAP token maximum is 8 characters
	String GetCoAPHeader(unsigned char method)
		○ Get CoAP header packet
		○ Return : CoAP header packet in hex string format
	String PathToCoAPOptions(int prevOptionCode, String path)
		○ Parse uri path to CoAP option packet
		○ Return : CoAP option packet in hex string format
	String CreateCoAPOption(TECHNIMAL_NB_NE866_COAP_OPTION option)
		○ Create CoAP option packet
		○ Return : CoAP option pakcet in hex string format
	String CreateCoAPOption(int prevOptionCode, TECHNIMAL_NB_NE866_COAP_OPTION option)
		○ Create CoAP option packet
		○ Return : CoAP option pakcet in hex string format
	String CreatePayload(String payload)
		○ Create CoAP payload packet
		○ Return : CoAP payload pakcet in hex string format
	String IntToHex2xStr(unsigned int intin)
		○ Convert int to hex signed 2's string
		○ Return : Hex signed 2's string
	String IntToHexStr(int intin)
		○ Convert int to hex string
		○ Return : Hex string
	String StrToHexStr(String strin)
		○ Convert String to hex string
		○ Return : Hex string
	String HexStrToStr(String hexstr)
		○ Convert Hex string to string
		○ Return : String
	TECHNIMAL_NB_NE866_COAP_RES ParseCoAPMessage(String data)
		○ Parse packet to CoAP message
		○ Return : CoAP message in TECHNIMAL_NB_NE866_COAP_RES struct


### Send Data:
	TECHNIMAL_NB_NE866_COAP_RES Get(String path)
		○ Send simple GET request
		○ Return : CoAP response in TECHNIMAL_NB_NE866_COAP_RES struct
	TECHNIMAL_NB_NE866_COAP_RES Post(String path, String payload)
		○ Send simple POST request
		○ Return : CoAP response in TECHNIMAL_NB_NE866_COAP_RES struct
	TECHNIMAL_NB_NE866_COAP_RES PUT(String path, String payload)
		○ Send simple PUT request
		○ Return : CoAP response in TECHNIMAL_NB_NE866_COAP_RES struct
	TECHNIMAL_NB_NE866_COAP_RES DELETE(String path)
		○ Send simple DELETE request
		○ Return : CoAP response in TECHNIMAL_NB_NE866_COAP_RES struct
	TECHNIMAL_NB_NE866_COAP_RES SendCoAPPacket(String packet)
		○ Send custom packet and waiting for response
		○ Return : CoAP response in TECHNIMAL_NB_NE866_COAP_RES struct
	bool SendPacket(String packet)
		○ Send hex string packet
		○ Return : `True` if success, `False` if failed

### Receive Data: 
	TECHNIMAL_NB_NE866_RAW_RES WaitingResponse(long timeout)
		○ Waiting response from network
		○ Return : Response in TECHNIMAL_NB_NE866_RAW_RES struct
	TECHNIMAL_NB_NE866_MODULE_RES WaitingResponseFromModule(long timeout, String waiting_key)
		○ Waiting for module reponse
		○ Return : Response in TECHNIMAL_NB_NE866_MODULE_RES struct

## Device Compatibility:
### DEVIO NB-XBEE I
`DEVIO NB-XBEE I` is a communication board for XBEE form factor compatible pinout.

### Specifications:
  Communication Module: Telit NE866B1-E1
  Network Technology: LTE Cat. NB1 (NB-IoT)
  
  Frequency: B8 (900 MHz) /B20 (800MHz)
  
  Data Transmission:
  - Uplink 20 kbps (Single-tone)
  - Downlink 250 kbps 
  
  Protocol Stack:
  - UDP/IP
  
  Serial Communication (UART)
  - Control via AT commands according to 3GPP TS 27.005, 27.007 and customized AT commands.
  
  Dimensions: 
  - Width x Length x Height: 25x35x2 mm (Excluded antenna)
  
  NB-IoT Module Certification
  - RED (Europe), CCC (China), GCF.



# Getting Started
  1. Connect device to Dev Board
  2. Coding & Compile using Arduino Desktop IDE
		- Install the Arduino board drivers
		- Import Technimal NB-IoT Library 
		- Open Technimal NB-IoT Example and specify pin and your server IP & Port
		- Compile & Upload