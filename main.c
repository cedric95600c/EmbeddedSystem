/*	Author: chans034
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "io.h"
#include <stdlib.h> // random function
#endif




unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn(){
  TCCR1B =0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;
  _avr_timer_cntcurr = _avr_timer_M;
  SREG |= 0x80;
}
void TimerOff(){
  TCCR1B = 0x00;
}
void TimerSet(unsigned long M){
  _avr_timer_M = M;
  _avr_timer_cntcurr = _avr_timer_M;
}

volatile unsigned char TimerFlag = 0;
void TimerISR() { TimerFlag = 1; }
ISR(TIMER1_COMPA_vect){
  _avr_timer_cntcurr --;
  if(_avr_timer_cntcurr==0){
    TimerISR();
    _avr_timer_cntcurr = _avr_timer_M;
  }
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned char matrix[8][8] ={{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
unsigned short horizontalmvmt;
unsigned short verticalmvmt;
unsigned char x[100];
unsigned char y[100];

unsigned char length=3;
enum States { up,down,left,right,end,wait} position;
unsigned char fruitx,fruity;
unsigned char score;
unsigned char dizaine=0;
void createFruit()
{

 unsigned char randNumber = rand();  // Get a random number (0 to 255)
randNumber = randNumber & 0x0F; // Set number range to  0 to 15
if (randNumber > 9)             // Set number range to 0 to 9
    randNumber -= 6;
  while(fruitx >= 8 || fruity >= 8 || matrix[fruitx][fruity] !=0)
    {  fruitx = rand();
      fruitx = fruitx & 0x0F;
      if(fruitx>8){fruitx -=7;}
      fruity = rand();
      fruity = fruity & 0x0F;
      if (fruity>8){fruity -=7;}
    }
}

void  drawScreen(unsigned char  buffer2[8][8])
{unsigned char count;
  // Turn on each row in series
      // count next row
unsigned char i=0;
unsigned char a=0;

  for(i=0;i<8;i++)     
    {
      count=0;
    PORTB =( 0x01 << i);

      for(a=0;a<8;a++)
	{if(a==0){count=count + buffer2[i][7-a];}
	 if(a==1){count=count + buffer2[i][7-a]*2;}
	 if(a==2){count=count + buffer2[i][7-a]*4;}
	 if(a==3){count=count + buffer2[i][7-a]*8;}
	 if(a==4){count=count + buffer2[i][7-a]*16;}
	 if(a==5){count=count + buffer2[i][7-a]*32;}
	 if(a==6){count=count + buffer2[i][7-a]*64;}
	 if(a==7){count=count + buffer2[i][7-a]*128;}}
      PORTD = PORTD - count;
      asm("nop");
      asm("nop");
      
   
	PORTD= 0xFF;
	PORTB=0x00;}   
}
void button()
{
unsigned short vertical = 512;
unsigned short horizontal = 512;
switch (ADMUX)
	{
	case 0x40:
	  { 
	     verticalmvmt = ADC;
	    ADC=0;
	    ADMUX=0x41;
	    break;
	  }
	  	case 0x41:
	  {	   
	    horizontalmvmt = ADC;
	    ADC=0;
	    ADMUX=0x40;
	    break;
	  }
	}
      if(horizontalmvmt > (horizontal + 300) && horizontalmvmt > verticalmvmt && y[0]!=y[1]-1){position = up;}//haut led 1 
      if(horizontalmvmt < (horizontal - 300) && horizontalmvmt < verticalmvmt && y[0]!=y[1]+1){position= down;}//bas led 2 
      if( verticalmvmt < (vertical - 300) && horizontalmvmt > verticalmvmt && x[0]!=x[1]+1)   {position = left;}//gauche led 3 
      if( verticalmvmt > (vertical + 300) && horizontalmvmt < verticalmvmt && x[0]!=x[1]-1)   {position=right;}//droite led4
}

void moove()
{

 unsigned char newx[100];
 unsigned char newy[100];
 unsigned char i,j;
unsigned char blank_matrix[8][8] ={{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
 blank_matrix[fruitx][fruity]=1;

 switch(position)
    { case up:
	//deplacement
	if(y[0]<7)
	  { newy[0]=y[0]+1;
	    newx[0]=x[0];
	  for(i=1;i<length;i++)
	  {
	  newx[i]=x[i-1];
	  newy[i]=y[i-1];
	  }
	  if(newx[0]==fruitx && newy[0]==fruity)
	    {length++;
	      score=score+1;
	      newx[length]=x[length-1];
	      newy[length]=y[length-1];
	      createFruit();
	  }
	  for(i=0;i<length;i++)
	  {
	    x[i]=newx[i];
	    y[i]=newy[i];
	  }
	  //self collision
	  for(i=1;i<length;i++)
	  {
	    for(j=1;j<length;j++)
	      {
		if(x[0]==x[i]&&y[0]==y[i]){ LCD_DisplayString(1, "Score:          Self  colision u"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
		  LCD_WriteData( score + '0' ); while(position != wait){drawScreen(matrix); if((PINA & 0x04)==0){position=wait;} }}
	      }
	  }
	  //fill new matrix
	  for(i=0;i<length;i++)
	    {
		
		  blank_matrix[x[i]][y[i]]=1;
		
	    }
	  }//if border stop
       	else{ LCD_DisplayString(1, "Score:          Wall colision u"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
       LCD_WriteData( score + '0' ); while(position !=wait){drawScreen(matrix); if((PINA & 0x04)==0){position=wait;} }}
	//new matrix = old one
	for(i=0;i<8;i++)
	  {for(j=0;j<8;j++){   
	      matrix[i][j]=blank_matrix[i][j];
	 }}

     break;

     case right:
	if(x[0]<7)
	  {newx[0]=x[0]+1;
	    newy[0]=y[0];
	  for(i=1;i<length;i++)
	  {
	  newx[i]=x[i-1];
	  newy[i]=y[i-1];
	  }
	  if(newx[0]==fruitx && newy[0]==fruity)
	    {length++;
	    
	      newx[length]=x[length-1];
	      newy[length]=y[length-1];
	      createFruit();
	      score=score+1;
	  }
	  for(i=0;i<length;i++)
	  {
	    x[i]=newx[i];
	    y[i]=newy[i];
	  }
	   //self collision
	  for(i=1;i<length;i++)
	  {
	    for(j=1;j<length;j++)
	      {
		if(x[0]==x[i]&&y[0]==y[i]){ LCD_DisplayString(1, "Score:          Self  colision r"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
		  LCD_WriteData( score + '0' ); while(position!=wait){drawScreen(matrix);  if((PINA & 0x04)==0){position=wait;} }}
	      }
	  }
	  for(i=0;i<length;i++)
	    {
		blank_matrix[x[i]][y[i]]=1;}
	  }
        	else{ LCD_DisplayString(1, "Score:          Wall colision r"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
       LCD_WriteData( score + '0' ); while(position!=wait){drawScreen(matrix); if((PINA & 0x04)==0){position=wait;} }}
	for(i=0;i<8;i++)
	  {for(j=0;j<8;j++){
	      matrix[i][j]=blank_matrix[i][j];}}

     break;
 case left:
	if(x[0]>0)
	  {newx[0]=x[0]-1;
	    newy[0]=y[0];

	  for(i=1;i<length;i++)
	  {
	  newx[i]=x[i-1];
	  newy[i]=y[i-1];
	  }
	  if(newx[0]==fruitx && newy[0]==fruity)
	    {length++;
	     
	      newx[length]=x[length-1];
	      newy[length]=y[length-1];
	      createFruit();
	      score=score+1;
	  }
	  for(i=0;i<length;i++)
	  {
	    x[i]=newx[i];
	    y[i]=newy[i];
	  }
	   //self collision
	  for(i=1;i<length;i++)
	  {
	    for(j=1;j<length;j++)
	      {
	if(x[0]==x[i]&&y[0]==y[i]){ LCD_DisplayString(1, "Score:          Self  colision l"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
		  LCD_WriteData( score + '0' ); while(position!=wait){drawScreen(matrix);  if((PINA & 0x04)==0){position=wait;} }}
	      }
	  }
	  for(i=0;i<length;i++)
	    {
		blank_matrix[x[i]][y[i]]=1;}
	  }
 	else{ LCD_DisplayString(1, "Score:          Wall colision l"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
       LCD_WriteData( score + '0' ); while(position!=wait){drawScreen(matrix); if((PINA & 0x04)==0){position=wait;} }}
	for(i=0;i<8;i++)
	  {for(j=0;j<8;j++){
	      matrix[i][j]=blank_matrix[i][j];}}
     break;
      case down:
	if(y[0]>0)
	  {newy[0]=y[0]-1;
	    newx[0]=x[0];
	  for(i=1;i<length;i++)
	  {
	  newx[i]=x[i-1];
	  newy[i]=y[i-1];
	  }
	  if(newx[0]==fruitx && newy[0]==fruity)
	    {length++;
	    
	      newx[length]=x[length-1];
	      newy[length]=y[length-1];
	      createFruit();
	      score=score+1;
	  }
	  for(i=0;i<length;i++)
	  {
	    x[i]=newx[i];
	    y[i]=newy[i];
	  }
	   //self collision
	  for(i=1;i<length;i++)
	  {
	    for(j=1;j<length;j++)
	      {
	if(x[0]==x[i]&&y[0]==y[i]){ LCD_DisplayString(1, "Score:          Self  colision d"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
		  LCD_WriteData( score + '0' ); while(position!=wait){drawScreen(matrix);  if((PINA & 0x04)==0){position=wait;} }}
	      }
	  }
	  for(i=0;i<length;i++)
	    {
		blank_matrix[x[i]][y[i]]=1;}
	  }
 	else{ LCD_DisplayString(1, "Score:          Wall colision d"        ); LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
       LCD_WriteData( score + '0' ); while(position!=wait){drawScreen(matrix);  if((PINA & 0x04)==0){position=wait;} }}
	for(i=0;i<8;i++)
	  {for(j=0;j<8;j++){
	      matrix[i][j]=blank_matrix[i][j];}}
     break;
    case wait:
      score=0;
      dizaine=0;
      LCD_Cursor(1);
       LCD_DisplayString(1, "Press the joystick to play");
       x[0]=1;x[1]=1;x[2]=1;
       y[0]=4;y[1]=3;y[2]=2;
       length =3;
       for(i=0;i<8;i++)
	  {for(j=0;j<8;j++){
	      matrix[i][j]=0;}}
	matrix[x[0]][y[0]]=1;
	matrix[x[1]][y[1]]=1;
	matrix[x[2]][y[2]]=1;
      break;
 default:
    matrix[x[0]][y[0]]=1;
    matrix[x[1]][y[1]]=1;
    matrix[x[2]][y[2]]=1;

 
    break;
    }
    
}
int main(void) {
    /* Insert DDR and PORT initializations */
   ADMUX |=(1<<REFS0);//setting the reference of ADC
   ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE);
 
  DDRA=0xF0;PORTA=0x0F;
  DDRB=0xFF;PORTB=0x00;
  DDRC=0xFF;PORTC=0x00;
  DDRD=0xFF;PORTD=0x00;
  TimerSet(5); TimerOn();
  x[0]=1;x[1]=1;x[2]=1;
  y[0]=4;y[1]=3;y[2]=2;
  matrix[x[0]][y[0]]=1;
  matrix[x[1]][y[1]]=1;
  matrix[x[2]][y[2]]=1;
  position=wait;
   TimerSet(1);
   TimerOn();
  LCD_init();
  unsigned char i=0;
 unsigned long tick_elapsedTime = 0;
 unsigned long draw_elapsedTime =0;
 score=0;
 createFruit();

    /* Insert your solution below */ 
    while (1)
    {
      if((PINA & 0x04) == 0)
	{ LCD_DisplayString(1, "Stop:");
	  position = wait;
	 }
      if (tick_elapsedTime >= 500) {
        
    
       moove();
      
       if(position != wait){
      
	  LCD_DisplayString(1, "Score:");
       LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' );
	  LCD_Cursor(8);
       LCD_WriteData( score + '0' );
       if(score >= 10){score=score-10;dizaine++;}
       if(dizaine >= 2){ LCD_DisplayString(1, "Score:          You Win!");LCD_Cursor(7);
       LCD_WriteData( dizaine + '0' ); LCD_Cursor(8);
       LCD_WriteData( score + '0' );while(position!=wait){drawScreen(matrix); if((PINA & 0x04)==0){position=wait;} }}}
       
      tick_elapsedTime = 0;
        }
      if(draw_elapsedTime >= 1)
        { button();
	  drawScreen(matrix);
	  draw_elapsedTime=0;
	}
      while(!TimerFlag);
      TimerFlag = 0;
      tick_elapsedTime +=1;
      draw_elapsedTime +=1;
    } 
 
    
    return 1;
}


