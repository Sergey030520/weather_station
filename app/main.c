#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

#include <FXRTOS.h>
#include "demo_bsp.h"
#include "main.h"

#define SIZE_LOG_TEMP 20


struct LogTemperature{
	float temp;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
};

fx_mutex_t record_mtx;
fx_timer_t timer_record, timer_input_console;
fx_mem_pool_t pool;
fx_msgq_t messageQueue;

//struct LogTemperature tempLogs[20];
uintptr_t tempLogs[SIZE_LOG_TEMP];


void showDate(RTC_DateTypeDef* date){
	printf("Year: %u, Month: %u, Day: %u \r\n",  date->Year, date->Date, date->Month);
}
void showTime(RTC_TimeTypeDef* time){
	printf("Hours: %u, Minutes: %u, Seconds: %u \r\n",  time->Hours, time->Minutes, time->Seconds);
}

void showTemp(float temp){
	printf("Temperature: %.3f \r\n", temp);
}
void showTimestampWithTemp(struct LogTemperature* log){
	printf("Date: %u-%u-%u %u:%u:%u temperature: %.3f \r\n",  log->date.Year, log->date.Month, log->date.WeekDay,
			log->time.Hours, log->time.Minutes, log->time.Seconds, log->temp);
}
void showTimestamp(RTC_DateTypeDef* date, RTC_TimeTypeDef* time){
	printf("Date: %u-%u-%u %u:%u:%u\r\n",  date->Year, date->Date, date->Month,
				time->Hours, time->Minutes, time->Seconds);
}

void fx_intr_handler(void)
{
    ;
}

bool isDigitLine(char* line, bool delimetr){
	for(int ind = 0; ind < strlen(line); ++ind){
		if(delimetr && (ind == 2 || ind == 5)){
			continue;
		}else if(!isdigit((unsigned char)*(line+ind))){
			return false;
		}
	}
	return true;
}


bool parse_time(char* line, RTC_TimeTypeDef* time){
	bool isDelimetr = strlen(line) == 8;
	if(strlen(line) < 6 || !isDigitLine(line, isDelimetr)){
		return false;
	}else if(strlen(line) < 8 || !isDigitLine(line, isDelimetr)){
		return false;
	}

	uint8_t sec = (((uint8_t)*(line+0))-0x30)*10 + (((uint8_t)*(line+1))-0x30);
	uint8_t minutes;
	uint8_t hours;
	if(isDelimetr){
		minutes = (((uint8_t)*(line+3))-0x30)*10 + (((uint8_t)*(line+4))-0x30);
		hours = (((uint8_t)*(line+6))-0x30)*10 + (((uint8_t)*(line+7))-0x30);
	}else{
		minutes = (((uint8_t)*(line+2))-0x30)*10 + (((uint8_t)*(line+3))-0x30);
		hours = (((uint8_t)*(line+4))-0x30)*10 + (((uint8_t)*(line+5))-0x30);
	}


	if(sec > 60 || minutes > 60 || hours > 24){
		return false;
	}

	time->Seconds = sec;
	time->Minutes = minutes;
	time->Hours = hours;

	return true;
}

enum Month{
	MONTH_JANUARY = 1,
	MONTH_FEBRUARY,
	MONTH_MARCH,
	MONTH_APRIL,
	MONTH_MAY,
	MONTH_JUNE,
	MONTH_JULY,
	MONTH_AUGUST,
	MONTH_SEPTEMBER,
	MONTH_OCTOBER,
	MONTH_NOVEMBER,
	MONTH_DECEMBER
};

enum DayInMonth{
	DAYS_MONTH_31 = 31,
	DAYS_MONTH_30 = 30,
	DAYS_MONTH_28 = 28
};
bool checkDayInMonth(uint8_t year, uint8_t month, uint8_t day){
	if(month == MONTH_JANUARY || month == MONTH_MARCH || month == MONTH_MAY ||month == MONTH_JULY || month ==MONTH_AUGUST ||
			month == MONTH_OCTOBER || month ==MONTH_DECEMBER){
		return DAYS_MONTH_31 >= day;
	}else if(month == MONTH_FEBRUARY){
		return DAYS_MONTH_28 >= day;
	}else if(month == MONTH_APRIL || month == MONTH_JUNE || month == MONTH_SEPTEMBER || month == MONTH_NOVEMBER){
		return DAYS_MONTH_30 >= day;
	}
	return false;
}
bool parse_date(char* line, RTC_DateTypeDef* date){
	bool isDelimetr = (strlen(line) == 8);
	if((strlen(line) < 6) || !isDigitLine(line, isDelimetr)){
		return false;
	}
	else if((strlen(line) < 8) || !isDigitLine(line, isDelimetr)){
		return false;
	}

	uint8_t day = (((uint8_t)*(line+0))-0x30)*10 + (((uint8_t)*(line+1))-0x30);
	uint8_t month = 0;
	uint8_t year = 0;

	if(isDelimetr){
		month = (((uint8_t)*(line+3))-0x30)*10 + (((uint8_t)*(line+4))-0x30);
		year = (((uint8_t)*(line+6))-0x30)*10 + (((uint8_t)*(line+7))-0x30);
	}else{
		month = (((uint8_t)*(line+2))-0x30)*10 + (((uint8_t)*(line+3))-0x30);
		year = (((uint8_t)*(line+4))-0x30)*10 + (((uint8_t)*(line+5))-0x30);
	}

	if(month > 12 || !checkDayInMonth(year, month, day)){
		return false;
	}

	date->WeekDay = day;
	date->Month = month;
	date->Year = year;

	return true;
}

void read_command(char* command){
	RTC_TimeTypeDef time = {0};
	RTC_DateTypeDef date = {0};
	float temp;

	fx_mutex_acquire(&record_mtx, NULL);
	get_time(&time);
	get_date(&date);
	temp = get_temp();
	fx_mutex_release(&record_mtx);

	printf("Command: %s\r\n", command);

	if(strcmp(command,"set_time") == 0){
		char line[8];
		printf("Input new time: \r\n");
		scanf("%s", line);
		printf("\r\n");

		printf("Input time: %s\r\n", line);

		if(parse_time(line, &time)){
			set_time(&time);
		}else{
			printf("time incorrect!\r\n");
		}
	}else if(strcmp(command,"set_date") == 0){
		char line[8];
		printf("Input new date: \r\n");
		scanf("%s", line);
		printf("\r\n");
		if(parse_date(line, &date)){
			set_date(&date);
		}else{
			printf("time incorrect!\r\n");
		}

	}else if(strcmp(command,"h") == 0 || strcmp(command,"hours") == 0){
		printf("Hours: %u\r\n", time.Hours);
	}else if(strcmp(command,"minutes") == 0){
		printf("Minutes: %u\r\n", time.Minutes);
	}else if(strcmp(command,"s") == 0 || strcmp(command,"seconds")== 0){
		printf("Seconds: %u\r\n", time.Seconds);
	}else if(strcmp(command,"y") == 0 || strcmp(command,"year") == 0){
		printf("Year: %u\r\n", date.Year);
	}else if(strcmp(command,"month") == 0 || strcmp(command,"m") == 0){
		printf("Month: %u\r\n", date.Month);
	}else if(strcmp(command,"d") == 0 || strcmp(command,"day") == 0){
		printf("Day: %u\r\n", date.WeekDay);
	}else if(strcmp(command, "time") == 0){
		showTime(&time);
	}else if(strcmp(command,"date") == 0){
		showDate(&date);
	}else if(strcmp(command,"timestamp") == 0){
		showTimestamp(&date, &time);
	}else if(strcmp(command,"temperature") == 0){
		showTemp(temp);
	}else if(strcmp(command,"list_temp") == 0){

		struct LogTemperature* logTemp;
		fx_mutex_acquire(&record_mtx, NULL);

		while(fx_msgq_timedreceive(&messageQueue, (uintptr_t*)&logTemp, 0) == 0){
			showTimestampWithTemp(logTemp);
			fx_mem_pool_free(&pool, logTemp);
		};

		fx_mutex_release(&record_mtx);


	}else if(strcmp(command,"clear") == 0){
		struct LogTemperature* logTemp;
		fx_mutex_acquire(&record_mtx, NULL);
		while(fx_msgq_timedreceive(&messageQueue, (uintptr_t*)&logTemp, 0) == 0){
			fx_mem_pool_free(&pool, logTemp);
		};

		fx_mutex_release(&record_mtx);
	}else{
		printf("Incorrect command!\r\n");
	}
}

void read_thread(){
	char command[11];
	do{
		scanf("%s", command);
		printf("\r\n");
		read_command(command);
	}while(strcmp(command,"stop") != 0);
	fx_thread_suspend();
}

int start_record(void* argument){
	RTC_TimeTypeDef time = {0};
	RTC_DateTypeDef date = {0};
	float temp;

	fx_mutex_acquire(&record_mtx, NULL);

	struct LogTemperature* newLogTemp;
	fx_mem_pool_alloc(&pool, sizeof(struct  LogTemperature), (void*)&newLogTemp);

	get_time(&time);
	get_date(&date);
	temp = get_temp();

	newLogTemp->date.Year = date.Year;
	newLogTemp->date.Month = date.Month;
	newLogTemp->date.WeekDay = date.WeekDay;
	newLogTemp->time.Seconds = time.Seconds;
	newLogTemp->time.Minutes = time.Minutes;
	newLogTemp->time.Hours = time.Hours;
	newLogTemp->temp = temp;


	while(fx_msgq_back_timedsend(&messageQueue, (uintptr_t) newLogTemp, 0) != FX_MSGQ_OK){
		struct LogTemperature* log_temp;
		fx_msgq_receive(&messageQueue, (uintptr_t*)&log_temp, NULL);
		fx_mem_pool_free(&pool, log_temp);
	}


	fx_mutex_release(&record_mtx);


	return 1;
}
void write_thread(){
	fx_timer_init(&timer_record, start_record, 0);
	fx_timer_set_abs(&timer_record, 3000, 10000);
	fx_thread_suspend();
}


char data[1024];

void fx_app_init(void)
{
    static fx_thread_t t[2];
    static int thread_stk[2][2548 / sizeof(int)];


	fx_mutex_init(&record_mtx, FX_MUTEX_CEILING_DISABLED, FX_SYNC_POLICY_FIFO);

	fx_mem_pool_init(&pool);
	fx_mem_pool_add_mem(&pool, (uintptr_t)data, 1024);

    fx_sched_state_t prev;
    fx_sched_lock(&prev);



    fx_msgq_init(&messageQueue, tempLogs, SIZE_LOG_TEMP, FX_SYNC_POLICY_FIFO);

    fx_thread_init(&t[0], write_thread, 0, 10, thread_stk[0],
        	                sizeof(thread_stk[0]), false);
    fx_thread_init(&t[1], read_thread, 0, 10, thread_stk[1],
            	                sizeof(thread_stk[0]), false);


    fx_sched_unlock(prev);
}


int main(void)
{
    //
    // Hardware modules initialization
    //
    core_init();
    led_init();
    setvbuf(stdin, NULL, _IONBF, 1);
    setvbuf(stdout, NULL, _IONBF, 1);
    console_init();
    temp_sens_init();
    date_time_init();


    printf("-------Start Engine!------\r\n");


    fx_kernel_entry();



    while(1){

    }
}
