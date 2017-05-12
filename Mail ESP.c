#define _XTAL_FREQ 20000000

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#include "esp8266_functions.h"

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)




//****LCD Functions Developed by Circuit Digest.***///
void Lcd_SetBit(char data_bit) //Based on the Hex value Set the Bits of the Data Lines
{
	if(data_bit& 1) 
		D4 = 1;
	else
		D4 = 0;

	if(data_bit& 2)
		D5 = 1;
	else
		D5 = 0;

	if(data_bit& 4)
		D6 = 1;
	else
		D6 = 0;

	if(data_bit& 8) 
		D7 = 1;
	else
		D7 = 0;
}

void Lcd_Cmd(char a)
{
	RS = 0;           
	Lcd_SetBit(a); //Incoming Hex value
	EN  = 1;         
        __delay_ms(4);
        EN  = 0;         
}

Lcd_Clear()
{
	Lcd_Cmd(0); //Clear the LCD
	Lcd_Cmd(1); //Move the curser to first position
}

void Lcd_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a== 1)
	{
	  temp = 0x80 + b - 1; //80H is used to move the curser
		z = temp>>4; //Lower 8-bits
		y = temp & 0x0F; //Upper 8-bits
		Lcd_Cmd(z); //Set Row
		Lcd_Cmd(y); //Set Column
	}
	else if(a== 2)
	{
		temp = 0xC0 + b - 1;
		z = temp>>4; //Lower 8-bits
		y = temp & 0x0F; //Upper 8-bits
		Lcd_Cmd(z); //Set Row
		Lcd_Cmd(y); //Set Column
	}
}

void Lcd_Start()
{
  Lcd_SetBit(0x00);
  for(int i=1065244; i<=0; i--)  NOP();  
  Lcd_Cmd(0x03);
	__delay_ms(5);
  Lcd_Cmd(0x03);
	__delay_ms(11);
  Lcd_Cmd(0x03); 
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x08); //Select Row 1
  Lcd_Cmd(0x00); //Clear Row 1 Display
  Lcd_Cmd(0x0C); //Select Row 2
  Lcd_Cmd(0x00); //Clear Row 2 Display
  Lcd_Cmd(0x06);
}

void Lcd_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
   char Lower_Nibble,Upper_Nibble;
   Lower_Nibble = data&0x0F;
   Upper_Nibble = data&0xF0;
   RS = 1;             // => RS = 1
   Lcd_SetBit(Upper_Nibble>>4);             //Send upper half by shifting by 4
   EN = 1;
   for(int i=2130483; i<=0; i--)  NOP(); 
   EN = 0;
   Lcd_SetBit(Lower_Nibble); //Send Lower half
   EN = 1;
   for(int i=2130483; i<=0; i--)  NOP();
   EN = 0;
}

void Lcd_Print_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Print_Char(a[i]);  //Split the string using pointers and call the Char function 
}
//***End of LCD functions***//




void main()
{
    TRISD = 0x00;
    Lcd_Start();
    Initialize_ESP8266() ; 
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("Circuit Digest");
    Lcd_Set_Cursor(2,1);
    Lcd_Print_String("Mail using ESP");
    __delay_ms(1500);
    Lcd_Clear();
    
    
   
    /*Check if the ESP_PIC communication is successful*/
    do
    {
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("ESP not found");
    }while (!esp8266_isStarted()); //wait till the ESP send back "OK"
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("ESP is connected");
    __delay_ms(1500);
    Lcd_Clear();
    /*Yes ESP communication successful -Proceed*/
    
    
    /*Put the module in AP+STA*/
    esp8266_mode(3);
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("ESP set AP+STA");
    __delay_ms(1500);
    Lcd_Clear();
    /*Module set as AP+STA*/
    
    
    /*Connect to a AccesPoint*/
    esp8266_connect("BPAS home","cracksun"); //Enter you WiFi name and password here, here BPAS home is the name and cracksun is the pas
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("Connected 2 WIFI"); //Print on LCD for debugging. 
    __delay_ms(1500);
    Lcd_Clear();
    /*Connected to WiFi*/
    
    
    
    _esp8266_enale_MUX(); //Enable multiple connections
    _esp8266_create_server(); //Create a server on port 80
    _esp8266_connect_SMPT2GO(); //Establish TCP connection with SMPT2GO
    
    
    /*LOG IN with your SMPT2GO approved mail ID*/
    /*Visit the page https://www.smtp2go.com/ and sign up using any Gmail ID
     * Once you gmail ID is SMPT2GO approved convert your mail ID and password in 64 base format
     * visit https://www.base64encode.org/ for converting 64 base format online
     * FORMAT -> _esp8266_login_mail("mailID in base 64","Password in base 64");
     * This program uses the ID-> aswinthcd@gmail.com and password -> circuitdigest as an example
     */
    _esp8266_login_mail("YXN3aW50aGNkQGdtYWlsLmNvbQ==","Y2lyY3VpdGRpZ2VzdA==");
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("Login Successful"); //display on LCD for debugging
    __delay_ms(1500);
    Lcd_Clear();
    /*End of Login*/
    
    
    _esp8266_mail_sendID("aswinthcd@gmail.com"); //The sender mail ID
    _esp8266_mail_recID("mailtoaswinth@gmail.com"); //The Receiver mail ID
    
    _esp8266_start_mail();
    _esp8266_mail_subject("Mail from ESP8266"); //Enter the subject of your mail
    _esp8266_mail_body("Testing Success -CircuitDigest"); //Enter the body of your mail       
    _esp8266_End_mail();
    
    _esp8266_disconnect_SMPT2GO();
    
    
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("Mail Sent"); //Print on LCD for debugging
 
            
    while(1)
    {
        //do nothing 
    }

}