#include "16f877a.h"
#device ADC=10
#use delay(clock=20m)
#use rs232(UART1)

void main()
{
      int vin0;//input variables
      setup_adc(ADC_CLOCK_INTERNAL);//adc clock
      setup_adc_ports(ALL_ANALOG); //input combination
      set_adc_chanel(0); // select ra0
      
      int incode;
      setup_uart(9600);
      
      while(1)
      {
         incode = getc(); //read char from uart
         printf("ASCII = %d", incode); // display on terminal
         putc(13);
      }
}
