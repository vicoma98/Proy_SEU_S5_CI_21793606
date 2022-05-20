#include "FreeRTOS.h"
#include <stdio.h>
#include "tareas_serie.h"
#include <task.h>
#include "cmsis_os.h"
#include "main.h"
#include "utility.h"
#include <string.h>
#include <stm32f4xx_hal_dma.h>

void Task_Display( void *pvParameters );
void Task_DMA( void *pvParameters );
void Task_Send( void *pvParameters );
void Task_Receive( void *pvParameters );

#define buffer_DMA_size 2048
uint8_t buffer_DMA[buffer_DMA_size];
extern UART_HandleTypeDef huart1;
#define UART_ESP_AT_WIFI (&huart1)
#define UART_ESP8266 (&huart1)



extern BUFF_BUFFER_t * buff;
extern BUFF_BUFFER_t * buff_rx;

void serie_Init_FreeRTOS(void){

	BaseType_t res_task;

	printf (PASCU_PRJ " at "__TIME__);
	fflush(0);

	buff=bufferCreat(128);
	if (!buff) return;

	buff_rx=bufferCreat(512);
	if (!buff_rx) return;

	res_task=xTaskCreate(Task_Display,"DISPLAY",512,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}

	res_task=xTaskCreate(Task_DMA,"DMA",512,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
		if( res_task != pdPASS ){
				printf("PANIC: Error al crear Tarea Visualizador\r\n");
				fflush(NULL);
				while(1);
		}

	res_task=xTaskCreate(Task_Send,"ENVIO",512,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}

	res_task=xTaskCreate(Task_Receive,"RECEIVE",512,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}
}
char candenafinal[100];
void boot_wifi(char * ssid, char * passwd){
	uint32_t res;
	//char conf[]="AT+CWMODE=1\r\n";
	char cad[]="AT+CWJAP=\"%s\",\"%s\"\r\n";

	sprintf(candenafinal,cad,ssid,passwd);

	// send line (command) to ESP
	res=HAL_UART_Transmit(UART_ESP_AT_WIFI,"AT+CWMODE=1\r\n",15,1000);
		// wait a bit time
		// prepare reception buffer from ESP
	HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
	osDelay(500);
		//stop reception probably all data are in dma buffer
	HAL_UART_DMAStop(UART_ESP_AT_WIFI);
	// send to console ESP answer.
	int buffer_ct1=HAL_DMA_getcounter(UART_ESP_AT_WIFI);
	int buffer_ct=0;
	while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
	// wait a bit time
	osDelay(1);

	// send line (command) to ESP
	res=HAL_UART_Transmit(UART_ESP_AT_WIFI,candenafinal,strlen(candenafinal),1000);
	// wait a bit time
	// prepare reception buffer from ESP
	HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
	osDelay(500);
	//stop reception probably all data are in dma buffer
	HAL_UART_DMAStop(UART_ESP_AT_WIFI);
	// send to console ESP answer.
	buffer_ct1=HAL_DMA_getcounter(UART_ESP_AT_WIFI);
	buffer_ct=0;
	while (buffer_ct<buffer_ct1)
		res=buff->put(buff,buffer_DMA[buffer_ct++]);
	// wait a bit time
	osDelay(1);

}
void entregable(char * nombreMaquina,char * ssid, char * passwd){
	uint32_t res;
		char cad[]="AT+CWJAP=\"%s\",\"%s\"\r\n";

		sprintf(candenafinal,cad,ssid,passwd);

		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,"AT+CWMODE=3\r\n",15,1000);
		HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
		osDelay(500);
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);
		int buffer_ct1=buffer_DMA_size - HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		int buffer_ct=0;
		while (buffer_ct<buffer_ct1)
				res=buff->put(buff,buffer_DMA[buffer_ct++]);
		osDelay(1);


		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,candenafinal,strlen(candenafinal),1000);
		HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
		osDelay(500);
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);
		buffer_ct1=buffer_DMA_size - HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		buffer_ct=0;
		while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
		osDelay(1);


		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,"AT+CIFSR\r\n",15,1000);
		HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
		osDelay(500);
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);
		buffer_ct1=buffer_DMA_size - HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		buffer_ct=0;
		while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
		osDelay(1);


		char cad2[]="AT+CIPSTART=\"TCP\",\"%s\",80\r\n";
		sprintf(candenafinal,cad2,nombreMaquina);
		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,candenafinal,strlen(candenafinal),1000);
		HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
		osDelay(500);
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);
		buffer_ct1=buffer_DMA_size - HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		buffer_ct=0;
		while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
		osDelay(1);

		//***get***
		char cad3[]="AT+CIPSEND=%i";
		char * get="GET /api/json/cet/now HTTP/1.1\r\nAccept: text/html\r\nHost: worldclockapi.com\r\n\r\n";
		sprintf(candenafinal,cad3,sizeof(get));
		HAL_UART_Receive_DMA(UART_ESP8266, buffer_DMA,2048);
		//HAL_UART_Transmit(UART_ESP8266,candenafinal);
		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,candenafinal,sizeof(candenafinal),1000);
		HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
		osDelay(1000);
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);
		buffer_ct1=buffer_DMA_size - HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		buffer_ct=0;
		while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
		osDelay(1);


		HAL_UART_Receive_DMA(UART_ESP8266, buffer_DMA,2048);
		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,candenafinal,sizeof(candenafinal),1000);
		osDelay(1000);
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);
		buffer_ct1=buffer_DMA_size - HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		buffer_ct=0;
		while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
		osDelay(2000);

}


void Task_Send( void *pvParameters ){

	uint32_t it;
    uint32_t res;

	char cad[100];
	it=0;
	//boot_wifi("OPPOReno2","ilovematy");
	entregable("worldclockapi.com","OPPOReno2","ilovematy");
	while(1){
		it++;
		sprintf(cad,"IT %d\r\n",(int)it);
		res=buff->puts(buff,(BUFF_ITEM_t *)cad,strlen(cad));
		vTaskDelay(10000/portTICK_RATE_MS );

	}
}

void Task_Receive( void *pvParameters ){

	uint32_t it;
    uint32_t res;
    BUFF_ITEM_t  car;
#define buffer_length	128
    BUFF_ITEM_t  buffer[buffer_length];
    int buffer_ct,buffer_ct1;
    int crln_detect;

	it=0;


	while(1){
		it++;

		crln_detect=0;
		buffer_ct=0;

		while(crln_detect<2){
	    	res=buff->get(buff_rx,&car);
	    	buffer[buffer_ct++]=car;
	    	if (buffer_ct>1){

	    		if ((buffer[buffer_ct-2]=='\r')&&(buffer[buffer_ct-1]=='\n')) // \r\n detection end of line
					crln_detect=2;
				else
					if ((buffer_ct)==buffer_length)  // line out of limits --> error
						crln_detect=3;
	    	}

		}

		// prepare reception buffer from ESP
		HAL_UART_Receive_DMA(UART_ESP_AT_WIFI, buffer_DMA,buffer_DMA_size);
		// send line (command) to ESP
		res=HAL_UART_Transmit(UART_ESP_AT_WIFI,buffer,buffer_ct,1000);
		// wait a bit time
		osDelay(500);
		//stop reception probably all data are in dma buffer
		HAL_UART_DMAStop(UART_ESP_AT_WIFI);

		// send to console ESP answer.
		buffer_ct1=HAL_DMA_getcounter(UART_ESP_AT_WIFI);
		buffer_ct=0;
		while (buffer_ct<buffer_ct1)
			res=buff->put(buff,buffer_DMA[buffer_ct++]);
		// wait a bit time
		osDelay(1);
	}
}
