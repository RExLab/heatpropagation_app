#ifndef APP_H
#define APP_H

// ###########################################################################################################################################
// DESENVOLVIMENTO
#define LOG_MODBUS		pdON

// ###########################################################################################################################################
// MODEL WORKTEMP

#define MODEL			"ms10"	// somente 4 chars, por causa so modbus
#define FIRMWARE_VERSION "1.0"	// usar x.y s� com uma casa decimal, pois estamos usando a impress�o de temperatura
								// usar 3 chars por causa do modbus
#define PROGAM_NAME "MSIP" MODEL " firmware v" FIRMWARE_VERSION " [compiled in " __DATE__ " " __TIME__ "]" CMD_TERMINATOR


#define nTHERMOMETER  		 3    	// Quantidade de term�metro que o RH deve gerenciar. O ID modbus do term�metro deve ser sequencial de 1 a quantidade de term�metro
#define EXP_nSLOTS_SAMPES 	 600  	// 600 = 10 minutos. Quantos slots guardaremos para as amostras das temperaturas
#define nBUFFER_SAMPLES_READ 50  	// Quantidade de registradores dos resultado vamos ler por vez. M�ximo 120



// ###########################################################################################################################################
// INCLUDES


// ###########################################################################################################################################
// MODBUS
#define COM_PORT "/dev/ttyAMA0" 	// Consultar /dev/ttySxxx do linux
									// Opensuse com placa pci para serial = "/dev/ttyS4"
									// Raspberry "/dev/ttyAMA0"
									// Para descobrir qual o n� da porta UART fa�a o seguinte:
									// 		Conecte o RASP com um PC via cabo serial.
									//		No PC com um terminal de comnica��o serial abra uma conex�o com RASP a 115200bps 8N1
									//		No RAPS liste as portas tty: ls tty*
									// 		No RAPS na linha de comando envie uma mensagem para cada porta serial: echo ola /dev/ttyXXXX
									//			Fa�a para para todas as portas listadas at� que receba a mensagem no terminal do PC

#define MODBUS_BAUDRATE		B57600 // N�o usar acima de 57600, pois h� erro de recep��o do raspberry.
									// Deve ser algum bug de hardware do rasp porque o baudrate do rasp n�o fica indentico do ARM
									// pois a comunica��o com PC a 115200 funciona bem.
									// Ou a tolerancia de erro no rasp n�o � t�o grande como no PC onde o ARM tem um erro consider�vel
									//	TODO Quando usar o oscilador interno do ARM refazer os testes a sabe com usando oscilador interno do ARM isso se resolve

// ###########################################################################################################################################
// CONTROLE DO SISTEMA

typedef enum {
	cmdNONE = 0,
	cmdGET_NTHERM,
	cmdGET_INFO,        // RH: Comando para ler os registradores: modelo do RH e vers�o do firmware
						// NetWork: Pegar informa��es de controle do experimento e valores dos term�metros
    cmdGET_ACTION,      // RH: Comando para ler que a��o o recurso de hardware est� fazendo agora
                                // 0: O recurso de hardware n�o est� fazendo nada;
                                // 1: O recurso de hardware est� fazendo um experimento ou uma configura��o nos term�metros.
    cmdGET_STATUS,      // RH: Comando para o status do experimento
                                // 0: Sinaliza que o experimento ou processo de configura��o dos term�metros foi feito com sucesso;
                                // 1: N�o foi poss�vel se comunicar pelo menos com um term�metro;
                                // 2: Pelo menos um dos term�metros deu erro de leitura de temperatura, ou a mesma ficou fora dos limites de trabalho
    cmdGET_SAMPLES,     // RH: Comando para pegar quantas amostras foram tiradas durante experimento. M�ixma 600 amostra = 10 minutos de experimento
    cmdGET_RESULTS,     // RH/Network: Comando para ler as amostras tiradas durante experimento de todos os term�metros. M�ximo 600 amostra = 10 minutos de experimento
    cmdEXP_START,       // RH/Network: Comando para dar in�cio ao experimento
    cmdEXP_STOP,        // RH/Network: Comando para parar o experimento
    cmdGET_TEMP,        // RH/Network: Comando para ler as temperaturas
    cmdRST_STAT_TEMP,   // RH/Network: Comando para iniciar a estatisticas de temperatura
    cmdSET_TEMP_TYPE,    // RH/Network: Comando para ajustar a grandeza da temperatura
                                // 0 - Ajuste via jumper
                                // 1 - Celsius
                                // 2 - Fahrenheit
	cmdSET_RELAY,  
    cmdSET_DURATION,
	cmdGET_DURATION,
	cmdCANCEL_COOLING,  
    cmdSET_COOLING_DURATION,
	cmdGET_COOLING_DURATION,
	cmdGET_TEMP_LIMIT,
	cmdSET_TEMP_LIMIT,
	cmdSET_2_RELAY
} tCommand;

typedef struct {
	int stsCom;				// Status de comunica��o:
							//		0: Sem comunica��o com o dispositivo. O mesmo n�o est� conectado, ou est� desligado, ou n�o h� dispositivo neste endere�o.
							//		1: O mult�metro recebeu uma fun��o que n�o foi implementada;
							//		2: Foi acessado a um endere�o de registrador inexistente;
							//		3: Foi tentado gravar um valor inv�lido no registrador do mult�metro;
							//		4: Um irrecuper�vel erro ocorreu enquanto o mult�metro estava tentando executar a a��o solicitada;
							//		5: Comunica��o estabelecida com sucesso
	int sts;				// Status do sensor de temperatura
							//      0: Sinaliza que o WorkTemp est� lendo pela primeira vez a temperatura.
							//    		Isto somente acontece no momento que o WorkTemp � ligado.
							//      1: O WorkTemp j� cont�m valor da temperatura;
							//      2: Sinaliza um erro, indica que a temperatura est� abaixo da escala
							//          permitida pelo WorkTemp. Ou seja, a conex�o com o sensor
							//          est� em curto, seja o sensor em si ou o fio que se liga a ele;
							//      3: Sinaliza um erro, indica que a temperatura est� acima da escala
							//          permitida pelo WorkTemp. Ou seja, a conex�o com o sensor est� aberta,
							//          ou o sensor est� danificado, ou o fio do sensor est� partido
							//          ou n�o est� conectado ao WorkTemp.

	int temperatureType;	//  Sinaliza se a grandeza da temperatura:
							//      0: Graus Celsius;
                    		//      1: Graus Fahrenheit.
	int value; 				// valor da temperatura * 10
	int valueMin; 			// valor da temperatura * 10
	int valueMax; 			// valor da temperatura * 10
} tThermometer;


typedef struct {
	// flash de controle de a��o para acesso ao RH. Esses flags devem ser atualizados via rede
	unsigned exit:1;		// 1 Sinaliza para cair fora do programa
	unsigned getInfo:1;		// Sinaliza para capturar as informa��es do recurso de hardware
	unsigned getTemp:1;
	unsigned setTempType:1;
	unsigned rstTempStatistics:1;
	unsigned getAction:1;
	unsigned getStatus:1;
	unsigned getSamples:1;
	unsigned getResult:1;
	unsigned expStart:1;
	unsigned expStop:1;
	unsigned expGetResult:1;	// Sinaliza para capturar as informa��es das sa�das digitais do recurso de hardware
	unsigned getDuration:1;
	unsigned setDuration:1;
	unsigned set2relay:1;
	unsigned getLimit:1;
	unsigned setLimit:1;
	unsigned getCooling:1;
	unsigned setCooling:1;
	unsigned cancelCooling:1;
	unsigned setRelay:1;
	int stsCom;			// Status de comunica��o com RH via modbus
							//		0: Sem comunica��o com o RH. O mesmo n�o est� conectado, ou est� desligado, ou n�o h� dispositivo neste endere�o.
							//		1: O RH recebeu uma fun��o que n�o foi implementada;
							//		2: Foi acessado a um endere�o de registrador inexistente;
							//		3: Foi tentado gravar um valor inv�lido no registrador do RH;
							//		4: Um irrecuper�vel erro ocorreu enquanto o RH estava tentando executar a a��o solicitada;
							//		5: Comunica��o estabelecida com sucesso
	uint rhID;			// Qual o ID do RH no barramento modbus
	string rhModel;		// Modelo do RH
	string rhFirmware;	// Firmware vers�o
	tThermometer thermometer[nTHERMOMETER];
	uint tempType;
	int thermAccessAddr;  							// Endere�o do termometro a ser lido
    int nThermAccessSamples; 						// Quantidade de valores devemos ler no acesso. Valor m�ximo de 120
    int thermBufferSamples[nBUFFER_SAMPLES_READ];   // Buffer tempor�rio da leitura dos registradores das asmostras de temperatura lida do RH
    int action;			// Sinaliza que a��o o recurso de hardware est� fazendo agora
                    		// 0: O recurso de hardware n�o est� fazendo nada;
                    		// 1: O recurso de hardware est� fazendo um experimento ou uma configura��o nos term�metros.
	int status;     	// Guarda o status do experimento
                    		// 0: Sinaliza que o experimento ou processo de configura��o dos term�metros foi feito com sucesso;
                    		// 1: N�o foi poss�vel se comunicar pelo menos com um term�metro;
                   	 		// 2: Pelo menos um dos term�metros deu erro de leitura de temperatura, ou a mesma ficou fora dos limites de trabalho
    int nSamples;       // Quantidades de amostras de todos os term�metros tiradas durante o experimento
    int expResult[nTHERMOMETER][EXP_nSLOTS_SAMPES]; // resultado dos experimentos
	unsigned int duration, duration_cooling;
	unsigned int relay;
	int temp_limit, time_2relay;
	
} tControl, *pControl;

typedef enum {readREGS, writeREG, writeREGS} tcmd;


// ###############################################################################
// PROTOTIPOS
int modbus_Init(void);
void modbus_SendCommand( tCommand c) ;
void init_control_tad();
void * modbus_Process(void * params);

#endif
