//Karolina Bozek, Michal Kimak, Bartosz Lasota
#define F_CPU 1000000UL //czestotliwosc taktowania AVR

#include <avr/io.h>
#include <avr/interrupt.h>  //obsluga przerwan
#include "scheduler.h"


struct Task //deklaracja struktury opisujacej wywolywane zadanie
{
  void (*func_task) (void *params); //wskaznik do funkcji wywolywanej przez scheduler
  void	*params;
  unsigned char priority;  //priorytet zadania
  unsigned int period; //co ile ms wywolywane jest zadanie
  unsigned int counter; //licznik do odmierzania czasu wywyolania zadania
  unsigned char active; //flaga czy ma byc wykonane zadanie, ustawiana gdy czy uplynal czas period
};

struct Task tab_task [MAX_NUMBER_OF_TASKS]; //deklaracja tablicy schedulera z zadaniami
unsigned char num_task; //ile zadan jest wykonywanych

//prototypy funkcji
void schedule (void);
void init_timer_irq(void);
void AddTask(unsigned char priority, unsigned int period, void *func_ptr, void *params);
void init_scheduler(void);
void KeyboardTask(void *params);
void SerialReceiveTask(void *params);
void WatchdogTask(void *params);

//funkcje
//przerwanie TIMER0 OVF, przepelnienie
ISR(TIMER0_OVF_vect) {
  
  TCNT0 = TIMER0_PERIOD_1MS; //poczatkowa wartosc licznika T0, tak aby bylo przepelnienie co 1ms
  schedule(); //wywolanie
}

//funkcja schedule, odpowiedzialna za oznaczanie zadan do wykonania
void schedule (void) {
  
  unsigned char i;
  
  for (i=0; i < MAX_NUMBER_OF_TASKS; i++) {  //przegladanie tablicy z zadaniami
    if(tab_task[i].func_task != 0) //czy jest przypisane zadanie do priorytetu o numerze i
      tab_task[i].counter++;  //inkrementacja licznika czasu
  }
  
  for (i=0; i < MAX_NUMBER_OF_TASKS; i++) {  //przegladanie tablicy z zadaniami
    if(tab_task[i].func_task != NULL) //czy jest przypisane zadanie do priorytetu o numerze i jesli tak to
      if (tab_task[i].counter >=  tab_task[i].period) { //czy uplynal czas do wykonania zadania
	tab_task[i].active = 1; //ustawienie flagi, ze zadanie ma byc wykonane
	tab_task[i].counter = 0; //zerowanie licznika czasu zadania
      }
  }
}

//funkcja execute, ciagle przeszukiwanie zadan gotowych do wykonania i ich wykonywanie
void execute(void) {
  
  unsigned char i=0;
  
  while (1) { //nieskonczona petla
    
    if(tab_task[i].func_task != NULL) //czy jest przypisane zadanie do priorytetu o numerze i jesli tak to:
      if(tab_task[i].active == 1) { //czy zadanie ma byc wykonane
	tab_task[i].func_task(tab_task[i].params);  //wywolanie aktywnego zadania z przekazaniem parametrow params do zadania
	tab_task[i].active  = 0; //zerowanie flagi aktywnosci zadania
      }
    
    i++; //inkrementacja wskaznika na kolejne zadanie 
    
    if (i == MAX_NUMBER_OF_TASKS) //czy sprawdzone wszystkie zadania
      i = 0; //jesli tak to od poczatku sprawdzanie, i=0
  }
}

//funkcja dodajaca zadanie do tablicy zadanie
void AddTask(unsigned char priority, unsigned int period, void *func_ptr, void *params) {
  
  tab_task[priority].func_task = func_ptr;
  tab_task[priority].params = params;
  tab_task[priority].priority = priority;
  tab_task[priority].period = period;
  tab_task[priority].counter = 0;
  tab_task[priority].active = 0;
  
}

//inicjalizacja tablicy schedulera
void init_scheduler(void) {
  
  unsigned char i;
  
  for (i=0; i < MAX_NUMBER_OF_TASKS; i++) {  //czyszczenie tablicy schedulera
    tab_task[i].func_task = NULL;
    tab_task[i].params = NULL;
    tab_task[i].priority = 0;
    tab_task[i].period = 0;
    tab_task[i].counter = 0;
    tab_task[i].active = 0;
    
  }
}

//incjalizacja pracy licznika T0
void init_timer_irq(void) {
  
  TCNT0 = TIMER0_PERIOD_1MS; //poczatkowa wartosc licznika T0, tak by bylo przepelnienie co 1ms
  TIMSK = (1 << TOIE0);   // zezwolenie na przerwanie timer0 ovf
  TCCR0 = (1 << CS00) | (1 << CS01); //wlaczenie licznika T0, preskaler /64
  
	sei();  //globalne przerwanie wlaczone
}

//zadanie nr 1
void KeyboardTask(void *params) {
  
  
}
//zadanie nr 2
void SerialReceiveTask(void *params) {
  
  
}

//zadanie nr 3
void WatchdogTask(void *params) {
  
  int	a;
  
  a=0;
  
}


//program glowny
int main(int argc, char * argv[])
{
  init_scheduler(); //init schedulera, tablicy
  init_timer_irq();  //init timer0
  
  AddTask(0, 2, &KeyboardTask, NULL); //dodanie do schedulera zadania 1
  AddTask(1, 4, &SerialReceiveTask, NULL); //dodanie do schedulera zadania 2
  AddTask(3, 8, &WatchdogTask, NULL); //dodanie do schedulera zadania 3
  
  execute(); //wywolanie funkcji execute, nieskonczone dzialanie funkcji
}
