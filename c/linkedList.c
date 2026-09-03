#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defining a node
typedef struct Node {
    char* data;
    struct Node* next;
    struct Node* prev;
} Node;

// Create a heap-allocated copy of a string
char* copyString(const char* source)
{
    char* copy = malloc(strlen(source) + 1);

    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, source);
    return copy;
}

// Create a new node containing a heap-allocated string
Node* createNode(const char* data)
{
    Node* newNode = malloc(sizeof(Node));

    if (newNode == NULL) {
        return NULL;
    }

    newNode->data = copyString(data);

    if (newNode->data == NULL) {
        free(newNode);
        return NULL;
    }

    newNode->next = NULL;
    newNode->prev = NULL;

    return newNode;
}

// Insert a node at the beginning
int insertAtBeginning(Node** head, const char* data)
{
    Node* newNode = createNode(data);

    if (newNode == NULL) {
        return 1;
    }

    // Check if the doubly linked list is empty
    if (*head == NULL) {
        *head = newNode;
        return 1;
    }

    newNode->next = *head;
    (*head)->prev = newNode;
    *head = newNode;
    
    return 0;
}

// Insert a node at the end
int insertAtEnd(Node** head, const char* data)
{
    Node* newNode = createNode(data);

    if (newNode == NULL) {
        return 1;
    }

    // Check if the doubly linked list is empty
    if (*head == NULL) {
        *head = newNode;
        return 1;
    }

    Node* temp = *head;

    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = newNode;
    newNode->prev = temp;

    return 0;
}

// Insert a node at a specified position
void insertAtPosition(Node** head, const char* data, int position)
{
    if (position < 1) {
        printf("Position should be >= 1.\n");
        return;
    }

    if (position == 1) {
        insertAtBeginning(head, data);
        return;
    }

    Node* newNode = createNode(data);
    Node* temp = *head;

    for (int i = 1; temp != NULL && i < position - 1; i++) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf(
            "Position greater than the number of nodes.\n");
        return;
    }

    newNode->next = temp->next;
    newNode->prev = temp;

    if (temp->next != NULL) {
        temp->next->prev = newNode;
    }
    temp->next = newNode;
}

// Find the first node containing the specified string
Node* findNode(Node* head, const char* data)
{
    Node* temp = head;

    while (temp != NULL) {
        if (strcmp(temp->data, data) == 0) {
            return temp;
        }

        temp = temp->next;
    }

    return NULL;
}

// Delete a node from the beginning
void deleteAtBeginning(Node** head)
{
    if (*head == NULL) {
        printf("The list is already empty.\n");
        return;
    }

    Node* temp = *head;
    *head = (*head)->next;

    if (*head != NULL) {
        (*head)->prev = NULL;
    }

    // Free both the string and the node
    free(temp->data);
    free(temp);
}

// Delete a node from the end
void deleteAtEnd(Node** head)
{
    if (*head == NULL) {
        printf("The list is already empty.\n");
        return;
    }

    Node* temp = *head;

    if (temp->next == NULL) {
        *head = NULL;
        free(temp->data);
        free(temp);
        return;
    }

    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->prev->next = NULL;

    free(temp->data);
    free(temp);
}

// Delete a node from a specified position
void deleteAtPosition(Node** head, int position)
{
    if (*head == NULL) {
        printf("The list is already empty.\n");
        return;
    }

    if (position < 1) {
        printf("Position should be at least 1.\n");
        return;
    }

    if (position == 1) {
        deleteAtBeginning(head);
        return;
    }

    Node* temp = *head;

    for (int i = 1; temp != NULL && i < position; i++) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Position is greater than the number of nodes.\n");
        return;
    }

    if (temp->next != NULL) {
        temp->next->prev = temp->prev;
    }

    if (temp->prev != NULL) {
        temp->prev->next = temp->next;
    }

    free(temp->data);
    free(temp);
}


// Print the list in the forward direction
void printListForward(Node* head)
{
    Node* temp = head;

    printf("Forward List: ");

    while (temp != NULL) {
        printf("%s ", temp->data);
        temp = temp->next;
    }

    printf("\n");
}

// Print the list in the reverse direction
void printListReverse(Node* head)
{
    Node* temp = head;

    if (temp == NULL) {
        printf("The list is empty.\n");
        return;
    }

    // Move to the end of the list
    while (temp->next != NULL) {
        temp = temp->next;
    }

    // Traverse backward
    printf("Reverse List: ");

    while (temp != NULL) {
        printf("%s ", temp->data);
        temp = temp->prev;
    }

    printf("\n");
}

// Free every remaining node and string
void freeList(Node** head)
{
    Node* current = *head;

    while (current != NULL) {
        Node* next = current->next;

        free(current->data);
        free(current);

        current = next;
    }

    *head = NULL;
}

int main(void)
{
    printf("Hello, world!\n\n");

    Node* head = NULL;

    // Test insertion functions
    insertAtEnd(&head, "5");
    insertAtEnd(&head, "10");
    insertAtBeginning(&head, "20");
    insertAtPosition(&head, "30", 2);

    // List: 20, 30, 5, 10
    printf("After insertions:\n");
    printListForward(head);
    printListReverse(head);

    // Test finding an existing string
    Node* result = findNode(head, "5");

    if (result != NULL) {
        printf("\nFound: %s\n", result->data);
    } else {
        printf("\n5 was not found.\n");
    }

    // Test finding a nonexistent string
    result = findNode(head, "40");

    if (result != NULL) {
        printf("Found: %s\n", result->data);
    } else {
        printf("40 was not found.\n");
    }

    // Test deletion functions
    deleteAtBeginning(&head);
    deleteAtEnd(&head);

    printf("\nAfter deletions:\n");
    printListForward(head);
    printListReverse(head);

    // Release any nodes that remain
    freeList(&head);

    return 0;
}