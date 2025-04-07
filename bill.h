#ifndef BILL_H
#define BILL_H

#include <stdio.h> 

//Structs

//split phone number into its components
typedef struct {
    int areaCode;
    int exchange;
    int line;
} PhoneNumber;


//store dates of le calls
typedef struct {
    int year;
    int month;
    int day;
} Date;


//single phone call record, includes previous 2 structs
typedef struct {
    Date date;
    int time;
    PhoneNumber number;
    int duration;
    float cost;
} CallRecord;


//za customer info
typedef struct {
    char name[100];
    char address1[100];
    char address2[100];
    CallRecord *calls;
    int count;
    int capacity;
} Bill;

//Function stuff

// init the bill struct
void init_bill(Bill *bill);

//free up za bill memory
void free_bill(Bill *bill);

//read za bill file and all its stuffs
void read_bill(const char *filename, Bill *bill);

//format za bill as neeeded
void display_bill(FILE *out, Bill *bill);

//well what if i wanna know abt mah bill? Prints average duration and cost for every month in the bill and le yearly total
void summarize_bill(FILE *out, Bill *bill);

//well what if i only want a monthly summary? Here ya go 
void summarize_month(FILE *out, Bill *bill, int month);

//what if i wanna know mah total average cost
void average_cost(FILE *out, Bill *bill);

//what if I wanna know my average duration
void average_duration(FILE *out, Bill *bill);

//name to month conversion
int convert_month_by_name(const char *month);

//number to month conversion
const char* convert_month_by_number(int month);

//ADD MY RECORD PLZ
void add_record(Bill *bill, CallRecord record);

#endif
