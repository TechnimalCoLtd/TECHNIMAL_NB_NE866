/*
TECHNIMAL_NB_NE866 v0.1
Author: Technimal
Create Date: 23 August 2019
Modified: 23 August 2019

* Note : Not support option If-Match and ETag

Tested with AIS NB NE866
*/

#include "TECHNIMAL_NB_NE866.h"

#define DEBUG

TECHNIMAL_NB_NE866::TECHNIMAL_NB_NE866() {
	_Host = F("127.0.0.1");
	_Port = F("5683");
	_Token = F("");
	_MsgID = 1;
}

void TECHNIMAL_NB_NE866::SetupModule(Stream* serial, String host, String port) {
	Serial.println();
	Serial.println(F("############ TECHNIMAL_NB_NE866 Library ############"));

	_Serial = serial;
	_Host = host;
	_Port = port;

	RebootModule();
	setEchoOff();

	_Serial->println(F("AT+CMEE=2"));
	WaitingResponseFromModule(1000, F("OK"));

#ifdef DEBUG
	Serial.println(F("### Module Info ###"));
	
	Serial.print(F("# Firmware version :  "));
	Serial.println(GetFirmwareVersion());

	Serial.print(F("# IMEI :  "));
	Serial.println(GetIMEI());

	Serial.print(F("# IMSI SIM :  "));
	Serial.println(GetIMSI());

	Serial.print(F("# CCID :  "));
	Serial.println(GetCCID());
#endif
}

void TECHNIMAL_NB_NE866::RebootModule() {
	_Serial->println(F("AT"));
	WaitingResponseFromModule(1000, F("OK"));

#ifdef DEBUG
	Serial.print(F("# Reboot Module"));
#endif

	_Serial->println(F("AT#REBOOT"));

	WaitModuleReady();

#ifdef DEBUG
	Serial.println();
#endif

    _Serial->flush();
}

void TECHNIMAL_NB_NE866::WaitModuleReady() {
	unsigned char count = 0;
	while (count <= 1){
#ifdef DEBUG
		Serial.print(F("."));
#endif
		if (WaitingResponseFromModule(1000, F("OK")).status == 1) {
			count++;
		}
	}

#ifdef DEBUG
	Serial.println(F("OK"));
#endif
}

String TECHNIMAL_NB_NE866::GetFirmwareVersion(){
	_Serial->println(F("AT+CGMR"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(1000, F("OK"));
	if (res.status == 1)
    	res.data.replace(F("OK"),"");
	clearInputBuffer();
	return res.data;
}

String TECHNIMAL_NB_NE866::GetIMEI() {
	_Serial->println(F("AT+CGSN"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(1000, F("OK"));
	if (res.status == 1)
		res.data.replace(F("OK"),"");
	clearInputBuffer();
	delay(1500); // After read IMEI need to wait around 1.5 sec
	return res.data;
}

String TECHNIMAL_NB_NE866::GetIMSI() {
	_Serial->println(F("AT+CIMI"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(1000, F("OK"));
	if (res.status == 1)
		res.data.replace(F("OK"),"");
	clearInputBuffer();
	return res.data;
}

String TECHNIMAL_NB_NE866::GetCCID() {
	_Serial->println(F("AT+CCID"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(1000, F("OK"));
	if (res.status == 1) {
		res.data.replace(F("OK"),"");
		res.data.replace(F("+CCID: "), "");
	}
	clearInputBuffer();
	return res.data;
}

String TECHNIMAL_NB_NE866::GetDeviceIP() {
	_Serial->println(F("AT+CGPADDR=0"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(3000, F("+CGPADDR"));
	if (res.status == 1) {
		int index = res.data.indexOf(F(","));
		res.data = res.data.substring(index+1, res.data.length());
	}
	clearInputBuffer();
	return res.data;
}

bool TECHNIMAL_NB_NE866::Connect() {
#ifdef DEBUG
	Serial.print("# Geting signal");
#endif
	TECHNIMAL_NB_NE866_SIGNAL signal;
	while(1) {
		signal = getSignal(0);
		if(signal.csq.indexOf(F("99"))==-1) break;
#ifdef DEBUG
		Serial.print(".");
#endif
		delay(500);
	}
#ifdef DEBUG
		Serial.println("Got signal");
#endif


	if (!isConnected()) {
#ifdef DEBUG
		Serial.print(F("# Connecting NB-IoT Network."));
#endif
		for(int i=1;i<60;i+=1){
			setPhoneFunction(1);
			setAutoConnectOn();
			sgact(1);
			for (int j=0;j<5;j++) {
				delay(1000);
				if (isConnected()) {
#ifdef DEBUG
					Serial.println("Connected");
#endif
					delay(2000); //Waiting for ip
					return true;
				}
			}
#ifdef DEBUG
			Serial.print(F("."));
#endif
		}
#ifdef DEBUG
		Serial.println("Connect failed");
#endif
	} else {
		return true;
	}
	return false;
}

TECHNIMAL_NB_NE866_PING_RES TECHNIMAL_NB_NE866::Ping() {
#ifdef DEBUG
	Serial.print(F("# Ping to "));
	Serial.print(_Host);
#endif
	TECHNIMAL_NB_NE866_PING_RES ping_res;
	_Serial->print(F("AT#PING="));
	_Serial->println(_Host);
	TECHNIMAL_NB_NE866_MODULE_RES res;
	for (int i=0;i<60;i++) {
		res = WaitingResponseFromModule(1000, F("#PING: 04"));
		if (res.status == 1) break;
#ifdef DEBUG
		Serial.print(F("."));
#endif
	}
	if (res.status == 1) {
		int index = res.data.indexOf(F(","));
		int index2 = res.data.indexOf(F(","),index+1);
		int index3 = res.data.indexOf(F(","),index2+1);
		ping_res.status = true;
		ping_res.addr = res.data.substring(index+1,index2);
		ping_res.ttl = res.data.substring(index2+1,index3);

		index = res.data.indexOf(F("#PING: "), index3);
		if (index>0)
			ping_res.rtt = res.data.substring(index3+1, index);
		else
			ping_res.rtt = res.data.substring(index3+1, res.data.length());
		
#ifdef DEBUG
		Serial.println("...Successfully");
		Serial.print(F("# Pong IP: "));
		Serial.print(ping_res.addr);
		Serial.print(F(", ttl= "));
		Serial.print(ping_res.ttl);
		Serial.print(F(", rtt= "));
		Serial.println(ping_res.rtt);
#endif
	} else {
#ifdef DEBUG
		if (res.status == 2) 
			Serial.println(F("...Time out"));
		else
			Serial.println(F("...Failed"));
#endif
		ping_res.status = false;
	}

	return ping_res;
}

TECHNIMAL_NB_NE866_MODULE_RES TECHNIMAL_NB_NE866::WaitingResponseFromModule(long timeout, String waiting_key) {
	unsigned long start_time = millis();
	TECHNIMAL_NB_NE866_MODULE_RES res;
	res.status = 0;
	res.data = "";
	while(true) {
		if (_Serial->available()) {
			res.data += _Serial->readStringUntil('\n');
			if (res.data.indexOf(waiting_key)>=0) {
				res.status = 1;
				break;
			}
			if (res.data.indexOf(F("ERROR"))>=0) {
				res.status = 0;
				break;
			}
		}
		if (millis() - start_time >= timeout) {
			res.status = 2;
			break;
		}
	}
	return res;
}

// CoAP
// For CoAP protocol
// Format       | Version | Type | Token Length | Request Code |           Message ID           | Token |                                       Options                               | Start Payload | Payload |
// Bits Amount  |    2    |  2   |       4      |      8       |              16                | 0-64  |                                          ?                                  |      8        |    ?    |
// Translate    |   V.1   |  T.0 | token 0 bytes|     POST     |       Message id = 1           |   -   |                                      2 options                              |    Start      |         |
//              |         |      |              |              |                                |       |               Uri-Path: api              |          Uri-Path: v1            |               |         |
//              |         |      |              |              |       Hi      |      Low       |       | Delta | Length |          Data           | Delta | Length |      Data       |               |         |
// Example Bits |    01   |  00  |     0000     |   00000010   |    00000000   |    00000001    |   -   |  1011 |  0011  | 11000010111000001101001 |  0000 |  0010  | 111011000110001 |               |         |
// Example Hex  |        0x4     |     0x0      |     0x02     |      0x00     |      0x01      |   -   |  0xb  |  0x3   |       0x617069          |   0x0 |   0x2  |      0x7631     |     0xff      |    ?    |
void TECHNIMAL_NB_NE866::SetToken(String token) {
	if (token.length() > 8) {
		Serial.println("Maximum token length is 8 characters");
		return;
	}
	_Token = token;
}

TECHNIMAL_NB_NE866_COAP_RES TECHNIMAL_NB_NE866::Get(String path) {
	unsigned int msgID = _MsgID;
	String packet = GetCoAPHeader(GET);
	packet += PathToCoAPOptions(0, path);
	return SendCoAPPacket(packet);
}

TECHNIMAL_NB_NE866_COAP_RES TECHNIMAL_NB_NE866::Post(String path, String payload) {
	String packet = GetCoAPHeader(POST);
	packet += PathToCoAPOptions(0, path);
	packet += CreatePayload(payload);
	return SendCoAPPacket(packet);
}

TECHNIMAL_NB_NE866_COAP_RES TECHNIMAL_NB_NE866::Put(String path, String payload) {
	String packet = GetCoAPHeader(PUT);
	packet += PathToCoAPOptions(0, path);
	packet += CreatePayload(payload);
	return SendCoAPPacket(packet);
}

TECHNIMAL_NB_NE866_COAP_RES TECHNIMAL_NB_NE866::Delete(String path) {
	String packet = GetCoAPHeader(DELETE);
	packet += PathToCoAPOptions(0, path);
	return SendCoAPPacket(packet);
}

TECHNIMAL_NB_NE866_COAP_RES TECHNIMAL_NB_NE866::SendCoAPPacket(String packet) {
	unsigned long timeout[5]={2000,4000,8000,16000,32000};
	TECHNIMAL_NB_NE866_COAP_RES coapreq = ParseCoAPMessage(packet);
	for (int i=0;i<5;i++) {
		clearInputBuffer();
		for (int j=0;j<5;j++) {
			if (SendPacket(packet)) {
				TECHNIMAL_NB_NE866_RAW_RES res = WaitingResponse(timeout[i]);
				if (res.status == 1) {
					TECHNIMAL_NB_NE866_COAP_RES tmpresp = ParseCoAPMessage(res.data);
					if (coapreq.token == tmpresp.token && coapreq.msgID == tmpresp.msgID) {
						return tmpresp;
					}
				}
				break;
			}
			delay(200);
#ifdef DEBUG
			Serial.println(F("# Resend"));
#endif
		}
	}
	TECHNIMAL_NB_NE866_COAP_RES coapresp;
	return coapresp;
}

String TECHNIMAL_NB_NE866::GetCoAPHeader(unsigned char method) {
	String packet = "4"; //Version 1 and type 0
	packet += IntToHexStr(_Token.length());
	if (method == GET) {
		packet += "01";
	} else if (method == POST) {
		packet += "02";
	} else if (method == PUT) {
		packet += "03";
	} else if (method == DELETE) {
		packet += "04";
	}
	if (_MsgID < 256)
		packet += "00";
	packet += IntToHex2xStr(_MsgID++); //Message ID
	packet += StrToHexStr(_Token); //Token
	return packet;
}

String TECHNIMAL_NB_NE866::PathToCoAPOptions(int prevOptionCode, String path) {
	if (path.length() == 0) {
		return "";
	}

	int startIndex = 0;
	if (path.startsWith("/")) {
		startIndex = 1;
	}

	bool isend = false;
	int stopIndex = -1;
	stopIndex = path.indexOf("/", startIndex);
	if (stopIndex == -1) {
		stopIndex = path.length();
		isend = true;
	}

	TECHNIMAL_NB_NE866_COAP_OPTION option;
	option.code = URI_PATH;
	option.valueType = 0;
	option.valueStr = path.substring(startIndex, stopIndex);
	
	String out = CreateCoAPOption(prevOptionCode, option);
	if (!isend) {
		out += PathToCoAPOptions(option.code, path.substring(stopIndex + 1, path.length()));
	}
	return out;
}

String TECHNIMAL_NB_NE866::CreateCoAPOption(TECHNIMAL_NB_NE866_COAP_OPTION option) {
	return CreateCoAPOption(0, option);
}

String TECHNIMAL_NB_NE866::CreateCoAPOption(int prevOptionCode, TECHNIMAL_NB_NE866_COAP_OPTION option) {
	int optionCode = option.code - prevOptionCode;
	int len = 0;
	String value = "";
	if (option.valueType == 0) {
		value = StrToHexStr(option.valueStr);
		len = option.valueStr.length();
	} else if (option.valueType == 1) {
		value = IntToHex2xStr(option.valueUInt);
		len = value.length() / 2;
	}
	return IntToHexStr(optionCode) + IntToHexStr(len) + value;
}

String TECHNIMAL_NB_NE866::CreatePayload(String payload) {
	String res = F("ff");
	res += StrToHexStr(payload);
	return res;
}

TECHNIMAL_NB_NE866_COAP_RES TECHNIMAL_NB_NE866::ParseCoAPMessage(String data) {
#ifdef DEBUG
	Serial.println(F("========="));
	Serial.println(F("# Parse CoAP message"));
#endif
	TECHNIMAL_NB_NE866_COAP_RES res;

	// Type
	res.type = String(data[0]).toInt() - 4;
#ifdef DEBUG
	if (res.type == CONFIRMABLE) {
		Serial.println(F("# Type : CONFIRMABLE"));
	} else if (res.type == NON_CONFIRMABLE) {
		Serial.println(F("# Type : NON_CONFIRMABLE"));
	} else if (res.type == ACKNOWLEDGEMENT) {
		Serial.println(F("# Type : ACKNOWLEDGEMENT"));
	} else if (res.type == RESET) {
		Serial.println(F("# Type : RESET"));
	}
#endif

	// Status
	char statusbuf[2] = {data[2], data[3]};
	res.status = strtol(statusbuf, NULL, 16);

#ifdef DEBUG
	switch (res.status) {
		case EMPTY:
			Serial.println("# Status : EMPTY");
			break;
		case GET:
			Serial.println("# Status : GET");
			break;
		case POST:
			Serial.println("# Status : POST");
			break;
		case PUT:
			Serial.println("# Status : PUT");
			break;
		case DELETE:
			Serial.println("# Status : DELETE");
			break;
		case CREATED:
			Serial.println("# Status : CREATED");
			break;
		case DELETED:
			Serial.println("# Status : DELETED");
			break;
		case VALID:
			Serial.println("# Status : VALID");
			break;
		case CHANGED:
			Serial.println("# Status : CHANGED");
			break;
		case CONTENT:
			Serial.println("# Status : CONTENT");
			break;
		case CONTINUE:
			Serial.println("# Status : CONTINUE");
			break;
		case BAD_REQUEST:
			Serial.println("# Status : BAD_REQUEST");
			break;
		case FORBIDDEN:
			Serial.println("# Status : FORBIDDEN");
			break;
		case NOT_FOUND:
			Serial.println("# Status : NOT_FOUND");
			break;
		case METHOD_NOT_ALLOWED:
			Serial.println("# Status : METHOD_NOT_ALLOWED");
			break;
		case NOT_ACCEPTABLE:
			Serial.println("# Status : NOT_ACCEPTABLE");
			break;
		case REQUEST_ENTITY_INCOMPLETE:
			Serial.println("# Status : REQUEST_ENTITY_INCOMPLETE");
			break;
		case PRECONDITION_FAILED:
			Serial.println("# Status : PRECONDITION_FAILED");
			break;
		case REQUEST_ENTITY_TOO_LARGE:
			Serial.println("# Status : REQUEST_ENTITY_TOO_LARGE");
			break;
		case UNSUPPORTED_CONTENT_FORMAT:
			Serial.println("# Status : UNSUPPORTED_CONTENT_FORMAT");
			break;
		case INTERNAL_SERVER_ERROR:
			Serial.println("# Status : INTERNAL_SERVER_ERROR");
			break;
		case NOT_IMPLEMENTED:
			Serial.println("# Status : NOT_IMPLEMENTED");
			break;
		case BAD_GATEWAY:
			Serial.println("# Status : BAD_GATEWAY");
			break;
		case SERVICE_UNAVAILABLE:
			Serial.println("# Status : SERVICE_UNAVAILABLE");
			break;
		case GATEWAY_TIMEOUT:
			Serial.println("# Status : GATEWAY_TIMEOUT");
			break;
		case PROXY_NOT_SUPPORTED:
			Serial.println("# Status : PROXY_NOT_SUPPORTED");
			break;
		default:
			Serial.println("# Status : Unknow");
			break;
	}
#endif

	// Token
	int tokenLength = String(data[1]).toInt() * 2;
	res.token = HexStrToStr(data.substring(8, 8 + tokenLength));
#ifdef DEBUG
	Serial.print(F("# Token : "));
	Serial.println(res.token);
#endif

	// Message id
	char msgidbuf[4] = {data[4], data[5], data[6], data[7]};
	res.msgID = strtol(msgidbuf, NULL, 16);
#ifdef DEBUG
	Serial.print(F("# Message id : "));
	Serial.println(res.msgID);
#endif

	// Payload
	int payloadindex = data.indexOf(F("ff"));
	if (payloadindex == -1) {
		res.payload = "";
	} else {
		res.payload = HexStrToStr(data.substring(payloadindex + 2, data.length()));
	}
#ifdef DEBUG
	Serial.print(F("# Payload : "));
	Serial.println(res.payload);
	Serial.println(F("========="));
#endif

	return res;
}

String TECHNIMAL_NB_NE866::IntToHex2xStr(unsigned int intin) {
	String out = "";
	if (intin >= 256) {
		out = IntToHex2xStr(intin / 256);
		intin = intin % 256;
	}
	char buf[2];
	sprintf(buf, "%02x", intin);
	out += String(buf);
	return out;
}

String TECHNIMAL_NB_NE866::IntToHexStr(int intin) {
	int len = intin / 16;
	char buf[len+1];
	sprintf(buf, "%x", intin);
	return String(buf);
}

String TECHNIMAL_NB_NE866::StrToHexStr(String strin) {
	int lenuse = strin.length();
	char charBuf[lenuse*2-1];
	char strBuf[3];
	String strout = "";
	strin.toCharArray(charBuf,lenuse*2) ;
	for (int i = 0; i < lenuse; i++){
		sprintf(strBuf, "%02x", charBuf[i]);

		if (String(strBuf) != F("00") ){
			strout += strBuf;
		}
	}

	return strout;
}

String TECHNIMAL_NB_NE866::HexStrToStr(String hexstr) {
	String str = "";
	char buf[3];
	for (int i=0;i<hexstr.length();i+=2) {
		hexstr.toCharArray(buf, 3, i);
		str += (char) strtol(buf, NULL, 16);
	}
	return str;
}

bool TECHNIMAL_NB_NE866::SendPacket(String packet) {
	if (!createSocket()) return false;

#ifdef DEBUG
	Serial.println(F("\n========="));
	Serial.print(F("# Sending data to "));
	Serial.print(_Host);
	Serial.print(F(":"));
	Serial.print(_Port);
	Serial.println();
#endif

	// Send packet length to module
	_Serial->print(F("AT#SSENDEXT=1,"));
	_Serial->print(String(packet.length()/2));
	_Serial->write(13);
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(200, F(">"));
	if (res.status != 1) {
#ifdef DEBUG
		Serial.println("Failed to set packet length");
#endif
		return false;
	}

	// Send pakcet to module
	_Serial->print(packet);
	_Serial->write(13);
	_Serial->flush();
#ifdef DEBUG
	Serial.print(F("# Data : "));
	Serial.println(packet);
#endif

	res = WaitingResponseFromModule(6000, F("OK"));
#ifdef DEBUG
	if (res.status == 1) {
		Serial.println("# Successfully");
	} else {
		Serial.println("# Failed");
	}
	Serial.println(F("========="));
#endif
	return res.status == 1;
}

TECHNIMAL_NB_NE866_RAW_RES TECHNIMAL_NB_NE866::WaitingResponse(long timeout) {
	TECHNIMAL_NB_NE866_RAW_RES res;
	res.status = 0;

	String input = "";
	int countFoundEnd = 0;
	unsigned long startTime = millis();
	while (true) {
		if (_Serial->available()) {
			char data = (char)_Serial->read();
			if (data == '\n' || data == '\r') {
				countFoundEnd++;
				if (countFoundEnd > 3) break;
			} else {
				input += data;
				if (input.indexOf(F("SRING: ")) != -1) startTime = millis();
			}
		}
		if (millis() - startTime > timeout) {
#ifdef DEBUG
			Serial.println(F("# TIMEOUT"));
#endif
			res.status = 2;
			return res;
		}
	}

	int start = input.indexOf(F("SRING: "));
	int stop = input.indexOf(F(","), start);
	if (start > -1 && stop > -1) {
		// IP
		res.ip = input.substring(start+7, stop);

		// Port
		start = stop + 1;
		stop = input.indexOf(",", start);
		res.port = input.substring(start, stop);

		// Length
		start = input.indexOf(",", stop+1) + 1;
		stop = input.indexOf(",", start);
		res.length = input.substring(start, stop).toInt();

		// Data
		start = input.indexOf(",", stop + 1) + 1;
		stop = input.indexOf(F("\r"), start);
		res.data = input.substring(start, stop);
		res.data.toLowerCase();
		if (res.data.length() / 2 != res.length) {
			res.status = 0;
		} else {
			res.status = 1;
		}
#ifdef DEBUG
		Serial.println(F("\n========="));
		Serial.print(F("Receive data from "));
		Serial.print(res.ip);
		Serial.print(F(":"));
		Serial.println(res.port);
		Serial.print(F("Length : "));
		Serial.println(res.length);
		Serial.print(F("Data : "));
		Serial.println(res.data);
		Serial.println(F("========="));
#endif
	}
	return res;
}

// Private
bool TECHNIMAL_NB_NE866::createSocket() {
	_Serial->println(F("AT#SCFGEXT=1,3,1,0,0,1"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(3000, F("OK"));
	
	_Serial->print(F("AT#SD=1,1,"));
	_Serial->print(_Port);
	_Serial->print(F(",\""));
	_Serial->print(_Host);
	_Serial->println(F("\",0,5100,1"));

	res = WaitingResponseFromModule(3000, F("OK"));
	delay(300);
#ifdef DEBUG
	if (res.status == 1) {
		Serial.println(F("# Create socket successful"));
	} else if (!socketCreated) {
		Serial.println(F("# Create socket failed"));
	}
#endif
	if (res.status == 1) socketCreated = true;
	return socketCreated || res.status == 1;
}

void TECHNIMAL_NB_NE866::setEchoOff() {
	_Serial->println(F("ATE0"));
	WaitingResponseFromModule(500,F("OK"));
}

void TECHNIMAL_NB_NE866::clearInputBuffer() {
	while (_Serial->available()) {
		_Serial->read();
	}
}

TECHNIMAL_NB_NE866_SIGNAL TECHNIMAL_NB_NE866::getSignal(unsigned char state) {
	_Serial->println(F("AT+CSQ"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(500, F("+CSQ"));
	TECHNIMAL_NB_NE866_SIGNAL signal;
	if (res.status == 1) {
		int index = res.data.indexOf(F(":"));
		int index2 = res.data.indexOf(F(","));
		signal.csq = res.data.substring(index+1,index2);
		int x = signal.csq.toInt();
		x = (2*x)-113;
		signal.rssi = String(x);
		signal.ber  = res.data.substring(index2+1);
#ifdef DEBUG
		if (state == 1) {
			Serial.print(F("# Get CSQ Signal: csq= "));
			Serial.print(signal.csq);
			Serial.print(F(", rssi= "));
			Serial.print(signal.rssi);
			Serial.print(F(", ber= "));
			Serial.println(signal.ber);
		}
#endif
	}
	return signal;
}

bool TECHNIMAL_NB_NE866::isConnected(){
	_Serial->println(F("AT#SGACT?"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(500, F("#SGACT:"));
	if(res.status == 1 && res.data.indexOf(F("#SGACT: 0,1"))!=-1) {
		return true;
	}
	clearInputBuffer();
	return false;
}

bool TECHNIMAL_NB_NE866::setPhoneFunction(unsigned char mode) {
	_Serial->print(F("AT+CFUN="));
	_Serial->println(mode);
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(1000,F("OK"));
	clearInputBuffer();
	delay(2000); // // After set phone function need to wait around 2 sec
	return res.status == 1;
}

bool TECHNIMAL_NB_NE866::setAutoConnectOn() {
	_Serial->println(F("AT+NCONFIG=AUTOCONNECT,TRUE"));
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(1000,F("OK"));
	clearInputBuffer();
	return res.status == 1;
}

bool TECHNIMAL_NB_NE866::sgact(unsigned char mode) {
	_Serial->print(F("AT#SGACT=0,")); 
	_Serial->println(mode);
	TECHNIMAL_NB_NE866_MODULE_RES res = WaitingResponseFromModule(5000,F("OK"));
	clearInputBuffer();
	return res.status == 1;
}