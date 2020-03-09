// data format data:- temp, humidty,adc0air, adc1sound
// pb0 r1 fan, pb1 r2 airalarm
// pd3 for dh11 data
// a0 air sensor, a1 sound sensor

#define F_CPU 1000000UL  // clock speed

//#define BAUD 9600
//#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#define DHT11_PIN 3  //pd3 for dht11 data

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "lcd.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define NOSEARIAL


char str[4];  // convert int to str
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;  // data collection variable for dht 11

void Request()				/* Microcontroller send start pulse/request */
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);			/* wait for 20ms */
	PORTD |= (1<<DHT11_PIN);	/* set to high pin */
}

void Response()				/* receive response from DHT11 */
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()			/* receive data */
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);	/* then its logic HIGH */
		else			/* otherwise its logic LOW */
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}


// adc funfion ***************************************

void InitADC()
{
	ADMUX=(1<<REFS0);									// For Aref=AVcc;
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	// Prescalar div factor = 128
}

uint16_t ReadADC(uint8_t ch)
{
	//Select ADC Channel ch must be 0-7
	ch=ch&0b00000111;
	ADMUX&=0b11100000;
	ADMUX|=ch;

	//Start Single conversion
	ADCSRA|=(1<<ADSC);

	//Wait for conversion to complete
	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA|=(1<<ADIF);

	return(ADC);
}

//end adc funtion ************************************


void convertstr(int a){    // converts values to string format
	unsigned int b;
	//b=((float)a/1023)*100;
	itoa(a,str,10);
}




void USARTInit(uint16_t ubrr_value)  // enabling serial communication
{
	//Set Baud rate
	UBRRL = ubrr_value;
	UBRRH = (ubrr_value>>8);
	UCSRA = (1<<U2X); // its double
	UCSRC=(1<<URSEL)|(3<<UCSZ0);
	//Enable The receiver and transmitter
	UCSRB=(1<<RXEN)|(1<<TXEN);
}

char BtReadChar(){    // read data from serial
	while(!(UCSRA & (1<<RXC)))
	{
	}
	return UDR;
}


void BtWriteChar(char data){    //write character to serial
	while(!(UCSRA & (1<<UDRE)))
	{
	}
	UDR=data;
}

void BtString(char *str){     // write string to serial
	int i;
	for(i=0; ; i++){
		if(str[i]=='\0')
		break;
		BtWriteChar(str[i]);
	}
}


int main(void){


	//enabling display ports
	DDRD |= (1<<4);
	DDRD |= (1<<5);
	DDRD |= (1<<6);
	DDRD |= (1<<7);	
	DDRC = 0xFF;
	

	DDRB = 0xFF;  //relay output
	PORTB = 0xFF;  //  pull up for button
	
	
	Lcd4_Init();
	InitADC();
	USARTInit(12); //12 double 51
	
	
	
	

	while(1){
		char datastr[30] = "data:-";
		int adcval;
		int airval;
		
		Request();		/* send start pulse */
		Response();		/* receive response */
		I_RH=Receive_data();	/* store first eight bit in I_RH */    // interger part
		D_RH=Receive_data();	/* store next eight bit in D_RH */     // decimal part
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */   // like parity bit (check errors)
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			Lcd4_Clear();
			Lcd4_Write_String("Error Dh11");
		}
		
		else
		{
			Lcd4_Clear();
			convertstr(I_Temp);
			Lcd4_Write_String("temp:- ");
			Lcd4_Write_String(str);
			
			#ifndef NOSEARIAL 
			strcat(datastr, str);
			strcat(datastr, ".");
			#endif
			
			convertstr(D_Temp);
			Lcd4_Write_String(".");
			Lcd4_Write_String(str);
			
			#ifndef NOSEARIAL
			strcat(datastr, str);
			strcat(datastr, ",");
			#endif
			
		
			
			Lcd4_Set_Cursor(2,0);
			convertstr(I_RH);
			Lcd4_Write_String("Hum:- ");
			Lcd4_Write_String(str);
			
			#ifndef NOSEARIAL
			strcat(datastr, str);
			strcat(datastr, ",");
			#endif
			
			
			
		}
		
		
		
		
		
		Lcd4_Write_String(" A6=");
		airval = ReadADC(0);
		convertstr(airval);
		Lcd4_Write_String(str);
		
		#ifndef NOSEARIAL
		strcat(datastr, str);
		strcat(datastr, ",");
		#endif
		
		adcval = ReadADC(1);
		convertstr(adcval);
		
		#ifndef NOSEARIAL
		strcat(datastr, str);
		strcat(datastr, ",");
		#endif
		
		
		
		_delay_ms(1000);
		Lcd4_Clear();
		
		
		//BtString(" ishara thilina\n");
		BtString(datastr);
		
		if(I_Temp>35){
			PORTB &= ~(1<<0);  // on fan relay	
		}else{
			PORTB |= (1<<0);  // off fan relay		
		}
		
		
		
		if(airval>200){
			PORTB &= ~(1<<1);  // on fan relay
			}else{
			PORTB |= (1<<1);  // off fan relay
		}
		
		
		
		
	}
}


//  itoa(val,str,10);
//	Lcd4_Set_Cursor(1,1);
//	Lcd4_Write_Char('5');
//  Lcd4_Clear();
//	_delay_ms(2000);
//ch = BtReadChar();
// Lcd4_Write_String("Adc0 is = ");

//Lcd4_Write_Char(ch);
//send(ch);