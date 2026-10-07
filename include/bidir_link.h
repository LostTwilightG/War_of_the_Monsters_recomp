#ifndef BIDIR_LINK_H
#define BIDIR_LINK_H

/* Intrusive doubly linked list node (retail: BidirLink). */
struct BidirLink {
    BidirLink *prev;
    BidirLink *next;

    void init(void);
};

#endif
