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
void q_free(struct list_head *l) 
{
    if (!l)
        return;
    element_t *curr, *safe;
    list_for_each_entry_safe(curr, safe, l, list) {
        list_del(&(curr->list));
        q_release_element(curr);
    }
    free(l);
}

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
    if (!head || list_empty(head))
        return false;
    struct list_head *front, *back;
    front = head->next;
    back = head->prev;
    while (front != back && front->next != back) {
        front = front->next;
        back = back->prev;
    }

    element_t *node = list_entry(back, element_t, list);
    list_del(back);
    q_release_element(node);
    return true;
}

/* Delete all nodes that have duplicate string 
* Node: this function always be called after sorting, in other words,
* list is guranteed to be sorted in ascending order.
*/
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    if (head->prev == head->next)
        return true;
    struct list_head **indir = &head->next;
    while (*indir != head && (*indir)->next != head) {
        char *a = list_entry(*indir, element_t, list)-> value;
        char *b = list_entry((*indir)->next, element_t, list)->value;
        if (strcmp(a, b) == 0) {
            struct list_head *tmp = (*indir)->next;
            do {
                list_del(tmp);
                q_release_element(list_entry(tmp, element_t, list));
                tmp = (*indir)->next;
            } while (tmp != head &&
                     strcmp(a, list_entry(tmp, element_t, list)->value) == 0);
            tmp = *indir;
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));        
        } else {
            indir = &(*indir)->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    
    struct list_head **indir = &head->next;
    while (*indir != head && (*indir)->next != head) {
        struct list_head **sec = &(*indir)->next;
        // do next swap
        *indir = *sec;
        *sec = (*indir)->next;
        (*indir)->next = (*indir)->prev;

        // do prev swap
        sec = &(*sec)->prev;
        *sec = (*indir)->prev;
        (*indir)->prev = (*indir)->prev->prev;
        (*sec)->prev = *indir;

        indir = &(*sec)->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) 
{
    if (!head || list_empty(head) || head->next == head->prev)
        return;
    struct list_head *curr = head;
    do {
        struct list_head *tmp = curr->next;
        curr->next = curr->prev;
        curr->prev = tmp;
        curr = curr->prev;
    } while (curr != head);
}

static void merge_list(struct list_head *l1,
                       struct list_head *l2,
                       struct list_head **phead,
                       size_t cnt)
{
    struct list_head *ptr = *phead;

    while (l1 != *phead && l2 != *phead) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) <= 0) {
            ptr->next = l1;
            l1->prev = ptr;
            l1 = l1->next;
        } else {
            ptr->next = l2;
            l2->prev = ptr;
            l2 = l2->next;
        }
        ptr = ptr->next;
    }

    ptr->next = (l1 != (*phead)) ? l1 : l2;
    ptr->next->prev = ptr;

    if (cnt == 1) {
        while (ptr->next != (*phead))
            ptr = ptr->next;
        (*phead)->prev = ptr;
    }
}

static struct list_head *mergesort_list(struct list_head *node,
                                        struct list_head **phead,
                                        size_t cnt)
{
    if (!(*phead) || node->next == *phead)
        return node;

    struct list_head *left, *right;
    struct list_head *slow = node;
    struct list_head *fast = node->next->next;
    struct list_head *mid = NULL;
    while (fast != *phead && fast->next != *phead) {
        slow = slow->next;
        fast = fast->next->next;
    }

    mid = slow->next;
    slow->next = *phead;

    cnt++;
    left = mergesort_list(node, phead, cnt);
    right = mergesort_list(mid, phead, cnt);
    merge_list(left, right, phead, cnt);
    return (*phead)->next;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) 
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    size_t cnt = 0;
    struct list_head **phead = &head;
    (*phead)->next = mergesort_list((*phead)->next, phead, cnt);
    (*phead)->next->prev = (*phead);
}
