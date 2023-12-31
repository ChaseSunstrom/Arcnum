#ifndef CORE_COLLECTION_H
#define CORE_COLLECTION_H

// ===============================================================================
// COLLECTION HELPER METHODS:

// Casts to a void* vector because every collection WILL have a size variable
#define COLLECTION_SIZE(_x) (((vector(void*))_x)->size)
#define COLLECTION_DATA(_x) (((vector(void*))_x)->data)

// ===============================================================================

#endif //CORE_COLLECTION_H