#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
//#include "wyswietlacze.h" //mój plik nagłówkowy
uint8_t cyfry[11]  = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F, 0x6F,0x79};
volatile uint8_t wysw; // zmienna oznaczająca numer aktywnego wyświetlacza
uint8_t liczba = 0;
uint8_t liczba1 = 0;
uint8_t kropka=0;
volatile uint8_t tysiace;
volatile uint8_t setki;
volatile uint8_t dziesiatki;
volatile uint8_t jednosci;

int main(void)
{

  DDRA = 0xff; //cały port jako wyjściowy
  DDRB =0xff; //porty PD0 i PD1 również jako wyjścia

  TCCR1B |= (1 << WGM12); // Ustawia timer1 w tryb CTC
  OCR1A = 40000; // Ustawia wartość pożądaną na 100Hz dla preskalera 1
  TCCR1B |= (1 << CS10); // Ustawia timer z preskalerem Fcpu/1
  TIMSK |= (1 << OCIE1A); // Zezwolenie na przerwania dla CTC
  sei(); // Zezwolenie globalne na przerwania

  while(1)
    {
      tysiace=cyfry[(liczba/1000)%10];
      setki=cyfry[(liczba/100)%10];
      dziesiatki = (cyfry[liczba]/10)%10;
      jednosci = cyfry[liczba]%10;
      _delay_ms(100); // przerwa między liczeniem
      if (liczba < 1000) {liczba ++;} else { kropka=1;}
    }

  return 0;
}

ISR(TIMER1_COMPA_vect)
{
  liczba1++;
  if(liczba1>=99){liczba++,liczba1=0}
  switch (wysw)
    {
    case 0:
      PORTB=0111;
      PORTA = cyfry[tysiace];}
  wysw++;
  break;

 case 1:
   PORTB=1011;
   PORTA = cyfry[setki];
   wysw++;
   break;
       
 case 2:
   PORTB=1101
     PORTA = cyfry[dziesiatki];
   if(kropka==1){
   
     PORTA=PORTA & (7<<1)
       }
       
       
}
wysw++;
break;
       
 case 3:
PORTB=1110
  PORTA=cyfry[jednosci]
  wysw=0;
break;       
}
}
