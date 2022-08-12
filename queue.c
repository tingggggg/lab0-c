#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

#define min(x, y) ((x) < (y) ? (x) : (y))

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l) {}

static element_t *ele_malloc(const char *s)
{
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return NULL;
    
    int s_len = sizeof(char) * strlen(s) + 1;
    ele->value = malloc(s_len);
    if (!ele->value) {
        free(ele);
        return NULL;
    }
    strncpy(ele->value, s, s_len);
    return ele;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{   
    if (!head)
        return false;

    element_t *ele = ele_malloc(s);
    if (!ele)
        return false;

    list_add(&(ele->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = ele_malloc(s);
    if (!ele)
        return false;

    list_add_tail(&(ele->list), head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{   
    element_t *node = NULL;
    if (!head || list_empty(head))
        return node;

    node = list_first_entry(head, element_t, list);
    list_del(&(node->list));

    if (sp) {
        int len = strlen(node->value) + 1;
        len = min(bufsize, len);
        strncpy(sp, node->value, len);
        sp[len - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue 
* Return target element.
* Return NULL if queue is NULL or empty.
* If sp is non-NULL and an element is removed, copy the removed string to *sp
* (up to a maximum of bufszie characters, plus a null terminator.)
*/
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *node = NULL;
    if (!head || list_empty(head))
        return node;
    
    node = list_last_entry(head, element_t, list);
    list_del(&(node->list));

    if (sp) {
        int len = strlen(node->value) + 1;
        len = min(len, bufsize);
        strncpy(sp, node->value, len);
        sp[len - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue
* Return 0 if head is NULL or empty.
*/
int q_size(struct list_head *head)
{   
    if (!head || list_empty(head))
        return 0;
    struct list_head *node;
    int  size = 0;
    list_for_each(node, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}
