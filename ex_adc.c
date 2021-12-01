#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#define BAUD 1200
#define MYUBRR F_CPU/16/BAUD-1

void ADC_Init(){
  DDRA = 0x00;
  ADMUX  |= 0<<REFS1 | 1<<REFS0;
  ADCSRA |= 1<<ADEN | 1<<ADSC | 3 << ADPS0;
  //ADEN - ADC Enable, ADSC - Start, ADPS2:0 - Prescalar 111=128
  while (!(ADCSRA & 1<<ADIF));
}

int ADC_Read(char channel){
  ADMUX |= channel;
  ADCSRA |= 1<<ADSC;
  while ((ADCSRA&(1<<ADIF)) == 0);
  _delay_ms(10);
  return (ADCL|ADCH<<8);
}

void UART_init(uint16_t ubrr){
  UBRRL = (uint8_t)(ubrr);
  UBRRH = (uint8_t)(ubrr>>8);
  UCSRB = (1<<RXEN) | (1<<TXEN);
  UCSRC = (1<<URSEL) | (3<<UCSZ0);
}

unsigned char UART_RxChar(){
  while ((UCSRA & (1<<RXC)) == 0);
  return (UDR);
}

void UART_TxChar(char ch){
  while (!(UCSRA & (1<<UDRE)));
  UDR = ch;
}

void UART_SendString(char* str){
  unsigned char j = 0;
  while (str[j] != 0){
    UART_TxChar(str[j]);
    ++j;
  }
}

int main(void){

  uint8_t stop = 0;
  int value;
  float v;
  char str[10];

  ADC_Init();
  UART_init(MYUBRR);

  while (stop == 0){
    if(PIND & (1<<PIN7)){
      UART_SendString("!q!\n");
      stop = (stop) ? 0 : 1;
    }else{
      value = ADC_Read(3);
      v = (value * 5.0)/1024;
      dtostrf(v, 4, 2, str);
      UART_SendString(str);
      UART_TxChar('\n');
      _delay_ms(500);
    }
  }
  return 0;
}
