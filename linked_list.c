#include <stdio.h>
#include <stdlib.h>

typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;

typedef struct Stack {
    StackNode* top;
} Stack;

Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) {
        printf("Error: Stack creation failed\n");
        return NULL;
    }
    stack->top = NULL;
    return stack;
}

int isEmpty(Stack* stack) {
    return stack->top == NULL;
}

void push(Stack* stack, int data) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (!newNode) {
        printf("Error: Stack overflow\n");
        return;
    }
    newNode->data = data;
    newNode->next = stack->top;
    stack->top = newNode;

    printf("Pushed %d to stack\n", data);
}

int pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Error: Stack underflow\n");
        return -1;
    }

    StackNode* temp = stack->top;
    int popped = temp->data;

    stack->top = stack->top->next;
    free(temp);

    printf("Popped %d from stack\n", popped);
    return popped;
}

int peek(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Error: Stack is empty\n");
        return -1;
    }
    return stack->top->data;
}

void displayStack(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return;
    }
    
    printf("Stack elements: ");
    StackNode* current = stack->top;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("\n");
}

void freeStack(Stack* stack) {
    StackNode* current = stack->top;
    StackNode* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    printf("Stack freed\n");
    free(stack);
}

// Queue implementation

typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        printf("Error: Queue creation failed\n");
        return NULL;
    }
    queue->front = queue->rear = NULL;
    return queue;
}

int isEmptyQueue(Queue* queue) {
    return queue->front == NULL;
}

void enqueue(Queue* queue, int data) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (isEmptyQueue(queue)) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    printf("Enqueued %d to queue\n", data);
}

int dequeue(Queue* queue) {
    if (isEmptyQueue(queue)) {
        printf("Error: Queue is empty\n");
        return -1;
    }

    QueueNode* temp = queue->front;
    int dequeued = temp->data;

    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);

    printf("Dequeued %d from queue\n", dequeued);
    return dequeued;
}

int front(Queue* queue) {
    if (isEmptyQueue(queue)) {
        printf("Error: Queue is empty\n");
        return -1;
    }
    return queue->front->data;
}

void displayQueue(Queue* queue) {
    if (isEmptyQueue(queue)) {
        printf("Queue is empty\n");
        return;
    }

    printf("Queue elements: ");
    QueueNode* current = queue->front;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("\n");
}

void freeQueue(Queue* queue) {
    QueueNode* current = queue->front;
    QueueNode* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    printf("Queue freed\n");
    free(queue);
}

int main() {
    printf("Stack implementation\n");
    Stack* stack = createStack();
    
    push(stack, 10);
    push(stack, 20);
    push(stack, 30);

    displayStack(stack);
    
    printf("top element: %d\n", peek(stack));
    printf("Popped element: %d\n", pop(stack));
    displayStack(stack);

    printf("Queue implementation\n");
    Queue* queue = createQueue();

    enqueue(queue, 10);
    enqueue(queue, 20);
    enqueue(queue, 30);

    displayQueue(queue);

    printf("Front element: %d\n", front(queue));
    printf("Dequeued element: %d\n", dequeue(queue));
    displayQueue(queue);

    freeStack(stack);
    freeQueue(queue);

    return 0;
}