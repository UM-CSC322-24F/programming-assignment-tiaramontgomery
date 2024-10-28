#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE_OF_STORAGE 120
#define LENGTH_OF_NAME 127
#define LEGNTH_OF_FILE 255

#define SLIPRATE 12.5
#define LANDRATE 14
#define TRAILORRATE 25
#define STORAGERATE 11

typedef enum {
    slip, 
    land, 
    trailor, 
    storage,
} PlaceType;

typedef union {
    int dock;
    char landLetter;
    char trailorLicense[7];
    int storageNumber;
} TypeInfo;

typedef struct Boat {
    char name[LENGTH_OF_NAME];
    float feet;
    PlaceType place;
    TypeInfo extra;
    float totalOwed;
} Boat;

Boat * array[SIZE_OF_STORAGE];
int totalSize;

int comparison(const void * boat1, const void * boat2) {
    const Boat * boat_1 = * (const Boat **)boat1;
    const Boat * boat_2 = * (const Boat **)boat2;

    return strcmp(boat_1->name, boat_2->name);
}

void load(const char * files) {
    FILE * file = fopen(files, "r");
    if (!file) {
        perror("Error: exiting");
        exit(EXIT_FAILURE);
    }
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        Boat * boat = (Boat *)malloc(sizeof(Boat));
        char placeString[10];

        sscanf(line, "%[^,],%f,%[^,],%[^,],%f", boat->name, &boat->feet, 
        placeString, boat->extra.trailorLicense, &boat->totalOwed);
            if (strcmp(placeString, "slip") == 0) {
                boat->place = slip;
                boat->extra.dock = atoi(boat->extra.trailorLicense);
            } else if (strcmp(placeString, "land") == 0) {
                boat->place = land;
                boat->extra.landLetter = boat->extra.trailorLicense[0];
            } else if (strcmp(placeString, "trailor") == 0) {
                boat->place = trailor;
            } else if (strcmp(placeString, "storage") == 0) {
                boat->place = storage;
                boat->extra.storageNumber = atoi(boat->extra.trailorLicense);
            }
            array[totalSize++] = boat;
        }
        fclose(file);
    qsort(array, totalSize, sizeof(Boat*), comparison);
}

// writing the Boat data from the boats array to a CSV file
void save(const char * files) {
    FILE * file = fopen(files, "w");
    if (!file) {
        perror("Error: exiting");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < totalSize; i++) {
        Boat * boat = array[i];
        switch (boat->place) {
            case slip:
                fprintf(file, "%s,%.0f,slip,%d,%.2f\n", boat->name, boat->feet, 
                boat->extra.dock, boat->totalOwed);
                break;
            case land:
                fprintf(file, "%s,%.0f,land,%c,%.2f\n", boat->name, boat->feet, 
                boat->extra.landLetter, boat->totalOwed);
                break;
            case trailor:
                fprintf(file, "%s,%.0f,trailor,%s,%.2f\n", boat->name, 
                boat->feet, boat->extra.trailorLicense, boat->totalOwed);
                break;
            case storage:
                fprintf(file, "%s,%.0f,storage,%d,%.2f\n", boat->name, 
                boat->feet, boat->extra.storageNumber, boat->totalOwed);
                break;
        }
    }
    fclose(file);
}

//Printing inventory
void printB() {
    printf("%-20s %-20s %-20s %-20s %-20s\n", " ", " ", " ", " ", " ");
    for (int i = 0; i < totalSize; i++) {
        Boat * boat = array[i];
        printf("%-20s %-20.0f ", boat->name, boat->feet);
    switch (boat->place) {
        case slip:
            printf("%-20s #%-20d ", "Slip", boat->extra.dock);
            break;
        case land:
            printf("%-20s #%-20c ", "Land", boat->extra.landLetter);
            break;
        case trailor:
            printf("%-20s %-20s ", "Trailor", boat->extra.trailorLicense);
            break;
        case storage:
            printf("%-20s #%-20d ", "Storage", boat->extra.storageNumber);
            break;
        }
    printf("Owes: %-20.2f\n", boat->totalOwed);
    }
}

//Adding a boat via a string 
void newBoat(const char * csvData) {
    if (totalSize == SIZE_OF_STORAGE) {
        printf("Error: maximum number of boats reached\n");
        return;
    }

    Boat *boat = (Boat *)malloc(sizeof(Boat));
    char placeString[10];

    sscanf(csvData, "%[^,],%f,%[^,],%[^,],%f", boat->name, &boat->feet, 
    placeString, boat->extra.trailorLicense, &boat->totalOwed);

    if (strcmp(placeString, "slip") == 0) {
        boat->place = slip;
        boat->extra.dock = atoi(boat->extra.trailorLicense);
    } else if (strcmp(placeString, "land") == 0) {
        boat->place = land;
        boat->extra.landLetter = boat->extra.trailorLicense[0];
    } else if (strcmp(placeString, "trailor") == 0) {
        boat->place = trailor;
    } else if (strcmp(placeString, "storage") == 0) {
        boat->place = storage;
        boat->extra.storageNumber = atoi(boat->extra.trailorLicense);
    }
        array[totalSize++] = boat;
}

void boatRemoval(const char * boatName) {
    for (int i = 0; i < totalSize; i++) {
        if (strcasecmp(array[i]->name, boatName) == 0) {
            free(array[i]);
            for (int j = i; j < totalSize - 1; j++) {
                array[j] = array[j + 1];
            }
            totalSize--;
            return;
        }
    }
    printf("No boat with that name\n");
}


//Accepting payments
void acceptingPayments(const char * boatName, float payment) {
    for (int i = 0; i < totalSize; i++) {
        if (strcasecmp(array[i]->name, boatName) == 0) {
            if (payment <= array[i]->totalOwed) {
                array[i]->totalOwed -= payment;
            } else {
                printf("That is more than the amount owed, %s: %.2f\n", 
                array[i]->name, array[i]->totalOwed);
            }
            return;
        }
    }
    printf("No boat with that name.\n");
}

void freeBoat() {
    for (int i = 0; i < totalSize; i++) {
        free(array[i]);
        array[i] = NULL;
    }
    totalSize = 0;
}

void updatedMonthlyAmount (float month) {
    float rate;
    int i;
    for (i = 0; i<totalSize; i++) {
        Boat *boat = array[i];
        switch(boat->place) {
            case slip:
                array[i]->totalOwed += (array[i]->feet * (month*SLIPRATE));
                break;
            case land:
                array[i]->totalOwed += (array[i]->feet * (month*LANDRATE));
                break;
            case trailor:
                array[i]->totalOwed += (array[i]->feet * (month*TRAILORRATE));
                break;
            case storage:
                array[i]->totalOwed += (array[i]->feet * (month*STORAGERATE));
                break;
        }
    }
}

//Boat Management Menu
void menu() {
    char choice;
    char value[1024];
    
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------\n");
    while (1) {
        printf("\n");
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        scanf(" %c", &choice);
        while (getchar() != '\n'); 
        switch (tolower(choice)) {
            case 'i': 
                printB();
                break;
            case 'a':
                printf("Please enter the boat data in CSV format: ");
                fgets(value, sizeof(value), stdin);
                value[strcspn(value, "\n")] = 0; 
                newBoat(value);
                break;
            case 'r':
                printf("Please enter the boat name: ");
                fgets(value, sizeof(value), stdin);
                value[strcspn(value, "\n")] = 0; 
                boatRemoval(value);
                break;
            case 'p':
                    printf("Please enter boat name: ");
                    fgets(value, sizeof(value), stdin);
                    value[strcspn(value, "\n")] = 0; 
                    printf("Please enter the amount to be payed: ");
                    float payment;
                    scanf("%f", &payment);
                    while (getchar() != '\n'); 
                    acceptingPayments(value, payment);
                break;
            case 'm': {
                    float month;
                    updatedMonthlyAmount(month);
                }
                break;
            case 'x':
                printf("Exiting the Boat Management System");
                return;
            default:
                printf("Invalid option\n");
        }
    }
}

int main() {
    load("BoatData.csv");
    qsort(array, totalSize, sizeof(Boat *), comparison);
    menu();
    save("BoatData.csv");
    freeBoat();
    return 0;
}
