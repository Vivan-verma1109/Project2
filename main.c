#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bill.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <commands.txt> <output.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");

    if (!in || !out) {
        fprintf(stderr, "Could't open output'.\n");
        return EXIT_FAILURE;
    }

    char line[100]; //just some set amt
    Bill bill;
    init_bill(&bill);

    while (fgets(line, sizeof(line), in)) {
        // Skip stuffs
        if (line[0] == '#' || line[0] == '\n' || isspace(line[0])) continue;

        // Remove the trailing thing
        line[strcspn(line, "\n")] = '\0';

        // load stuff
        if (strncmp(line, "load", 4) == 0) {
            char filename[100];
            if (sscanf(line, "load %s", filename) == 1) {
                fprintf(out, "Command: load %s\n", filename);
                read_bill(filename, &bill);
                fprintf(out, "    Loaded %d records from %s\n", bill.count, filename);
            }
        }

        //summarize le month
        else if (strncmp(line, "summarize ", 10) == 0) {
            char month[32];
            if (sscanf(line, "summarize %s", month) == 1) {
                int month_num = convert_month_by_name(month);
                fprintf(out, "Command: summarize %s\n", month);
                summarize_month(out, &bill, month_num);
            }
        }

        // summarize le all
        else if (strcmp(line, "summarize") == 0) {
            fprintf(out, "Command: summarize\n");
            summarize_bill(out, &bill);
        }

        // le duration average
        else if (strcmp(line, "average duration") == 0) {
            fprintf(out, "Command: average duration\n");
            average_duration(out, &bill);
        }

        // le cost average
        else if (strcmp(line, "average cost") == 0) {
            fprintf(out, "Command: average cost\n");
            average_cost(out, &bill);
        }

        // display za studf
        else if (strcmp(line, "display") == 0) {
            fprintf(out, "Command: display\n");
            display_bill(out, &bill);
        }
    }

    free_bill(&bill);
    fclose(in);
    fclose(out);
    return EXIT_SUCCESS;
}
