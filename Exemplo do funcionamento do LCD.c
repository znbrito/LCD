#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "hd44780.h"
#include <string.h>
#define    baud 57600  // baud rate
// Caution: erroneous result if F_CPU is not correctly defined
#define baudgen ((F_CPU/(16*baud))-1)  //baud divider

void init_USART(void)
{
  UBRR0 = baudgen;
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

char botao[6];

ISR(ADC_vect)
{
	if(ADC < 50) // botão RIGHT
	{
		strcpy(botao, "RIGHT");
	}
	else if(ADC < 250) // botão UP
	{
		strcpy(botao, "UP");
	}
	else if(ADC < 450) // botão DOWN
	{
		strcpy(botao, "DOWN");
	}
	else if(ADC < 650) // botão LEFT
	{
		strcpy(botao, "LEFT");
	}
	else if(ADC < 850) // botão SELECT
	{
		strcpy(botao, "SELECT");
	}
	else // nenhum botão pressionado
	{
		strcpy(botao, "NENHUM");
	}
}

int main()
{
	// LCD Keypad Shield de 16 colunas por 2 linhas

	// PINOS
	/*
	   VSS -> GND (está ligado ao GND do Arduino para que ambos tenham massas comuns)
	   VDD -> +5v (está ligado à porta de 5V do Arduino)
	   V0 -> ???? (ver datasheet)
	   RS -> PB0
	   RW -> GND (este pino está diretamente ligado à massa uma vez que é desnecessário pois apenas queremos escrever no LCD e não queremos ler nada dele)
	   E  -> PB1
	   D0 -> no ar
	   D1 -> no ar
	   D2 -> no ar
	   D3 -> no ar
	   D4 -> PD4
	   D5 -> PD5
	   D6 -> PD6
	   D7 -> PD7
	   A  -> +5V (está ligado à porta de 5V do Arduino)
	   K  -> ???? (ver datasheet)
	   BOTÕES -> PC0 (os botões são detetados através de um ADC -> ver datasheet)
	*/

	// Coordenadas do display do LCD
	/*
	      Y
	      |
	     _________________________________
	  	/								  \
	X - | 0 1 2 3 4 5 6 7 8 9 A B C D E F |  Posição inicial do cursor (x,y) = (0,0)
		| 1	 							  |
		\_________________________________/

	*/

	// Descrição dos pinos

	/* O controlador usado neste LCD é o HD44780 que é controlado por uma comunicação em paralelo de 4 ou 8 bits. Neste caso usam-se 4 bits pois é o que
	   o LCD permite devido à sua estrutura física, isto é, os pinos D0, D1, D2 e D3 estão ligados ao ar, logo não estão ligados diretamente às portas
	   do shield (que por sua vez estão diretamente ligadas às correspondentes portas do Arduino). As restantes portas D4, D5, D6 e D7 estão, logo
	   enviam-se os 4 bits por estas portas, sendo que D4 corresponde à porta do LSB e D7 À porta do MSB. Para além destas portas, também interessa a
	   porta RS pela qual vão ser enviados dados ou comandos para o controlador e a porta E que é o "enable" do controlador. A porta RW significa
	   R/(/W), isto é caso tenha nível lógico 1 significa que vamos ler alguma coisa do LCD. Isto pode ser útil caso se queira enviar alguma informação
	   (dados ou comandos) para o LCD. Podemos verificar uma flag que sinaliza o final da escrita, portanto, para ler esta flag é necessário que este
	   pino esteja a 1! Outro modo de fazer isto é simplesmente esperar um determinado tempo em que é garantido que a informação já foi enviada para o
	   LCD e neste modo não necessitamos do modo R. Quando assim é, ligamos este pino à massa pois /W é ativo com nível lógico 0. Neste LCD o pino RW
	   está ligado diretamente à massa, logo apenas se pode fazer operações de escrita no LCD esperando um tempo predefinido que garante que a
	   informação já foi enviada.
	 */

	// Descrição dos botões

	/* Este shield possui 5 botões que, através de vários divisores resistivos (ver datasheet), estão ligados a uma porta do shield ligada diretamente à
	   porta PC0 do Arduino. Quando um dos botões é pressionado, fecha-se um circuito que faz com que na entrada PC0 esteja um valor de tensão
	   que varia com o divisor resistivo. Como a cada botão está associado um divisor resistivo, então o valor disponível na entrada PC0 varia de
	   botão para botão, o que faz com que seja possível reconhecer cada botão através da leitura do valor do divisor resistivo a ele associado!
	   Esta leitura é obviamente feita através do ADC associado ao pino PC0, o ADC0. Através das leituras do ADC podemos distinguir qual botão está a
	   ser pressionado. Os valores das leituras para cada botão são:

	   Botões   | 	 Leitura

	   (nenhum) | 	ADC > 1000
	   RIGHT	|	ADC < 50
	   UP		|	ADC < 250
	   DOWN		|	ADC < 450
	   LEFT		|	ADC < 650
	   SELECT	|	ADC < 850

	   O código que permite implementar tudo isto está escrita no programa principal. Para além disto o shied ainda possui um botão de RESET que está
	   eletricamente ligado ao botão de RESET do Arduino
	 */

	// Biblioteca HD44780.h

	/* De várias bibliotecas AVR disponíveis para realizar operações com o controlador deste LCD - HD44780 - esta é mais completa que encontrei pois
	   permite fazer o controlo de mais de um LCDs ao mesmo tempo (suporta 4 [AINDA NÃO TESTADO]) e para além disso permite também trabalhar com o
	   pino RW ligado à massa ou não. Este último ponto é bastante útil pois permite trabalhar com uma vasta gama de LCD incluindo os integrados em
	   shields (como é o caso) pois alguns tem o pino RW ligado à massa, o que os limita bastante em relação ao uso de bibliotecas pois a maior parte
	   usa o pino RW.
	   Esta biblioteca é bastante fácil de utilizar e de configurar. Ela possui um ficheiro chamado "hd44780_settings_example.h" que é o ficheiro de
	   configurações da biblioteca em função do LCD utilizado. Ela permite definir as portas que estão a ser utilizadas, bem como o modo em que o pino
	   RW está a ser utilizado, o número de linhas do LCD, etc. e está muito bem explicado como configurar este ficheiro no próprio ficheiro. Depois de
	   configurado o nome do ficheiro deve ser mudado para "hd44780_settings.h".
	   Outra funcionalidade desta biblioteca é possuir uma função chamada lcd_command que permite alterar o valor lógico dos bits RS, RW e D0
	   até D7, o que é bastante útil pois dá-nos a possibilidade de escrever novas funções (como fiz na versão que fiz desta biblioteca). Para se criar
	   esta novas funções é necessário ver a datasheet deste controlador para perceber como se fazem. Uma vez que eu próprio alterei esta biblioteca,
	   acrescentei algumas funções (através da função lcd_command()) às que já estavam disponíveis, que se encontram descritas de seguida. POR ISSO,
	   A BIBLIOTECA INCLUÍDA NESTE PROGRAMA DEVE SER A QUE SE ENCONTRA NA PASTA "Biblioteca HD77480" -> "HD44780 v1.12 - José Brito" ! PORTANTO DEVEM
	   SER COPIADOS OS FICHEIROS "hd44780.c", "hd44780.h" e "hd44780_settings.h" PARA A PASTA DO PROJETO

	 */

	// Funções da biblioteca Biblioteca HD44780.h alterada por mim

	/* lcd_init(); // inicializa o display do LCD, o cursor (inicialmente invisível) e as portas I/O. Esta é a primeira função a ser chamada em qualquer programa que use esta biblioteca
	   lcd_command(uint8_t cmd); // função que altera bits específicos no registo RS (comandos ou dados) que vai ser enviado para o controlador do LCD. Esta função pode ser usada para escrever novas funções
	   lcd_clrscr(); // apaga tudo o que foi escrito no display do LCD e coloca o cursor na posição (x,y) = (0,0) sem alterar as suas definições (se estava a piscar, sublinhado ou ambos)
	   lcd_home(); // coloca o cursor na posição (x,y) = (0,0) sem alterar as suas definições (se estava a piscar, sublinhado ou ambos)
	   lcd_goto(uint8_t pos); // coloca o cursor na posição especificada: (0x00) -> (x,y) = (0,0) ||| (0x0F) -> (x,y) = (15,0) ||| (0x40) -> (x,y) = (0,1) ||| (0x4F) -> (x,y) = (15,1)
	   lcd_putc(char c); // escreve o parâmetro de entrada c (um carater) na posição onde o cursor se encontra
	   lcd_puts(const char *s); // escreve o parâmetro de entrada s (uma string de carateres) na posição onde o cursor se encontra
	   lcd_puts_P(const char *progmem_s); // ??? (escreve a string [constant char] s na memória flash do controlador HD44780 ???)
	   lcd_blink_cursor(); // põe o cursor a piscar
	   lcd_underline_cursor(); // sublinha o cursor
	   lcd_underline_blink_cursor(); // sublinha o cursor e põem-o a piscar
	   lcd_no_cursor(); // desliga o cursor (deixa de estar sublinhado e/ou a piscar)
	   lcd_shift_cursor_right(); // faz um shift right do cursor, isto é, move-o uma coordenada em x para a direita
	   lcd_shift_cursor_left(); // faz um shift left do cursor, isto é, move-o uma coordenada em x para a esquerda
	   lcd_shift_display_right(); // faz um shift right do display do LCD (ambas as linhas), isto é, move tudo o que foi escrito uma coordenada em x para a direita
	   lcd_shift_display_left();  // faz um shift left do display do LCD (ambas as linhas), isto é, move tudo o que foi escrito uma coordenada em x para a esquerda
	   lcd_write_backwards(); // escreve futuras strings e chars de trás para a frente e da esquerda para a direita em vez de escrever normalmente e da direita para a esquerda
	   lcd_write_normally(); //  escreve futuras strings e chars normalmente e da direita para a esquerda. Apenas chamar esta função se anteriormente chamou a função lcd_write_backwards()
	   lcd_write_speed_slow(); // escreve futuras string e chars mais lentamente (para ser visível o que está realmente a ser escrito no display do lcd). Esta velocidade pode ser alterada, mudando o valor do delay na linha 397 do ficheiro "hd44780.c"
	   lcd_write_speed_normal(); // escreve futuras strings e chars instantaneamente (predefinição). Apenas chamar esta função se anteriormente chamou a função lcd_write_speed_slow()
	*/

	// ----------------------------------------------------------

	// CONFIGURAÇÃO DA LEITURA DOS BOTÕES ATRAVÉS DO ADC0

	sei(); // habilitação das interrupções globais

	ADMUX = 0b01000000; // ADC escolhido é o ADC0 que se encontra no pino PC0
	ADCSRA = 0b10001111; // estão ativadas as interrupções quando o ADC0 acabar de fazer uma conversão. A conversão é manual

	DIDR0 = 0b00000001; // este registo desativa a função digital do pino correspondente ao bit especificado por ele (neste caso desativamos a função digital do pino PC0)

	/* Nota: A conversão do ADC deve ser manual uma vez que todos os timers 0 e 1 que podiam ser usados para fazer a conversão automática estão
	   indiretamente associados aos pinos que estão a ser usados para fazer a comunicação com o LCD. Apesar destes pinos não estarem a ser usados como
	   saídas de PWM (obviamente), o LCD parece não reagir muito bem a que se mexa nestes pinos fora das configurações feitas na biblioteca hd44780.h.
	   Por essa razão, a conversão deve ser feita manualmente, como está exemplificado no programa a seguir
	*/
	// INÍCIO DO PROGRAMA

	init_USART();
	lcd_init();
	lcd_putc('A'); // o primeiro caracter do programa escrito com as funções lcd_putc ou lcd_puts é muitas vezes aleatório (por razões que desconheço). Para resolver este problema
	lcd_clrscr(); // escreve-se um primeiro carater (neste caso o "A") e em seguida volta-se a apagá-lo. Mais nenhum carater deverá ser aleatório após este procedimento

	// ESCREVA O PROGRAMA A PARTIR DAQUI

	lcd_write_speed_slow();
	lcd_puts("Botao:");

	while(1)
	{
		lcd_goto(0x40); // coloca o cursor na posição (0,1) -> segunda linha, primeira coluna
		ADCSRA = ADCSRA | 0b01000000; // iniciar a conversão do ADC do pino PC0 através da ativação manual do bit ADSC
		while(ADCSRA & (1<<ADSC)); // o programa entra num ciclo infinito até acabar a conversão do ADC
		lcd_write_speed_normal();
		lcd_puts(botao); // escreve o nome do botão pressionado
		lcd_puts("    "); // escreve "    " para não aparecerem carates a mais
	}
}
