

#include "mem.h"
#include "kernel.h"
#include "type.h"
#include "list.h"
#include "exec.h"

#define MAGIC 0xDEADBEEF

#define ALIGN(s,n) (n + ((uint64)n % s == 0 ? 0 : (s - (uint64)n % s)))

#define SIZE_THRESHOLD 64
#define SMALL_BLOCK_ENTRIES 64
#define FREE_BLOCKS = SMALL_BLOCK_ENTRIES / 8

#define BLOCK_FULL(n) ((n->freeBlocks & 0xFFFFFFFFFFFFFFFF) == 0xFFFFFFFFFFFFFFFF)
#define IS_BLOCK_SET(n, slot) (n->freeBlocks & (1 << slot))

typedef enum { MT_SMALL, MT_LARGE } MemType_t;

typedef struct {
	Node_t n;
	uint32 magic;
	size_t size;
	MemType_t type;
} MemNode_t;

typedef struct {
	MemNode_t n;
	size_t blockSize;
	uint64 freeBlocks;
} SmallMemNode_t;

#define HEADER_SIZE sizeof(MemNode_t)
#define SMALL_HEADER_SIZE sizeof(SmallMemNode_t)

#define NEXT_MEM_NODE(node) ((MemNode_t*)(((uint8*)node) + HEADER_SIZE + ALIGN(4,node->size)))
#define TO_MEM(node) (((uint8*)node) + HEADER_SIZE)
#define TO_SMALL_MEM(node, slot) (((uint8*)node) + SMALL_HEADER_SIZE + (node->blockSize * slot))
#define FROM_MEM(mem) ((MemNode_t*)((uint8*)mem - HEADER_SIZE))

MemNode_t* AllocLargeBlock(size_t size) {
	Kernel_t* k = GetKernel();

	MemNode_t* node = (MemNode_t*)k->mem.head;
	MemNode_t* curr = NULL;
	if( node == NULL ) {

		node = (MemNode_t*)k->heapBase;
		InitNode((Node_t*)node, 0, NULL);

	} else {

		MemNode_t* prev = (MemNode_t*)k->mem.head;
		curr = (MemNode_t*)prev->n.next; 
		node = NEXT_MEM_NODE(prev);
		
		while( curr != NULL && (node->type == MT_SMALL || NEXT_MEM_NODE(node) > curr) ) {

			prev = curr;
			curr = (MemNode_t*)curr->n.next;
			node = NEXT_MEM_NODE(prev);
		}
	}

	node->magic = MAGIC;
	node->size = size;
	node->type = MT_LARGE;

	Insert(&(k->mem), (Node_t*)node, (Node_t*)curr);

	if( k->currentTask != NULL ) {
		k->currentTask->memUsage += HEADER_SIZE + size + ALIGN(4,size);
	}

	return node;
}

void* AllocSmallBlock(size_t size) {

	Kernel_t* k = GetKernel();

	size_t blockSize = 8;
	while( blockSize < size ) {
		blockSize *= 2;
	}

	SmallMemNode_t* node = (SmallMemNode_t*)k->mem.head;
	if( node == NULL ) {

		node = (SmallMemNode_t*)k->heapBase;
		InitNode((Node_t*)node, 0, NULL);
		Insert(&(k->mem), (Node_t*)node, NULL);
		
		node->n.magic = MAGIC;
		node->n.type = MT_SMALL;
		node->blockSize = blockSize;
		node->freeBlocks = 1;
		return TO_SMALL_MEM(node, 0);
		
	}
		
	node = (SmallMemNode_t*)k->mem.head;
	while ( node != NULL && (node->n.type != MT_SMALL || node->blockSize != blockSize || BLOCK_FULL(node))) {
		node = (SmallMemNode_t*)(node->n.n.next);
	}

	if( node == NULL ) {
		node = (SmallMemNode_t*)AllocLargeBlock((sizeof(SmallMemNode_t) - sizeof(MemNode_t)) 
							+ blockSize * SMALL_BLOCK_ENTRIES);
		if( node == NULL ) {
			return NULL;
		}
	 	node->n.type = MT_SMALL;
		node->blockSize = blockSize;
		node->freeBlocks = 1;
		return TO_SMALL_MEM(node, 0);
	} else {
		uint64 pos = 1;
		int slot = 0;
		for( slot = 0; slot < SMALL_BLOCK_ENTRIES; slot++ ) {
			if( (node->freeBlocks & pos) == 0 ) {
				break;
			}
			pos <<= 1;
		}
		node->freeBlocks |= pos;
		return TO_SMALL_MEM(node, slot);
	}

	if( k->currentTask != NULL ) {
		k->currentTask->memUsage += blockSize;
	}
}

void* AllocMem(size_t size) {
	if( size <= SIZE_THRESHOLD ) {
		return AllocSmallBlock(size);
	}
	return TO_MEM(AllocLargeBlock(size));
}

void FreeMem(void* mem) {

	Kernel_t* k = GetKernel();
	MemNode_t* node = FROM_MEM(mem);
	if( node->magic == MAGIC && node->type == MT_LARGE ) {
		RemoveNode(&(k->mem), (Node_t*)node);
		if( k->currentTask != NULL ) {
			k->currentTask->memUsage -= HEADER_SIZE + node->size + ALIGN(4, node->size);
		}
	} else {
		uint32* m = (uint32*)ALIGN(4,(uint8*)mem);
		while( *m != MAGIC ) {
			m--;
			if( m < (uint32*)&(k->mem.head) ) {
				return;
			}
		}
		SmallMemNode_t* snode = (SmallMemNode_t*)(((uint8*)m) - sizeof(Node_t));
		uint64 slot = ((((uint64)mem) - ((((uint64)snode)) + sizeof(SmallMemNode_t)))) / snode->blockSize;
		snode->freeBlocks &= ~(1 << slot);
		if( k->currentTask != NULL ) {
			k->currentTask->memUsage -= snode->blockSize;
		}
	}
}


