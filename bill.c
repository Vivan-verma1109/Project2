#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bill.h"


const char* convert_month(int month) {
    static const char *months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    return months[month - 1];
}   

// init the bill struct
void init_bill(Bill *bill) {
    bill->count = 0;
    bill->capacity = 10;
    bill->calls = (CallRecord *)malloc(sizeof(CallRecord) * bill->capacity);

    if (bill->calls == NULL) {
        fprintf(stderr, "malloc failed during init phase\n");
        exit(EXIT_FAILURE);//basically return -1
    }
}

// free up za bill memory
void free_bill(Bill *bill) {
    if (bill->calls != NULL) {
        free(bill->calls);
        bill->calls = NULL;
    }
    bill->count = 0;
    bill->capacity = 0;
}

// read za bill file and all its stuffs
void read_bill(const char *filename, Bill *bill) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "could not open file %s\n", filename);
        exit(EXIT_FAILURE);//basically return -1
    }

    // Read le customer info
    fgets(bill->name, sizeof(bill->name), file);
    fgets(bill->address1, sizeof(bill->address1), file);
    fgets(bill->address2, sizeof(bill->address2), file);

    bill->name[strcspn(bill->name, "\n")] = '\0';
    bill->address1[strcspn(bill->address1, "\n")] = '\0';
    bill->address2[strcspn(bill->address2, "\n")] = '\0';

    char line[256];

    // Skip stuff
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r'){ 
            continue;
        }
        if (strchr(line, '-') != NULL && strstr(line, "Date") == NULL){ 
            break;
        }
    }

    // Read za records
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r'){
            continue;
        }

        CallRecord rec;
        unsigned int phone;

        if (sscanf(line, "%4d%2d%2d %4d %d %d %f",
                &rec.date.year,
                &rec.date.month,
                &rec.date.day,
                &rec.time,
                &phone,
                &rec.duration,
                &rec.cost) == 7) {

            // componentiate (berak into components)
            rec.number.areaCode = phone / 10000000;
            rec.number.exchange = (phone / 10000) % 1000;
            rec.number.line = phone % 10000;

            add_record(bill, rec);
        }
    }
    fclose(file);
}

// format za bill as neeeded
void display_bill(FILE *out, Bill *bill) {
    fprintf(out, "%s\n", bill->name);
    fprintf(out, "%s\n", bill->address1);
    fprintf(out, "%s\n\n", bill->address2); 

    fprintf(out, "Date        Time    Number      Drtn    Cost\n");
    fprintf(out, "-----------------------------------------------------\n");

    for (int i = 0; i < bill->count; i++) {
        CallRecord *rec = &bill->calls[i];

        fprintf(out, "%04d%02d%02d    %04d    %03d%03d%04d  %-4d%.2f\n",
            rec->date.year,
            rec->date.month,
            rec->date.day,
            rec->time,
            rec->number.areaCode,
            rec->number.exchange,
            rec->number.line,
            rec->duration,
            rec->cost
        );

    }
}

// well what if i wanna know abt mah bill? Prints average duration and cost for every month in the bill and le yearly total
void summarize_bill(FILE *out, Bill *bill) {
    // Monthly
    for (int m = 1; m <= 12; m++) {
        summarize_month(out, bill, m);
    }

    // Yearly
    int total_calls = bill->count;
    int total_duration = 0;
    float total_cost = 0.0;

    for (int i = 0; i < bill->count; i++) {
        total_duration += bill->calls[i].duration;
        total_cost += bill->calls[i].cost;
    }

    float avg_duration;
    if (total_calls) {
        avg_duration = (float)total_duration / total_calls;
    } else {
        avg_duration = 0.0;
    }

    float avg_cost;
    if (total_calls) {
        avg_cost = total_cost / total_calls;
    } else {
        avg_cost = 0.0;
    }
    
    int year;
    if (bill->count > 0) {
        year = bill->calls[0].date.year;
    } else {
        year = 0;
    }

    fprintf(out, "      Year %d:    Total %d    Average Duration: %.4f   Average Cost: %.6f\n",
        year,
        total_calls,
        avg_duration,
        avg_cost);
}

// well what if i only want a monthly summary? Here ya go 
void summarize_month(FILE *out, Bill *bill, int month) {
    int total_calls = 0;
    int total_duration = 0;
    float total_cost = 0.0;

    for (int i = 0; i < bill->count; i++) {
        if (bill->calls[i].date.month == month) {
            total_calls++;
            total_duration += bill->calls[i].duration;
            total_cost += bill->calls[i].cost;
        }
    }

    const char *month_name = convert_month(month);

    if (total_calls == 0) {
        fprintf(out, "   %12s:    Total 0 Average Duration: 0     Average Cost: 0\n", month_name);

    } else {
        float avg_duration = (float)total_duration / total_calls;
        float avg_cost = total_cost / total_calls;
        fprintf(out, "   %12s:    Total %d Average Duration: %.4f   Average Cost: %.2f\n",
            month_name,
            total_calls,
            avg_duration,
            avg_cost);
    }
}
// what if i wanna know mah total average cost
void average_cost(FILE *out, Bill *bill) {
    if (bill->count == 0) {
        fprintf(out, "        Average Cost: 0\n");
        return;
    }

    float total = 0;
    for (int i = 0; i < bill->count; i++) {
        total += bill->calls[i].cost;
    }

    float avg = total / bill->count;
    fprintf(out, "        Average Cost: %.2f\n", avg);
}

// what if I wanna know my average duration
void average_duration(FILE *out, Bill *bill) {
    if (bill->count == 0) {
        fprintf(out, "        Average Duration: 0\n");
        return;
    }
    int total = 0;
    for (int i = 0; i < bill->count; i++) {
        total += bill->calls[i].duration;
    }
    float avg = (float)total / bill->count; //don't want to do integer division so float convert
    fprintf(out, "        Average Duration: %.4f\n", avg);
}

// name to month conversion
int convert_month_by_name(const char *month) {
    const char *months[] = { //THe months are conssitent
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    for (int i = 0; i < 12; i++) {
        if (strcasecmp(month, months[i]) == 0) { //case insensitive
            return i + 1; // 1-12
        }
    }
    return -1;
}

// ADD MY RECORD PLZ
void add_record(Bill *bill, CallRecord record) {
    //Resize if needed
    if (bill->count == bill->capacity) {
        bill->capacity *= 2;
        CallRecord *temp = realloc(bill->calls, sizeof(CallRecord) * bill->capacity);
        if (temp == NULL) {
            fprintf(stderr, "realloc failed in recording\n");
            exit(EXIT_FAILURE); //basically return -1
        }
        bill->calls = temp;
    }
    //Find za insert position
    int i = bill->count - 1;
    while (i >= 0) {
        CallRecord current = bill->calls[i];
        // Compare stuffs
        if (current.date.year > record.date.year ||
            (current.date.year == record.date.year && current.date.month > record.date.month) ||
            (current.date.year == record.date.year && current.date.month == record.date.month && current.date.day > record.date.day) ||
            (current.date.year == record.date.year && current.date.month == record.date.month && current.date.day == record.date.day && current.time > record.time)) {
            bill->calls[i + 1] = bill->calls[i]; // Shift right
            i--;
        } else {
            break;
        }
    }
    // Insert that 
    bill->calls[i + 1] = record;
    bill->count++;
}



