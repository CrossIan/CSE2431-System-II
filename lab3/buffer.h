//
//  buffer.h
//  CSE2431
//
//  Created by iqbal hasnan and Dylan Frost on 7/4/13.
//

#ifndef buffer_h
#define buffer_h

typedef int buffer_item;
#define BUFFER_SIZE 8

void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */

int insert_item(buffer_item item);
int remove_item(buffer_item *item);
#endif

