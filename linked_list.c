#include <string.h>
#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <unistd.h>
#include <stdio.h> //printf()
#include <stdlib.h>
#include <sys/types.h> //pid_t
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

 
/*
	Function: add_NewNode
	Arguments:
		head: the head node of linked list you would like to add to
		new_pid: process id of new node
		new_path: file path of new node
	Purpose: Adds new node with given proccess id and path to the start of the given linked list
	Returns: Updated head for the linked list 
*/
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	Node* new_node = (Node*)malloc(sizeof(Node));
	new_node->pid = new_pid;
	new_node->path = new_path;
	new_node->next = head;
	head = new_node;

	return head;
}

/*
	Function: deleteNode
	Arguments:
		head: the head node of linked list you would like to delete from
		pid: process id of node you would like to delete
	Purpose: Deletes node with given process id from the given linked list
	Returns: Updated head for the linked list 
*/
Node * deleteNode(Node* head, pid_t pid){
	Node* current = head;
	Node* previous = NULL;

	if (!PifExist(head, pid)) {
		return head;
	}

	while (current != NULL) {
		if (current->pid == pid) {
			if (current == head) {
				head = head->next;
				free(current);
				current = head;
			}
			else {
				previous->next = current->next;
				free(current);
				current = previous->next;
			}
		}
		else {
			previous = current;
			current = current->next;
		}
	}

	return head;
}

/*
	Function: printList
	Arguments:
		node: node in the linked list you would like to start printing the list from
	Purpose: Prints linked list - from the given node to the end of the list
*/
void printList(Node *node){
	Node* current = node;
	while (current != NULL) {
		printf("%d: %s\n", current->pid, current->path);
		current = current->next;
	}
}

/*
	Function: PifExist
	Arguments:
		node: the head node of the linked list you would like to search through
		pid: process id of node you want to find
	Purpose: Checks if pid exists by searching through the given linked list to find node with given process id
	Returns: 1 if pid exists and 0 if pid does not exist
*/
int PifExist(Node *node, pid_t pid){
	Node* current = node;
	while (current != NULL) {
		if (current->pid == pid) {
			return 1;
		}
		current = current->next;
	}
  	return 0;
}

