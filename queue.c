#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l) {
        // if head exists, clean the queue.
        element_t *iterator, *next;
        list_for_each_entry_safe (iterator, next, l, list) {
            list_del(&iterator->list);
            q_release_element(iterator);
        }
        free(l);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;  // the first node of inserted list_head is NULL
    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;  // no memory space for `new`
    int s_len = strlen(s) + 1;
    new->value = (char *) malloc(s_len * sizeof(char));
    if (!new->value) {
        free(new);
        return false;  // no memory space for `new->value`
    }
    memcpy(new->value, s, s_len);  // insert value
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;  // the first node of inserted list_head is NULL
    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;  // no memory space for `new`
    int s_len = strlen(s) + 1;
    new->value = (char *) malloc(s_len * sizeof(char));
    if (!new->value) {
        free(new);
        return false;  // no memory space for `new->value`
    }
    memcpy(new->value, s, s_len);  // insert value
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;  // `head` is NULL, or there's no list in `head`
    element_t *remove = list_first_entry(head, element_t, list);
    list_del(&remove->list);
    if (sp) {
        memcpy(sp, remove->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;  // `head` is NULL, or there's no list in `head`
    element_t *remove = list_last_entry(head, element_t, list);
    list_del(&remove->list);
    if (sp) {
        memcpy(sp, remove->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int size = 0;
    struct list_head *p;
    list_for_each (p, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;  // `head` is NULL, or there's no list in `head`
    /*if the foreward pointer hits the backward pointer, then they're in the
     * middle of the list*/
    struct list_head *foreward = head->next, *backward = head->prev;
    for (; foreward != backward && foreward->next != backward;
         foreward = foreward->next, backward = backward->prev)
        ;
    list_del(foreward);
    q_release_element(container_of(foreward, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;  // `head` is NULL, or there's no list in `head`
    element_t *iterator, *next;
    /*note that the list is sorted*/
    list_for_each_entry_safe (iterator, next, head, list) {
        if (&next->list != head && !strcmp(iterator->value, next->value)) {
            do {
                element_t *next_to_safe =
                    list_entry(next->list.next, element_t, list);
                list_del(&next->list);
                q_release_element(next);
                next = next_to_safe;
            } while (&next->list != head &&
                     !strcmp(iterator->value, next->value));
            list_del(&iterator->list);
            q_release_element(iterator);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;  // `head` is NULL, or there's no list in `head`
    struct list_head *curr = head->next, *next = curr->next;
    for (; curr != head && next != head; curr = curr->next, next = curr->next)
        list_move(curr, next);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;  // `head` is NULL, or there's no list in `head`
    struct list_head *iterator, *next;
    /*move each item the iterator points to to the head*/
    list_for_each_safe (iterator, next, head)
        list_move(iterator, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;  // `head` is NULL, or there's no list in `head`
    struct list_head *iterator, *next, *start = head;
    struct list_head *dummy =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!dummy)
        return;             // no space for dummy
    INIT_LIST_HEAD(dummy);  // pointer dummy serve as same as pointer head does
    int i = 0;
    list_for_each_safe (iterator, next, head) {
        i++;
        if (i < k)
            continue;
        list_cut_position(dummy, start,
                          iterator);  // cut k node of the list out as an
                                      // independent list to be reverse
        q_reverse(dummy);
        list_splice_init(dummy, start);  // take dummy back to the original list
        start = next->prev;
    }
    list_del(dummy);  // free the memory space
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
