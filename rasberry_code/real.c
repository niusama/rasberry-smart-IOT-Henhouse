#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>
#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mosquitto.h>
#include <cJSON.h>
#define DeviceName "Smart_home"//设备名
#define ProductID "LA57WTHWL6"//产品ID
#define DeviceSceret "9JHiCQQcP9nuZlVDuQ2ZnQ=="//设备秘钥

//服务器IP
#define SERVER_IP "LA57WTHWL6.iotcloud.tencentdevices.com"
#define SERVER_PORT 1883 //端口号

//MQTT三元组
#define ClientID "67027ef4feb1bf7f791c8938_dev3_0_1_2024101105"
#define Username "67027ef4feb1bf7f791c8938_dev3"
#define Password "c59f0b925d1bd90b715837d68e0f11c6e78fdd50da2fdbb87d8b104709ab5dc2"//密文 

//订阅主题:
#define SUB_TOPIC  "$oc/devices/67027ef4feb1bf7f791c8938_dev3/sys/messages/down"//订阅
//发布主题:
#define POST_TOPIC "$oc/devices/67027ef4feb1bf7f791c8938_dev3/sys/properties/report"//发布
#define RETURN_TOPIC "$oc/devices/67027ef4feb1bf7f791c8938_dev3/sys/commands/response/request_id=" //回复
#define BUF_SIZE 128
#define PUB 1
#define SUB 2

typedef struct data_mqtt
{
	char    hostname[BUF_SIZE] ;
	int     port ;
	char    username[BUF_SIZE] ;
	char    passwd[BUF_SIZE] ;
	char    clientid[BUF_SIZE] ;
	char    topic[BUF_SIZE] ;
	int     Qos;
 
	char    updata[BUF_SIZE] ;
	char    state[BUF_SIZE] ;
	char    reported[BUF_SIZE] ;
	char    method[BUF_SIZE] ;
	char    jsonid[BUF_SIZE] ;
	char    identifier[BUF_SIZE] ;
	char    version[BUF_SIZE] ;
}data_mqtt;



char mqtt_message[1024*1024];//上报数据缓存区
char request_id[100];
char mqtt_cmd_message[100];
char mqtt_cmd_data[100];
struct mosquitto    	 *mosq = NULL;
data_mqtt				 mqtt;

int sockfd;



 
/*信号处理函数*/
 void signal_func(int sig)
{
	//printf("捕获的信号:%d\n",sig);
	if(sig==SIGALRM)
	{
	//        MQTT_SentHeart();//心跳包
		alarm(5);
	}
}
/*
硬件连线：
MQ2烟雾传感器：GPIO4
DHT11温湿度传感器：GPIO17
火焰检测传感器：GPIO18
光敏电阻：GPIO27
蜂鸣器：GPIO22
LED灯1：GPIO23
LED灯2：GPIO24
LED灯3：GPIO25
雨滴检测传感器：GPIO5
继电器模块：GPIO6

*/
unsigned int DHT11_T;// 	环境温度
unsigned int DHT11_H;//	环境湿度
int MQ2;//     	烟雾浓度检测
int water;//       雨滴检测 
int flame;//     	火焰检测
int light;//		光强检测
int LED1;//   		LED1控制
int LED2;//   		LED2控制
int LED3;//   		LED3控制

typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
 
#define HIGH_TIME 32
 
int pinNumber = 17;
uint32 databuf;
  
void mqtt_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{

        printf("Obtaining content successfully\n");
        printf("\n");
        printf("Succeeded in obtaining the time and temperature:%s\n", (char *)msg->payload);
		printf("ID is %d\n",msg->mid);
		printf("TOPIC is %s\n",(char *)msg->topic);		

    // 目标字符串的基础部分
    const char* base_response = RETURN_TOPIC;

    // 查找 'request_id=' 在原始字符串中的位置
    const char* request_id_prefix = "request_id=";
    const char* request_id_pos = strstr((const char *)msg->topic, request_id_prefix);

    // 检查是否找到 'request_id='
    if (request_id_pos) {
        // 提取 request_id 的值
        request_id_pos += strlen(request_id_prefix); // 移动至request_id的值

        // 创建一个字符数组来存储 request_id（UUID）
        char request_id[100]; // UUID 的长度为 36 加上结束符
        sscanf(request_id_pos, "%36s", request_id); // 从字符串中提取 UUID

        // 合并字符串并准备响应
        char response_string[256]; // 确保给response字符串足够的空间
        snprintf(response_string, sizeof(response_string), "%s%s", base_response, request_id);
		printf("response_string is %s\n",response_string);
		int check=0;
		char returnmsg[100];
		sprintf(returnmsg,"{\"result_code\": 0,\"response_name\" : \"COMMAND_RESPONSE\",\"paras\" : {\"result\": \"success\"}}");
		printf("return msg is %s\n",returnmsg);
        check=mosquitto_publish(mosq,NULL,response_string,strlen(returnmsg)+1,returnmsg,mqtt.Qos,0) ;
        // 输出结果
		if(check != MOSQ_ERR_SUCCESS )
		{

			printf("%d-,%d-,%d-,%d-,%d-,%d-,%d-,%d-,%d-\n",MOSQ_ERR_SUCCESS,MOSQ_ERR_INVAL,MOSQ_ERR_NOMEM,MOSQ_ERR_NO_CONN,MOSQ_ERR_PROTOCOL
			,MOSQ_ERR_PAYLOAD_SIZE,MOSQ_ERR_MALFORMED_UTF8 ,MOSQ_ERR_QOS_NOT_SUPPORTED,MOSQ_ERR_OVERSIZE_PACKET);
			printf("main reason is %d\n ",check);
			printf("err_280\n");
			mosquitto_destroy(mosq) ;
			mosquitto_lib_cleanup() ;
			return 0;
		}
		else
		{
			printf("publish sucess!\n");
		}
        
    }
	else 
	{
		printf("no request id \n");
	}


		// 解析JSON字符串
	cJSON* json = cJSON_Parse((const char *)msg->payload);
	if (json == NULL) {
		const char* error_ptr = cJSON_GetErrorPtr();
		if (error_ptr) {
			printf("Error before: %s\n", error_ptr);
		}
		return EXIT_FAILURE;
	}

	// 获取"command_name"
	cJSON* command_name = cJSON_GetObjectItem(json, "command_name");
	// 获取"paras"
	cJSON* paras = cJSON_GetObjectItem(json, "paras");

	if (command_name != NULL) {
		printf("command_name: %s\n", command_name->valuestring);
		if (!strcmp(command_name->valuestring, (char*)"BEEP_CTL"))
		{
		
			// 获取"BEEP"
			cJSON* beep = cJSON_GetObjectItem(paras, "BEEP");
			if (beep != NULL) {
				printf("BEEP: %s\n", beep->valuestring);
				if(!strcmp(beep->valuestring, (char*)"ON"))
				{
					printf("蜂鸣器亮");
					digitalWrite(22,HIGH); //蜂鸣器响
				}
				else{
					printf("蜂鸣器暗");
					digitalWrite(22,LOW); //蜂鸣器响
				}
			}else {
				printf("beep is null\n");
			}
			
		}
		else if (!strcmp(command_name->valuestring, (char*)"LED1_CTL"))
		{
			cJSON* beep = cJSON_GetObjectItem(paras, "LED1");
			cJSON_Print(paras);
			if (beep != NULL) {
				printf("LED1_CTL: %s\n", beep->valuestring);
				if(!strcmp(beep->valuestring, (char*)"ON"))
				{
					printf("LED1亮");
					digitalWrite(23,HIGH); //LED1亮
				}
				else{
					printf("LED1暗");
					digitalWrite(23,LOW); //LED1暗
				}
			}else {
				printf("beep is null\n");
			}
		}
		else if (!strcmp(command_name->valuestring, (char*)"LED2_CTL"))
		{
			cJSON* beep = cJSON_GetObjectItem(paras, "LED2");
			if (beep != NULL) {
				printf("LED2_CTL: %s\n", beep->valuestring);
				if(!strcmp(beep->valuestring, (char*)"ON"))
				{
					printf("LED2亮");
					digitalWrite(24,HIGH); //LED2亮
				}
				else{
					printf("LED2暗");
					digitalWrite(24,LOW); //LED2暗
				}
			}else {
				printf("beep is null\n");
			}
		}
		else if (!strcmp(command_name->valuestring, (char*)"LED3_CTL"))
		{
			cJSON* beep = cJSON_GetObjectItem(paras, "LED3");
			if (beep != NULL) {
				printf("LED3_CTL: %s\n", beep->valuestring);
				if(!strcmp(beep->valuestring, (char*)"ON"))
				{
					printf("LED3亮");
					digitalWrite(25,HIGH); //LED2亮
				}
				else{
					printf("LED3暗");
					digitalWrite(25,LOW); //LED2暗
				}
			}else {
				printf("beep is null\n");
			}
		}
		else if (!strcmp(command_name->valuestring, (char*)"RELAYS_CTL"))
		{
			cJSON* beep = cJSON_GetObjectItem(paras, "RELAYS");
			if (beep != NULL) {
				printf("RELAYS_CTL: %s\n", beep->valuestring);
				if(!strcmp(beep->valuestring, (char*)"ON"))
				{
					printf("继电器ON\n");
					digitalWrite(6,HIGH); //继电器ON
				}
				else{
					printf("继电器OFF\n");
					digitalWrite(6,LOW); //继电器OFF
				}
			}else {
				printf("beep is null\n");
			}
		}
	}

}

uint8 readSensorData(void)
{
    uint8 crc; 
    uint8 i;
  
    pinMode(pinNumber, OUTPUT); // set mode to output
    digitalWrite(pinNumber, 0); // output a high level 
    delay(25);
    digitalWrite(pinNumber, 1); // output a low level 
    pinMode(pinNumber, INPUT); // set mode to input
    pullUpDnControl(pinNumber, PUD_UP);
 
    delayMicroseconds(27);
    if (digitalRead(pinNumber) == 0) //SENSOR ANS
    {
        while (!digitalRead(pinNumber))
            ; //wait to high
 
        for (i = 0; i < 32; i++)
        {
            while (digitalRead(pinNumber))
                ; //data clock start
            while (!digitalRead(pinNumber))
                ; //data start
            delayMicroseconds(HIGH_TIME);
            databuf *= 2;
            if (digitalRead(pinNumber) == 1) //1
            {
                databuf++;
            }
        }
 
        for (i = 0; i < 8; i++)
        {
            while (digitalRead(pinNumber))
                ; //data clock start
            while (!digitalRead(pinNumber))
                ; //data start
            delayMicroseconds(HIGH_TIME);
            crc *= 2;  
            if (digitalRead(pinNumber) == 1) //1
            {
                crc++;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}
  
void *PUB_my_msg(void *arg)
{
    while(1)
        {
			printf("%s","i'm working ");
			//读取DHT11温湿度数据
			pinMode(pinNumber, OUTPUT); // set mode to output
			digitalWrite(pinNumber, 1); // output a high level 
			delay(3000);
			if (readSensorData())
			{
				printf("DHT11 Sensor data read ok!\n");
				printf("RH:%d.%d\n", (databuf >> 24) & 0xff, (databuf >> 16) & 0xff); 
				printf("TMP:%d.%d\n", (databuf >> 8) & 0xff, databuf & 0xff);
				
				
				//温度整数部分
				DHT11_T=((databuf >> 24) & 0xff);
				printf("DHT11_T:%d\r\n",DHT11_T);
				//湿度整数部分
				DHT11_H=((databuf >> 8) & 0xff);
				printf("DHT11_H:%d\r\n",DHT11_H);
				
				databuf = 0;
				
			}
			else
			{
				printf("Sensor dosent ans!\n");
				databuf = 0;
			}
		
			
			//读取MQ2烟雾传感器状态
			MQ2=digitalRead (4); //读取GPIO口电平状态
			printf("MQ2:%d\r\n",MQ2);
			//火警报警
			if(MQ2==0)
			{
				digitalWrite(22,HIGH); //蜂鸣器响
			}
			else
			{
				digitalWrite(22,LOW);  //蜂鸣器关
			}
			
			
			//读取火焰传感器状态
			flame=digitalRead (18); //读取GPIO口电平状态
			printf("flame:%d\r\n",flame);
		
			
			
			
			//读取光敏传感器状态
			light=digitalRead (27); //读取GPIO口电平状态
			printf("light:%d\r\n",light);
			
			//读取雨滴检测传感器状态
			water=digitalRead (5); //读取GPIO口电平状态
			printf("water:%d\r\n",water);
			char strRelays[100];
			//下雨关窗
			if(water==0)
			{
				digitalWrite(6,HIGH); //继电器开
				strcpy(strRelays,"\"ON\"");
			}
			else
			{
				digitalWrite(6,LOW);  //蜂鸣器关
				strcpy(strRelays,"\"OFF\"");
			}
			
			
			//读取LED1状态
			LED1=digitalRead (23); //读取GPIO口电平状态
			printf("LED1:%d\r\n",LED1);
			
			//读取LED2状态
			LED2=digitalRead (24); //读取GPIO口电平状态
			printf("LED2:%d\r\n",LED2);
			
			//读取LED3状态
			LED3=digitalRead (25); //读取GPIO口电平状态
			printf("LED3:%d\r\n",LED3);
			char strMQ2[100];
			char strWater[100];
			char strLight[100];
			char strFlame[100];
			char strLed1[100];
			char strLed2[100];
			char strLed3[100];
			if(MQ2==1)
			{
				strcpy(strMQ2,"\"NO SMOKE\"");
			}
			else {
				strcpy(strMQ2,"\"SOMKING\"");
			}
			if (flame==1)
			{
				strcpy(strFlame,"\"NO FLAME\"");
			}else 
			{
				strcpy(strFlame,"\"FIRE ALARM\"");
			}
			if(light==1)
			{
				strcpy(strLight,"\"lighting\"");
			}
			else
			 {
				strcpy(strLight,"\"it's dark\"");
			}
			if(water==1)
			{
				strcpy(strWater,"\"NO water\"");
			}else 
			{
				strcpy(strWater,"\"it's raining \"");
			}
			if(LED1==1)
			{
				strcpy(strLed1,"\"ON\"");
			}
			else 
			{
				strcpy(strLed1,"\"OFF\"");
			}
			if(LED2==1)
			{
				strcpy(strLed2,"\"ON\"");
			}
			else 
			{
				strcpy(strLed2,"\"OFF\"");
			}
			if(LED3==1)
			{
				strcpy(strLed3,"\"ON\"");
			}
			else 
			{
				strcpy(strLed3,"\"OFF\"");
			}
			
			//组合传感器状态数据
			sprintf(mqtt_message,"{\"services\": [{\"service_id\": \"stm32\",\"properties\":{\"DHT11_T\":%d,\"DHT11_H\":%d,\"MQ2\":%s,\"Water\":%s,\"Flame\":%s,\"Light\":%s,\"LED1\":%s,\"LED2\":%s,\"LED3\":%s,\"Relays\":%s}}]}",DHT11_T,DHT11_H,strMQ2,strWater,strFlame,strLight,strLed1,strLed2,strLed3,strRelays);
			//sprintf(mqtt_message,"{\"services\": [{\"service_id\": \"stm32\",\"properties\":{\"DHT11_T\":%d,\"DHT11_H\":%d,\"MQ2\":%d,\"Water\":%d,\"Flame\":%d,\"Light\":%d,\"LED1\":%d,\"LED2\":%d,\"LED3\":%d}}]}",DHT11_T,DHT11_H,MQ2,water,flame,light,LED1,LED2,LED3);//温度
            printf("%s",mqtt_message);
			//上报数据
			//MQTT_PublishData(POST_TOPIC,mqtt_message,0);

            //if( mosquitto_publish(mosq,NULL,mqtt.topic,strlen(msg)+1,msg,mqtt.Qos,0) != MOSQ_ERR_SUCCESS )
            int check=0;
            check=mosquitto_publish(mosq,NULL,mqtt.topic,strlen(mqtt_message)+1,mqtt_message,mqtt.Qos,0) ;
            if(check != MOSQ_ERR_SUCCESS )
            {

                printf("%d-,%d-,%d-,%d-,%d-,%d-,%d-,%d-,%d-",MOSQ_ERR_SUCCESS,MOSQ_ERR_INVAL,MOSQ_ERR_NOMEM,MOSQ_ERR_NO_CONN,MOSQ_ERR_PROTOCOL
                ,MOSQ_ERR_PAYLOAD_SIZE,MOSQ_ERR_MALFORMED_UTF8 ,MOSQ_ERR_QOS_NOT_SUPPORTED,MOSQ_ERR_OVERSIZE_PACKET);
                printf("main reason is %d\n ",check);
                printf("err_6\n");
                mosquitto_destroy(mosq) ;
                mosquitto_lib_cleanup() ;
                return 0;
            }
            else
            {
                printf("publish sucess!\n");
            }
			printf("MQTT_PublishData....\r\n");
            sleep(2);
        }
}




/*
硬件连线：
MQ2烟雾传感器：GPIO4
DHT11温湿度传感器：GPIO17
火焰检测传感器：GPIO18
光敏电阻：GPIO27
蜂鸣器：GPIO22
LED灯1：GPIO23
LED灯2：GPIO24
LED灯3：GPIO25
雨滴检测传感器：GPIO5
继电器模块：GPIO6

*/

int main()
{
	//初始化GPIO口
	wiringPiSetupGpio();  //BCM编码格式
	
	//配置GPIO口的模式
	//输出模式
	pinMode(23,OUTPUT);
	pinMode(24,OUTPUT);
	pinMode(25,OUTPUT);
	pinMode(6,OUTPUT);
	pinMode(22,OUTPUT);
	
	//输入模式
	pinMode(4,INPUT);
	pinMode(18,INPUT);
	pinMode(27,INPUT);
	pinMode(5,INPUT);


	//DHT11温湿度初始化
	pinMode(pinNumber, OUTPUT); // set mode to output
    digitalWrite(pinNumber, 1); // output a high level 
	

    signal(SIGPIPE,SIG_IGN);/*忽略SIGPIPE信号*/
    signal(SIGALRM,signal_func);/*闹钟信号*/
	

    
	bool                	 session = true ;
    int                      rv = 0;
	




    strcpy(mqtt.hostname,"117.78.5.125");
    mqtt.port=1883;

    strcpy(mqtt.username , Username);

    strcpy(mqtt.passwd , Password);

    strcpy( mqtt.clientid , ClientID);

    strcpy( mqtt.topic,POST_TOPIC );
    
    mqtt.Qos=0;

    strcpy(mqtt.method , "wendu");

    strcpy(mqtt.jsonid ,"11111111" );

    strcpy( mqtt.identifier, "CurrentTemperature");

    strcpy( mqtt.version,"0" );

	char subtopic[1024];

	strcpy(subtopic,SUB_TOPIC);

    rv = mosquitto_lib_init();

    if( rv != MOSQ_ERR_SUCCESS ){
        printf("err_1\n");
        mosquitto_lib_cleanup() ;	
		return 0;
    }
    

    mosq = mosquitto_new(mqtt.clientid, session, NULL) ;

    if(!mosq)
	{
        printf("err_2\n");
        mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		return 0;
	}

    rv = mosquitto_username_pw_set(mosq,mqtt.username,mqtt.passwd);

    if(rv != MOSQ_ERR_SUCCESS)
	{
        printf("err_3\n");
        mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		return 0;
    }   

    if( (rv = mosquitto_connect(mosq, mqtt.hostname, mqtt.port, 60)) != MOSQ_ERR_SUCCESS )
	{
        printf("err_4\n");
		mosquitto_destroy(mosq) ;
		mosquitto_lib_cleanup() ;
		return 0;
	}
    //mosquitto_publish(mosq,NULL,mqtt.topic,strlen(mqtt_message)+1,mqtt_message,mqtt.Qos,0) ;
    
    if( (rv =  mosquitto_subscribe(mosq, NULL, subtopic, 0)) != MOSQ_ERR_SUCCESS )
	{
        printf("err_sub\n");
		mosquitto_destroy(mosq) ;
		mosquitto_lib_cleanup() ;
		return 0;
	}

    mosquitto_message_callback_set(mosq, mqtt_message_callback);



    
	pthread_t id;
	pthread_create(&id, NULL,PUB_my_msg,NULL);
	pthread_detach(id);//设置分离属性

	while (1)
	{
		int loop = mosquitto_loop(mosq, -1, 1);

		if(loop != MOSQ_ERR_SUCCESS)
		{
			printf("err_5\n");
			mosquitto_destroy(mosq) ;
			mosquitto_lib_cleanup() ;
			return 0;
		}
		else 
		{
			printf("loop is good \n");
		}
		sleep(2);
		/* code */
	}
	

    
    mosquitto_destroy(mosq) ;
    mosquitto_lib_cleanup() ;
    return 0;
}


