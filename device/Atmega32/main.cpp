#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define F_CPU 1000000UL 
#define DHT11_PIN 3 

void Request()
{
 DDRD |= (1<<DHT11_PIN); 
 PORTD &= ~(1<<DHT11_PIN);
 _delay_ms(20);   
 PORTD |= (1<<DHT11_PIN);
}

void Response()  
{
 DDRD &= ~(1<<DHT11_PIN);
 while(PIND & (1<<DHT11_PIN));
 while((PIND & (1<<DHT11_PIN))==0);
 while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()  
{
 for (int q=0; q<8; q++)
 {
  while((PIND & (1<<DHT11_PIN)) == 0);

  _delay_us(30);

  if(PIND & (1<<DHT11_PIN)){ 
   c = (c<<1)|(0x01);
  }
  else{
   c = (c<<1);
  }

  while(PIND & (1<<DHT11_PIN));
 }

 return c;
}


void InitADC()
{
 ADMUX=(1<<REFS0);         
 ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); 
}

uint16_t ReadADC(uint8_t ch)
{
 
 ch=ch&0b00000111;
 ADMUX&=0b11100000; 
 ADMUX|=ch;
 ADCSRA|=(1<<ADSC);

 while(!(ADCSRA & (1<<ADIF))); 

 ADCSRA|=(1<<ADIF);  

 return(ADC);
}

void convertstr(int a){ 
 itoa(a,str,10);
}

void USARTInit(uint16_t ubrr_value)
{
 UBRRL = ubrr_value;                 
 UBRRH = (ubrr_value>>8);
 UCSRA = (1<<U2X); 
 UCSRC=(1<<URSEL)|(3<<UCSZ0);
 UCSRB=(1<<RXEN)|(1<<TXEN);
}

char BtReadChar(){  
 while(!(UCSRA & (1<<RXC)));
 return UDR;
}


void BtWriteChar(char data){   
 while(!(UCSRA & (1<<UDRE)));
 UDR=data;
}

void BtString(char *str){  
 int i;
 for(i=0; ; i++){
  if(str[i]=='\0')
   break;
  BtWriteChar(str[i]);
 }
}

char str[4];
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

int main(void){

 DDRB = 0xFF; 
 PORTB = 0xFF; 
 
 InitADC();
 USARTInit(12); 

 while(1){
  char datastr[30] = "data:-";
  int light;
  int soil;
  
  Request();  
  Response(); 
  I_RH=Receive_data();
  D_RH=Receive_data();
  I_Temp=Receive_data(); 
  D_Temp=Receive_data(); 
  CheckSum=Receive_data();
  
  if ((I_RH + D_RH + I_Temp + D_Temp) == CheckSum)
  {
   convertstr(I_Temp);
   strcat(datastr, str);
   strcat(datastr, ".");

   convertstr(D_Temp);
   strcat(datastr, str);
   strcat(datastr, ",");

   convertstr(I_RH);
   strcat(datastr, str);
   strcat(datastr, ",");
  }

  soil = ReadADC(0);
  convertstr(soil);

  strcat(datastr, str);
  strcat(datastr, ",");
  
  light = ReadADC(1);
  convertstr(light);
  
  strcat(datastr, str);
  strcat(datastr, ",");
  
  BtString(datastr);
  
  if(I_Temp<=15){
   PORTB &= ~(1<<0); 
  }else{
   PORTB |= (1<<0);  
  }
  
  if(I_RH<90){
   PORTB &= ~(1<<1);
  }else{
   PORTB |= (1<<1);  
  }

  if(soil<300){
	PORTB &= ~(1<<2);  
  }else{
	PORTB |= (1<<2); 
  }

  _delay_ms(1000);
   
 }
}