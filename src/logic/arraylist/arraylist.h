#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef max
#define max(a, b) (a > b ? a : b)
#endif

#ifndef min
#define min(a, b) (a < b ? a : b)
#endif


#define NOERROR EXIT_SUCCESS
#define ARRAYLIST_INITIAL_SIZE 10U

typedef struct ArrayList {
	size_t size;
	size_t capacity;
	void** array;
} arraylist_t;

/// @brief Initializes a new arraylist at the supplied memory block.
/// @param list Memory address where the list shall be initialized
/// @return NOERROR, EINVAL, ENOMEM
int ArrayListNew(arraylist_t* list);

/// @brief Initialize an arraylist with a given capacity in the supplied memory block.
/// @param list Memory block where the list shall be initialized
/// @param capacity Initial capacity of the new list
/// @return NOERROR, EINVAL, ENOMEM
int ArrayListNewCapacity(arraylist_t* list, size_t capacity);

/// @brief Copy elements from original to copy.
/// All elements previously in copy are overwritten
/// @param source list from which elements are copied
/// @param destination list to which elements are copied
/// @return NOERROR, EINVAL, EINVAL, ENOMEM
int ArrayListCopy(arraylist_t* original, arraylist_t* copy);
/*
* Copies the arraylist to new memory block, changes in the original
* list will not be reflected in the copy.
* The elements of the list are not copied because of their unknown nature.
* If you want to pass a element-copy-function, use ArrayListCopy2.
*/

/// @brief Places all the return values of copyElementFunc(element) into copy for each element in original
/// Old elements of copy are removed
/// @param source
/// @param destination
/// @param copyElementFunc 
/// @return NOERROR, EINVAL, EINVAL
int ArrayListCopy2(arraylist_t* original, arraylist_t* copy, void* (*copyElementFunc) (void*, size_t));
/* Notes on ArrayListCopy also apply here
* void* (*copyElementFunc) (void*, size_t) will be passed every single 
* element of the list as well as it's index in order.
* It is expected to return a void pointer which is then stored in the new
* arraylist instead of the old element.
*/

/// @brief Resize list to newCapacity
/// @param list list to resize
/// @param newCapacity capacita to resize to
/// @return NOERROR, EINVAL, ENOMEM
int arrayListResize(arraylist_t* list, size_t newCapacity);
/*
Resizes arraylist_t* list to a new capacity of size_t newCapacity
In the case where size > newCapacity, all elements with index >= newCapacity
are omitted and not contained afterwards. Size will be changed accordingly.
*/

/// @brief deconstructor for the arraylist_t struct
/// @param list list to destroy
/// @return NOERROR, EINVAL
int ArrayListDestroy(arraylist_t* list);
/*
Frees list->array, but not the list itself, because the struct memory management
is user-side defined.
*/

/// @brief append element to the end of list
/// @param list list to which element will be appended
/// @param element element to append
/// @return NOERROR, EINVAL, ENOMEM
int arrayListAppend(arraylist_t* list, void* element);
/*
Appends void* element to arraylist_t*, appended elements are at
the end of the list.
*/

/// @brief retrieves the value at position index from list and writes to element 
/// @param list list to get a value from
/// @param index position at which to get the value 
/// @param element where to write the value to
/// @return NOERROR, EINVAL, EADDRNOTAVAIL, EINVAL
int arrayListGet(arraylist_t* list, size_t index, void** element);

/// @brief Set the array at position index to the value element
/// @param list list in which to set a value
/// @param index position where to set a value
/// @param element value to set
/// @return NOERROR, EINVAL, EADDRNOTAVAIL
int arrayListSet(arraylist_t* list, size_t index, void* element);
/*
Replaces the element at size_t index in arraylist_t* list with void* element
Does not free replaced elements
*/

/// @brief removes the element at index out of list
/// elements further in the back will be moved to the front
/// @param list list in which to remove the element at index
/// @param index index at which to remove an element
/// @return NOERROR, EINVAL, EADDRNOTAVAIL
int arrayListRemove(arraylist_t* list, size_t index);
/*
Removes the element at size_t index out of arraylist_t* list.
Does not free removed pointers.
*/

/// @brief Resizes the capacity of the list to fit to it's size, but at least 1
/// @param list list to resize
/// @return NOERROR, EINVAL, ENOMEM
int arrayListFitToSize(arraylist_t* list);
/*
Because realloc(ptr, 0) differs fron platform to platform, the new capacity will be set to at least 1.
*/

/// @brief inserts element at index into list, all elements from index on are shifted backwards
/// @param list list where element shall be inserted
/// @param index index at which to insert element
/// @param element element to insert
/// @return NOERROR, EINVAL, EADDRNOTAVAIL, ENOMEM
int arrayListInsert(arraylist_t* list, size_t index, void* element);
/*
Inserts void* element to arraylist_t* list at size_t index.
The element previously at index will be moved back.
*/

/// @brief Removes all elements from list
/// @param list list to clear
/// @return NOERROR, EINVAL
int arrayListClear(arraylist_t* list);
/*
Removes all elements from arraylist_t* list.
Does not free any memory, to reduce the memory usage,
take a look at arrayListFitToSize.
*/

/// @brief checks whether list contains element
/// @param list list in which to search
/// @param element element to find in list
/// @param isContained pointer where the truth value should be written to
/// @return NOERROR, EINVAL, EINVAL
int arrayListContains(arraylist_t* list, void* element, bool* isContained);
/// Writes $(element in list) to isContained 

/// @brief Write the first position of find in list to index
/// @param list list in which to search
/// @param find element to find
/// @param index pointer where to write the position to
/// @return EINVAL, EINVAL
int arrayListIndexOf(arraylist_t* list, void* find, size_t* index);
/* 
* Writes the index of void* find in arraylist_t* list to size_t* index
* if the element is not in the list, size_t* index is left unchanged
*/

/// @brief Replace first occurrence of target in list with replacement
/// @param list the list in which to replace target
/// @param target target element to replace
/// @param replacement what target will be replaced with
/// @return NOERROR, EINVAL, EINVAL, EADDRNOTAVAIL
int arrayListReplace(arraylist_t* list, void* target, void* replacement);
/// EINVAL will be returned, if the target is not contained in the list


/// @brief extend the destination arraylist with all the elements from source
/// @param destination The elements of source will be appended to this list
/// @param source The elements of this list will be copied over to destination
/// @return NOERROR, EDESTADDRREQ, EINVAL, ENOMEM
int arrayListExtend(arraylist_t* destination, arraylist_t* source);

#endif // ARRAYLIST_H
