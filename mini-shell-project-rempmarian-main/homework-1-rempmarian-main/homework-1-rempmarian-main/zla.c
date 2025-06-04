//Name: Marian Rempola
//PSU Email: mhr5220@psu
//PSU ID: 993254039

#include "zla.h"

// Function to create a ZeroLengthArray
ZeroLengthArray* create_zero_length_array(int num_elements) {
    // check if num_elements passed is valid
    if(num_elements <= 0) {
        perror("No elements were passed");
        return NULL;
    }

    // allocate memory for ZeroLengthArray struct + elements (size of int * num_elements)
    ZeroLengthArray* zla = (ZeroLengthArray*)malloc(sizeof(ZeroLengthArray) + num_elements * sizeof(int));

    // check if zla was initialized correctly or if number of elements passed is valid
    if (!zla) {
        perror("Failed to allocate memory");
        return NULL;
    }

    // store number of elements in the size field
    zla->size = num_elements;

    // return a pointer to the allocated structure
    return zla;
}

// Function to print the list
void print_zero_length_array(ZeroLengthArray* arr) {
    // check if arr is empty
    if (arr == NULL) {
        perror("Null pointer passed");
        return;
    }

    // check if size passed is valid
    if(arr->size < 0) {
        perror("Size in the array is invalid");
        return;
    }

    // following format from zla_output/out_expected
    printf("List elements (size = %d):\n", arr->size);
    // parse throught the array and print each data
    for (int i = 0; i < arr->size; i++) {
        // print each data 
        printf("%d ", arr->data[i]);
    }
    printf("\n");
}

// Function to set values in the array
void set_values(ZeroLengthArray* arr, int* values) {
    // check if arr is empty
    if (arr == NULL || values == NULL) {
        perror("Null pointer passed");
        return;
    }

    // parse through arr to set each data to its corresponding value
    for (int i = 0; i < arr->size; i++) {
        arr->data[i] = values[i];
    }
}

