/**
 * C library for the ESP8266 WiFi module with a PIC microcontroller
 * Copyright (C) 2015 Camil Staps <info@camilstaps.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *****************************************************************************
 *ORIGINALLY CREATED AS
 * File:    esp8266.h
 * Author:  Camil Staps <info@camilstaps.nl>
 * Website: http://github.com/camilstaps/ESP8266_PIC
 * Version: 0.1
 * 
 * MODFIED AND IMPROVED BY
 * File:    esp8266_functions.h
 * Author:  Aswinth Raj B  <mailtoaswinth@gmail.com>
 * Website: circuitdigest.com
 * Version: 0.1
 *
 * See:   circuitdigest.com for more explanation
 *
 * This is the header file for the ESP8266 PIC16F877A library where ESP runs on 115200 baudrate. 
 *
 */



#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define ESP8266_STATION 0x01
#define ESP8266_SOFTAP 0x02

#define ESP8266_TCP 1
#define ESP8266_UDP 0

#define ESP8266_OK 1
#define ESP8266_READY 2
#define ESP8266_FAIL 3
#define ESP8266_NOCHANGE 4
#define ESP8266_LINKED 5
#define ESP8266_UNLINK 6

int len;


///***______________________ESP module Function Declarations__________________**///
void _esp8266_putch(unsigned char);
unsigned char _esp8266_getch(void);
/** Function prototypes **/
bit esp8266_isStarted(void);        // Check if the module is started (AT)
bit esp8266_restart(void);          // Restart module (AT+RST)
void esp8266_echoCmds(bool);        // Enabled/disable command echoing (ATE)
// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char);
// Connect to AP (AT+CWJAP)
void esp8266_connect(unsigned char*, unsigned char*);
// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void);
// Local IP (AT+CIFSR)
void esp8266_ip(char*);
// Create connection (AT+CIPSTART)
bit esp8266_start(unsigned char protocol, char* ip, unsigned char port);
// Send data (AT+CIPSEND)
bit esp8266_send(unsigned char*);
// Receive data (+IPD)
void esp8266_receive(unsigned char*, uint16_t, bool);
/** Functions for internal use only **/
// Print a string to the output
void _esp8266_print(unsigned const char *);
// Wait for a certain string on the input
inline uint16_t _esp8266_waitFor(unsigned char *);
// Wait for any response on the input
inline unsigned char _esp8266_waitResponse(void);
void Lcd_Set_Cursor(char , char b);
void Lcd_Print_Char(char);
void Lcd_Print_String(char *);
void _esp8266_login_mail(unsigned char*, unsigned char*);
void _esp8266_mail_sendID(unsigned char*);
void _esp8266_mail_recID(unsigned char*);
void _esp8266_mail_subject(unsigned char*);
void _esp8266_mail_body(unsigned char*);
//********__________________End of Function Declaration_________________********///






//***Initialize UART for ESP8266**//
void Initialize_ESP8266(void)
{
    //****Setting I/O pins for UART****//
    TRISC6 = 0; // TX Pin set as output
    TRISC7 = 1; // RX Pin set as input
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required 
    baud rate and set BRGH for fast baud_rate**/
    SPBRG = 10;
    BRGH  = 1;  // for high baud_rate
    //_________End of baud_rate setting_________//
    
    //****Enable Asynchronous serial port*******//
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins
    //_____Asynchronous serial port enabled_______//
    //**Lets prepare for transmission & reception**//
    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception
    //__UART module up and ready for transmission and reception__//
    
    //**Select 8-bit mode**//  
    TX9   = 0;    // 8-bit reception selected
    RX9   = 0;    // 8-bit reception mode selected
    //__8-bit mode selected__//     
}
//________UART module Initialized__________//



 
//**Function to send one byte of date to UART**//
void _esp8266_putch(char bt)  
{
    while(!TXIF);  // hold the program till TX buffer is free
    TXREG = bt; //Load the transmitter buffer with the received value
}
//_____________End of function________________//


 
//**Function to get one byte of date from UART**//
char _esp8266_getch()   
{
    if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
    
    while(!RCIF);  // hold the program till RX buffer is free
    
    return RCREG; //receive the value and send it to main function
}
//_____________End of function________________//
 



//**Function to convert string to byte**//
void ESP8266_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        _esp8266_putch(*st_pt++); //process it as a byte data
}
//___________End of function______________//
//**End of modified Codes**//



//**Function to configure soft_AP**//
unsigned char esp8266_config_softAP(unsigned char* softssid, unsigned char* softpass) {
    _esp8266_print("AT+CWSAP=\"");
    _esp8266_print(softssid);
    _esp8266_print("\",\"");
    _esp8266_print(softpass);
    _esp8266_print("\",5,3\r\n");
    return _esp8266_waitResponse();
}
//___________End of function______________//




//**Function to stations IP/MAC**//
 void esp8266_get_stationIP()
    {
    char rex;
    ESP8266_send_string("AT+CWLIF\r\n");
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("IP:");
    do
    {
    rex = _esp8266_getch() ;
    Lcd_Print_Char(rex);
    }while(rex!=',');
    Lcd_Set_Cursor(2,1);
    Lcd_Print_String("MAC:");
    do
    {
    rex = _esp8266_getch() ;
    Lcd_Print_Char(rex);
    }while(rex!='O');
    }
 //___________End of function______________//
 

 
 //**Function to enable multiple connections**//
 void _esp8266_enale_MUX()
 {
 _esp8266_print("AT+CIPMUX=1\r\n"); //Enable Multiple Connections
 _esp8266_waitResponse();
 }
  //___________End of function______________//
 
 
  //**Function to create server on Port 80**//
 void _esp8266_create_server()
 {
    _esp8266_print("AT+CIPSERVER=1,80\r\n"); //Enable Server on port 80
    _esp8266_waitResponse(); 
 }
  //___________End of function______________//
 
 
 
 /*Enter into Start typing the mail*/
 void _esp8266_start_mail()
 {
    _esp8266_print("AT+CIPSEND=4,6\r\n");
    _esp8266_waitResponse();
    _esp8266_print("DATA\r\n");
    _esp8266_waitResponse();
 }
 /*Entered into the typing mode*/
 
 
 /*End the Mail using a "."*/
 void _esp8266_End_mail()
 {
    _esp8266_print("AT+CIPSEND=4,3\r\n");
    _esp8266_waitResponse();
    _esp8266_print(".\r\n");
    _esp8266_waitResponse();
 }
 /*End of mail*/
 
 /*Quit Connection from SMPT server*/
 void _esp8266_disconnect_SMPT2GO()
 {
    _esp8266_print("AT+CIPSEND=4,6\r\n");
    _esp8266_waitResponse();
    _esp8266_print("QUIT\r\n");
    _esp8266_waitResponse();
 }
 /*Disconnected*/
 
 
 

 /*Connect to SMPT2GO server*/
 void _esp8266_connect_SMPT2GO()
 {
    _esp8266_print("AT+CIPSTART=4,\"TCP\",\"mail.smtp2go.com\",2525\r\n");
    _esp8266_waitResponse();
    _esp8266_print("AT+CIPSEND=4,20\r\n");
    _esp8266_waitResponse();
    _esp8266_print("EHLO 192.168.1.123\r\n");
    _esp8266_waitResponse();
    _esp8266_print("AT+CIPSEND=4,12\r\n");
    _esp8266_waitResponse();
    _esp8266_print("AUTH LOGIN\r\n");
    _esp8266_waitResponse();
 }
    /*connected to Server*/
 
 /*LOG IN with your SMPT2GO approved mail ID*/
    /*Visit the page https://www.smtp2go.com/ and sign up using any Gmail ID
     * Once you gmail ID is SMPT2GO approved convert your mail ID and password in 64 base format
     * visit https://www.base64encode.org/ for converting 64 base format online
     * FORMAT -> _esp8266_login_mail("mailID in base 64","Password in base 64");
     * This program uses the ID-> aswinthcd@gmail.com and password -> circuitdigest as an example
     */
 void _esp8266_login_mail(unsigned char* mail_ID, unsigned char* mail_Pas) {
     len = strlen(mail_ID);
     len+= 2;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     _esp8266_print("AT+CIPSEND=4,");
     if ((l1+'0')>'0')
     _esp8266_putch(l1+'0');
     _esp8266_putch(l2+'0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    _esp8266_print(mail_ID);
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    len = strlen(mail_Pas);
    len+= 2;
    char l2 = len%10;
    char l1 = (len/10)%10;
     
    _esp8266_print("AT+CIPSEND=4,");
    if ((l1+'0')>'0')
    _esp8266_putch(l1+'0');
    _esp8266_putch(l2+'0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    
   _esp8266_print(mail_Pas);
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
}
 /*End of Login*/
 
 
 void _esp8266_mail_sendID(unsigned char* send_ID)
 {
     len = strlen(send_ID);
     len+= 14;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     _esp8266_print("AT+CIPSEND=4,");
     if ((l1+'0')>'0')
     _esp8266_putch(l1+'0');
     _esp8266_putch(l2+'0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    _esp8266_print("MAIL FROM:<");
    _esp8266_print(send_ID);
    _esp8266_print(">\r\n");
    _esp8266_waitResponse();   
 } 
 
 
  void _esp8266_mail_recID(unsigned char* rec_ID)
 {
     len = strlen(rec_ID);
     len+= 12;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     _esp8266_print("AT+CIPSEND=4,");
     if ((l1+'0')>'0')
     _esp8266_putch(l1+'0');
     _esp8266_putch(l2+'0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    _esp8266_print("RCPT To:<");
    _esp8266_print(rec_ID);
    _esp8266_print(">\r\n");
    _esp8266_waitResponse();   
 } 
  
    void _esp8266_mail_subject(unsigned char* subject)
 {
     len = strlen(subject);
     len+= 10;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     _esp8266_print("AT+CIPSEND=4,");
     if ((l1+'0')>'0')
     _esp8266_putch(l1+'0');
     _esp8266_putch(l2+'0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    _esp8266_print("Subject:");
    _esp8266_print(subject);
    _esp8266_print("\r\n");
    _esp8266_waitResponse();   
 } 
    
    
    
        void _esp8266_mail_body(unsigned char* body)
 {
     len = strlen(body);
     len+= 2;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     _esp8266_print("AT+CIPSEND=4,");
     if ((l1+'0')>'0')
     _esp8266_putch(l1+'0');
     _esp8266_putch(l2+'0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
    
    _esp8266_print(body);
    _esp8266_print("\r\n");
    _esp8266_waitResponse();   
 } 
 
 
 
 
 
 
 
 
 
 
/**
 * Check if the module is started
 *
 * This sends the `AT` command to the ESP and waits until it gets a response.
 *
 * @return true if the module is started, false if something went wrong
 */
bit esp8266_isStarted(void) {
    _esp8266_print("AT\r\n");
    return (_esp8266_waitResponse() == ESP8266_OK);
}

/**
 * Restart the module
 *
 * This sends the `AT+RST` command to the ESP and waits until there is a
 * response.
 *
 * @return true iff the module restarted properly
 */
bit esp8266_restart(void) {
    _esp8266_print("AT+RST\r\n");
    if (_esp8266_waitResponse() != ESP8266_OK) {
        return false;
    }
    return (_esp8266_waitResponse() == ESP8266_READY);
}

/**
 * Enable / disable command echoing.
 *
 * Enabling this is useful for debugging: one could sniff the TX line from the
 * ESP8266 with his computer and thus receive both commands and responses.
 *
 * This sends the ATE command to the ESP module.
 *
 * @param echo whether to enable command echoing or not
 */
void esp8266_echoCmds(bool echo) {
    _esp8266_print("ATE");
    if (echo) {
        _esp8266_putch('1');
    } else {
        _esp8266_putch('0');
    }
    _esp8266_print("\r\n");
    _esp8266_waitFor("OK");
}

/**
 * Set the WiFi mode.
 *
 * ESP8266_STATION : Station mode
 * ESP8266_SOFTAP : Access point mode
 *
 * This sends the AT+CWMODE command to the ESP module.
 *
 * @param mode an ORed bitmask of ESP8266_STATION and ESP8266_SOFTAP
 */
void esp8266_mode(unsigned char mode) {
    _esp8266_print("AT+CWMODE=");
    _esp8266_putch(mode + '0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
}

/**
 * Connect to an access point.
 *
 * This sends the AT+CWJAP command to the ESP module.
 *
 * @param ssid The SSID to connect to
 * @param pass The password of the network
 * @return an ESP status code, normally either ESP8266_OK or ESP8266_FAIL
 */
void esp8266_connect(unsigned char* ssid, unsigned char* pass) {
    _esp8266_print("AT+CWJAP=\"");
    _esp8266_print(ssid);
    _esp8266_print("\",\"");
    _esp8266_print(pass);
    _esp8266_print("\"\r\n");
    _esp8266_waitResponse();
}

/**
 * Disconnect from the access point.
 *
 * This sends the AT+CWQAP command to the ESP module.
 */
void esp8266_disconnect(void) {
    _esp8266_print("AT+CWQAP\r\n");
    _esp8266_waitFor("OK");
}

/**
 * Store the current local IPv4 address.
 *
 * This sends the AT+CIFSR command to the ESP module.
 *
 * The result will not be stored as a string but byte by byte. For example, for
 * the IP 192.168.0.1, the value of store_in will be: {0xc0, 0xa8, 0x00, 0x01}.
 *
 * @param store_in a pointer to an array of the type unsigned char[4]; this
 * array will be filled with the local IP.
 */
void esp8266_ip(unsigned char* store_in) {
    _esp8266_print("AT+CIFSR\r\n");
    unsigned char received;
    do {
        received = _esp8266_getch();
    } while (received < '0' || received > '9');
    for (unsigned char i = 0; i < 4; i++) {
        store_in[i] = 0;
        do {
            store_in[i] = 10 * store_in[i] + received - '0';
            received = _esp8266_getch();
        } while (received >= '0' && received <= '9');
        received = _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}

/**
 * Open a TCP or UDP connection.
 *
 * This sends the AT+CIPSTART command to the ESP module.
 *
 * @param protocol Either ESP8266_TCP or ESP8266_UDP
 * @param ip The IP or hostname to connect to; as a string
 * @param port The port to connect to
 *
 * @return true iff the connection is opened after this.
 */
bit esp8266_start(unsigned char protocol, char* ip, unsigned char port) {
    _esp8266_print("AT+CIPSTART=\"");
    if (protocol == ESP8266_TCP) {
        _esp8266_print("TCP");
    } else {
        _esp8266_print("UDP");
    }
    _esp8266_print("\",\"");
    _esp8266_print(ip);
    _esp8266_print("\",");
    unsigned char port_str[5] = "\0\0\0\0";
    sprintf(port_str, "%u", port);
    _esp8266_print(port_str);
    _esp8266_print("\r\n");
    if (_esp8266_waitResponse() != ESP8266_OK) {
        return 0;
    }
    if (_esp8266_waitResponse() != ESP8266_LINKED) {
        return 0;
    }
    return 1;
}

// Send data (AT+CIPSEND)
/**
 * Send data over a connection.
 *
 * This sends the AT+CIPSEND command to the ESP module.
 *
 * @param data The data to send
 *
 * @return true iff the data was sent correctly.
 */
bit esp8266_send(unsigned char* data) {
    unsigned char length_str[6] = "\0\0\0\0\0";
    sprintf(length_str, "%u", strlen(data));
    _esp8266_print("AT+CIPSEND=");
    _esp8266_print(length_str);
    _esp8266_print("\r\n");
    while (_esp8266_getch() != '>');
    _esp8266_print(data);
    if (_esp8266_waitResponse() == ESP8266_OK) {
        return 1;
    }
    return 0;
}

/**
 * Read a string of data that is sent to the ESP8266.
 *
 * This waits for a +IPD line from the module. If more bytes than the maximum
 * are received, the remaining bytes will be discarded.
 *
 * @param store_in a pointer to a character array to store the data in
 * @param max_length maximum amount of bytes to read in
 * @param discard_headers if set to true, we will skip until the first \r\n\r\n,
 * for HTTP this means skipping the headers.
 */
void esp8266_receive(unsigned char* store_in, uint16_t max_length, bool discard_headers) {
    _esp8266_waitFor("+IPD,");
    uint16_t length = 0;
    unsigned char received = _esp8266_getch();
    do {
        length = length * 10 + received - '0';
        received = _esp8266_getch();
    } while (received >= '0' && received <= '9');

    if (discard_headers) {
        length -= _esp8266_waitFor("\r\n\r\n");
    }

    if (length < max_length) {
        max_length = length;
    }

    /*sprintf(store_in, "%u,%u:%c%c", length, max_length, _esp8266_getch(), _esp8266_getch());
    return;*/

    uint16_t i;
    for (i = 0; i < max_length; i++) {
        store_in[i] = _esp8266_getch();
    }
    store_in[i] = 0;
    for (; i < length; i++) {
        _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}

/**
 * Output a string to the ESP module.
 *
 * This is a function for internal use only.
 *
 * @param ptr A pointer to the string to send.
 */
void _esp8266_print(unsigned const char *ptr) {
    while (*ptr != 0) {
        _esp8266_putch(*ptr++);
    }
}

/**
 * Wait until we found a string on the input.
 *
 * Careful: this will read everything until that string (even if it's never
 * found). You may lose important data.
 *
 * @param string
 *
 * @return the number of characters read
 */
inline uint16_t _esp8266_waitFor(unsigned char *string) {
    unsigned char so_far = 0;
    unsigned char received;
    uint16_t counter = 0;
    do {
        received = _esp8266_getch();
        counter++;
        if (received == string[so_far]) {
            so_far++;
        } else {
            so_far = 0;
        }
    } while (string[so_far] != 0);
    return counter;
}

/**
 * Wait until we received the ESP is done and sends its response.
 *
 * This is a function for internal use only.
 *
 * Currently the following responses are implemented:
 *  * OK
 *  * ready
 *  * FAIL
 *  * no change
 *  * Linked
 *  * Unlink
 *
 * Not implemented yet:
 *  * DNS fail (or something like that)
 *
 * @return a constant from esp8266.h describing the status response.
 */
inline unsigned char _esp8266_waitResponse(void) {
    unsigned char so_far[6] = {0,0,0,0,0,0};
    unsigned const char lengths[6] = {2,5,4,9,6,6};
    unsigned const char* strings[6] = {"OK", "ready", "FAIL", "no change", "Linked", "Unlink"};
    unsigned const char responses[6] = {ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE, ESP8266_LINKED, ESP8266_UNLINK};
    unsigned char received;
    unsigned char response;
    bool continue_loop = true;
    while (continue_loop) {
        received = _esp8266_getch();
        for (unsigned char i = 0; i < 6; i++) {
            if (strings[i][so_far[i]] == received) {
                so_far[i]++;
                if (so_far[i] == lengths[i]) {
                    response = responses[i];
                    continue_loop = false;
                }
            } else {
                so_far[i] = 0;
            }
        }
    }
    return response;
}