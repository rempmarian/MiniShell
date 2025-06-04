//Name: Marian Rempola
//PSU Email: mhr5220@psu.edu
//PSU ID: 993254039

#include "linked_list.h"

// Function to create a new node
Node* createNode(int data) {
    // create memory for a new node with the size of Node
    Node* newNode = (Node*) malloc(sizeof(Node)); 

    // check to see if new node is emptY
    if (!newNode) {
        perror("Failed to allocate memory");
        return NULL;
    } 

    // assign new node to be data, the next and previous ones to be Null
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

// Insert at the beginning
void insertAtBeginning(Node** head, int data) {
    // create new node 
    Node* newNode = createNode(data);

    // assign new node's next data to be at the head so that the first element is the newNode
    newNode->next = *head;
    // if the start of the list is still not empty, keep pushing the element back
    if (*head != NULL) {
        (*head)->prev = newNode;
    }
    // assign the start of the list to be the newNOde
    *head = newNode;
}

// Insert at the end
void insertAtEnd(Node** head, int data) {
    // create new node
    Node* newNode = createNode(data);

    // 1. check if there's anything before the new node
    if (*head == NULL) {
        *head = newNode;
        return;
    }

    // 2. if there's something already in the list, add it after the last element
    // keep track of starting point
    Node* lastNode = *head; 
    // find empty spot (when next pointer is NULL)
    while (lastNode->next != NULL) {
        // update last node to the next value
        lastNode = lastNode->next;
    }
    // assign next node to be the new node
    lastNode->next = newNode;
    // the last node will now be the previous node
    newNode->prev = lastNode;
}

// Delete a node by value
void deleteNode(Node** head, int value) {
    // check if list is empty
    if (!head) {
        perror("List is emtpy, nothing to delete");
        return;
    }

    // parse thru the list to see which node should be deleted
    Node* point = *head;

    while( point != NULL &&point->data != value) {
        point = point->next;
    }

    // check if point pointer is null, which will mean the value was not found in the list
    if (point == NULL) {
        perror("value is not in the list");
        return;
    }

    // check to see if value to be deleted is at the first, middle, or last node
    // 1. value is the first node
    if (point == *head) { 
        *head = point->next; // start of the list should be the next element
        if(*head != NULL) {
            (*head)->prev = NULL; // because it's the first node now
        }
    } 
    // 2. value is the last node
    else if (point->next == NULL) { 
        // the last node should now be the previous node
        point->prev->next = NULL; // ptr to the next node of the ptr (of value) to be deleted
    } 
    // 3. value is in the middle of the list
    else {
        point->prev->next = point->next; // ptr to the prev node's next ptr
        point->next->prev = point->prev; // ptr to the next node's previous ptr
    }

    // free node after deletion 
    free(point); 
}

// Display the list forward
void displayForward(Node* head) {
    // check if list is empty
    if (!head) {
        perror("List is empty");
        return;
    }

    // start at the first node
    Node* point = head;
    printf("List (Forward): ");
    // parse through the list and print each node
    while(point != NULL) {
        printf("%d ", point->data);
        point = point->next;
    }
    printf("\n");
}

// Display the list backward
void displayBackward(Node* head) {
    // check if list is empty
    if (!head) {
        perror("List is empty");
        return;
    }

    // start the last node
    Node* point = head;
    // go to last node
    while(point->next != NULL) {
        point = point->next;
    }

    // print from back of the list
    printf("List (Backward): ");
    while(point != NULL) {
        printf("%d ", point->data);
        point = point->prev;
    }
    printf("\n");
}

