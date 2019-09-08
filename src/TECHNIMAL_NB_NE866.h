#ifndef TECHNIMAL_NB_NE866_h
#define TECHNIMAL_NB_NE866_h

#include <Arduino.h>
#include <Stream.h>
#include <Wire.h>

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

enum TECHNIMAL_NB_NE866_COAP_OPTION_CODE {
	IF_MATCH=1,
	URI_HOST=3,
	ETAG=4,
	IF_NONE_MATCH=5,
	URI_PORT=7,
	LOCATION_PATH=8,
	URI_PATH=11,
	CONTENT_FORMAT=12,
	MAX_AGE=14,
	URI_QUERY=15,
	ACCEPT=17,
	LOCATION_QUERY=20,
	PROXY_URI=35,
	PROXY_SCHEME=39,
	SIZE1=60
};

enum TECHNIMAL_NB_NE866_COAP_TYPE {
	CONFIRMABLE=0,
	NON_CONFIRMABLE=1,
	ACKNOWLEDGEMENT=2,
	RESET=3
};

enum TECHNIMAL_NB_NE866_COAP_STATUS {
	EMPTY=00,
	GET=01,
	POST=02,
	PUT=03,
	DELETE=04,
	CREATED=65,
	DELETED=66,
	VALID=67,
	CHANGED=68,
	CONTENT=69,
	CONTINUE=95,
	BAD_REQUEST=128,
	FORBIDDEN=131,
	NOT_FOUND=132,
	METHOD_NOT_ALLOWED=133,
	NOT_ACCEPTABLE=134,
	REQUEST_ENTITY_INCOMPLETE=136,
	PRECONDITION_FAILED=140,
	REQUEST_ENTITY_TOO_LARGE=141,
	UNSUPPORTED_CONTENT_FORMAT=143,
	INTERNAL_SERVER_ERROR=160,
	NOT_IMPLEMENTED=161,
	BAD_GATEWAY=162,
	SERVICE_UNAVAILABLE=163,
	GATEWAY_TIMEOUT=164,
	PROXY_NOT_SUPPORTED=165
};

class TECHNIMAL_NB_NE866{
public:
	TECHNIMAL_NB_NE866();

	// Initial
	void SetupModule(Stream* serial, String host, String port);
	void RebootModule();
	bool WaitModuleReady();

	String GetIMEI();
	String GetIMSI();
	String GetCCID();
	String GetDeviceIP();
	String GetFirmwareVersion();

	bool Connect();

	TECHNIMAL_NB_NE866_PING_RES Ping();

	// CoAP
	void SetToken(String token);
	TECHNIMAL_NB_NE866_COAP_RES Get(String path);
	TECHNIMAL_NB_NE866_COAP_RES Post(String path, String payload);
	TECHNIMAL_NB_NE866_COAP_RES Put(String path, String payload);
	TECHNIMAL_NB_NE866_COAP_RES Delete(String path);
	TECHNIMAL_NB_NE866_COAP_RES SendCoAPPacket(String packet);
	String GetCoAPHeader(unsigned char method);
	String PathToCoAPOptions(int prevOptionCode, String path);
	String CreateCoAPOption(TECHNIMAL_NB_NE866_COAP_OPTION option);
	String CreateCoAPOption(int prevOptionCode, TECHNIMAL_NB_NE866_COAP_OPTION option);
	String CreatePayload(String payload);
	String IntToHex2xStr(unsigned int intin);
	String IntToHexStr(int intin);
	String StrToHexStr(String strin);
	String HexStrToStr(String hexstr);
	TECHNIMAL_NB_NE866_COAP_RES ParseCoAPMessage(String data);

	// General
	bool SendPacket(String packet);
	TECHNIMAL_NB_NE866_RAW_RES WaitingResponse(long timeout);
	TECHNIMAL_NB_NE866_MODULE_RES WaitingResponseFromModule(long timeout, String key);
	TECHNIMAL_NB_NE866_MODULE_RES WaitingResponseFromModule(long timeout, String first_key, String second_key);
	

private:
	TECHNIMAL_NB_NE866_MODULE_RES sendMsgChunk();
	bool createSocket();
	bool closeSocket();
	void setEchoOff();
	void clearInputBuffer();
	TECHNIMAL_NB_NE866_SIGNAL getSignal(unsigned char state);
	bool isConnected();
	bool setPhoneFunction(unsigned char mode);
	bool setAutoConnectOn();
	bool sgact(unsigned char mode);

	String _Host;
	String _Port;
	String _Token;
	String _imei;
	String _imsi;
	String _ccid;
	String _buffer;

	unsigned int _MsgID;

	bool socketCreated = false;
protected:
	Stream *_Serial;
};

#endif