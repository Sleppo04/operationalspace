#include "arraylist.h"

/// @brief Initializes a new arraylist at the supplied memory block.
/// @param list Memory address where the list shall be initialized
/// @return NOERROR, EINVAL, ENOMEM
int ArrayListNew(arraylist_t* list)
{
    return ArrayListNewCapacity(list, ARRAYLIST_INITIAL_SIZE);
}

/// @brief Initialize an arraylist with a given capacity in the supplied memory block.
/// @param list Memory block where the list shall be initialized
/// @param capacity Initial capacity of the new list
/// @return NOERROR, EINVAL, ENOMEM
int ArrayListNewCapacity(arraylist_t* list, size_t capacity)
{
    if (list == NULL) {
        return EINVAL;
    }
    void** array = (void**) (malloc(sizeof(void*) * capacity));
    if (array == NULL) {
        return ENOMEM;
    }
    list->size = 0;
    list->array = array;
    list->capacity = capacity;
    return 0;
}

/// @brief Copy elements from original to copy.
/// All elements previously in copy are overwritten
/// @param source list from which elements are copied
/// @param destination list to which elements are copied
/// @return NOERROR, EINVAL, EINVAL, ENOMEM
int ArrayListCopy(arraylist_t* source, arraylist_t* destination)
/*
* Copies the arraylist to new memory block, changes in the original
* list will not be reflected in the copy.
* The elements of the list are not copied because of their unknown nature.
* If you want to pass a element-copy-function, use ArrayListCopy2.
*/
{
    if (source == NULL) {
        return EINVAL;
    }
    if (destination == NULL) {
        return EINVAL;
    }
    
    destination->size     = source->size;
    destination->capacity = source->size;
    destination->array    = malloc(sizeof(void*) * destination->capacity);
    if (destination->array == NULL) {
        return ENOMEM;
    }
    memcpy(destination->array, source->array, source->size * sizeof(void*));

    return NOERROR;
}


/// @brief Places all the return values of copyElementFunc(element) into copy for each element in original
/// Old elements of copy are removed
/// @param source
/// @param destination
/// @param copyElementFunc 
/// @return NOERROR, EINVAL, EINVAL
int ArrayListCopy2(arraylist_t* source, arraylist_t* destination, void* (*copyElementFunc) (void*, size_t))
/* Notes on ArrayListCopy also apply here
* void* (*copyElementFunc) (void*, size_t) will be passed every single 
* element of the list as well as it's index in order.
* It is expected to return a void pointer which is then stored in the new
* arraylist instead of the old element.
*/
{
    if (source == NULL) {
        return EINVAL;
    }
    if (destination == NULL) {
        return EINVAL;
    }
    
    destination->size     = source->size;
    destination->capacity = source->size;
    destination->array    = malloc(sizeof(void*) * destination->capacity);
    if (destination->array == NULL) {
        return ENOMEM;
    }

    for (size_t idx = 0; idx < destination->size; idx++) {
        arrayListSet(destination, idx, copyElementFunc(destination->array[idx], idx));
    }

    return 0;
}

/// @brief Resize list to newCapacity
/// @param list list to resize
/// @param newCapacity capacita to resize to
/// @return NOERROR, EINVAL, ENOMEM
int arrayListResize(arraylist_t* list, size_t newCapacity)
/*
Resizes arraylist_t* list to a new capacity of size_t newCapacity
In the case where size > newCapacity, all elements with index >= newCapacity
are omitted and not contained afterwards. Size will be changed accordingly.
*/
{
    if (list == NULL) {
        return EINVAL;
    }

    void** newArray = realloc(list->array, newCapacity * sizeof(void*));
    if (newArray == NULL) {
        return ENOMEM;
    }

    list->array    = newArray;
    list->capacity = newCapacity;
    list->size     = min(list->size, newCapacity);
    return NOERROR;
}

/// @brief deconstructor for the arraylist_t struct
/// @param list list to destroy
/// @return NOERROR, EINVAL
int ArrayListDestroy(arraylist_t* list)
/*
Frees list->array, but not the list itself, because the struct memory management
is user-side defined.
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    free(list->array);
    return NOERROR;
}

/// @brief append element to the end of list
/// @param list list to which element will be appended
/// @param element element to append
/// @return NOERROR, EINVAL, ENOMEM
int arrayListAppend(arraylist_t* list, void* element)
/*
Appends void* element to arraylist_t*, appended elements are at
the end of the list.
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    if (list->size >= list->capacity) {
        size_t newCapacity = max(list->capacity * 2UL, 1UL);
        int resizedStatus = arrayListResize(list, newCapacity);
        switch (resizedStatus)
        {
        case EINVAL:
            return EINVAL;
        case ENOMEM:
            return ENOMEM;
        }
    }
    
    list->array[list->size] = element;
    list->size++;
    return NOERROR;
}

/// @brief retrieves the value at position index from list and writes to element 
/// @param list list to get a value from
/// @param index position at which to get the value 
/// @param element where to write the value to
/// @return NOERROR, EINVAL, EADDRNOTAVAIL, EINVAL
int arrayListGet(arraylist_t* list, size_t index, void** element)
{
    if (list == NULL) {
        return EINVAL;
    }
    if (index >= list->size) {
        return EADDRNOTAVAIL;
    }
    if (element == NULL) {
        return EINVAL;
    }

    element[0] = list->array[index];
    return NOERROR;
}

/// @brief Set the array at position index to the value element
/// @param list list in which to set a value
/// @param index position where to set a value
/// @param element value to set
/// @return NOERROR, EINVAL, EADDRNOTAVAIL
int arrayListSet(arraylist_t* list, size_t index, void* element)
/*
Replaces the element at size_t index in arraylist_t* list with void* element
Does not free replaced elements
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    if (index >= list->size) {
        return EADDRNOTAVAIL;
    }

    list->array[index] = element;
    return 0;
}


/// @brief removes the element at index out of list
/// elements further in the back will be moved to the front
/// @param list list in which to remove the element at index
/// @param index index at which to remove an element
/// @return NOERROR, EINVAL, EADDRNOTAVAIL
int arrayListRemove(arraylist_t* list, size_t index)
/*
Removes the element at size_t index out of arraylist_t* list.
Does not free removed pointers.
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    if (index >= list->size) {
        return EADDRNOTAVAIL;
    }

    void** destination = list->array + index;
    void** source      = destination + 1;
    size_t size        = (list->size  - index) * sizeof(void*);
    memmove(destination, source, size);
    list->size--;
    return NOERROR;
}

/// @brief Resizes the capacity of the list to fit to it's size, but at least 1
/// @param list list to resize
/// @return NOERROR, EINVAL, ENOMEM
int arrayListFitToSize(arraylist_t* list)
/*
Because realloc(ptr, 0) differs fron platform to platform, the new capacity will be set to at least 1.
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    return arrayListResize(list, max(list->size, 1U));
}

/// @brief inserts element at index into list, all elements from index on are shifted backwards
/// @param list list where element shall be inserted
/// @param index index at which to insert element
/// @param element element to insert
/// @return NOERROR, EINVAL, EADDRNOTAVAIL, ENOMEM
int arrayListInsert(arraylist_t* list, size_t index, void* element)
/*
Inserts void* element to arraylist_t* list at size_t index.
The element previously at index will be moved back.
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    if (index > list->size) {
        return EADDRNOTAVAIL;
    }
    if (list->size == list->capacity) {
        size_t newCapacity = max(list->capacity * 2UL, 1UL);
        int status = arrayListResize(list, newCapacity);
        if (status == 1) {
            return EINVAL;
        }
        if (status == 3) {
            return ENOMEM;
        }
    }

    void** source      = list->array + index;
    void** destination = source + 1;
    size_t len         = (list->capacity - index) * sizeof(void*);
    memmove(destination, source, len);
    list->array[index] = element;
    list->size++;
    return 0;
}

/// @brief Removes all elements from list
/// @param list list to clear
/// @return NOERROR, EINVAL
int arrayListClear(arraylist_t* list)
/*
Removes all elements from arraylist_t* list.
Does not free any memory, to reduce the memory usage,
take a look at arrayListFitToSize.
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    list->size = 0UL;
    return NOERROR;
}

/// @brief checks whether list contains element
/// @param list list in which to search
/// @param element element to find in list
/// @param isContained pointer where the truth value should be written to
/// @return NOERROR, EINVAL, EINVAL
int arrayListContains(arraylist_t* list, void* element, bool* isContained)
/// Writes $(element in list) to isContained 
{
    if (list == NULL) {
        return EINVAL;
    }
    if (isContained == NULL) {
        return EINVAL;
    }

    void* listElement;
    for (size_t index = 0; index < list->size; index++) {
        arrayListGet(list, index, &listElement);
        if (listElement == element) {
            isContained[0] = true;
            return (int) 0;
        }
    }
    return (int) 0;
}

/// @brief Write the first position of find in list to index
/// @param list list in which to search
/// @param find element to find
/// @param index pointer where to write the position to
/// @return EINVAL, EINVAL
int arrayListIndexOf(arraylist_t* list, void* find, size_t* index)
/* 
* Writes the index of void* find in arraylist_t* list to size_t* index
* if the element is not in the list, size_t* index is left unchanged
*/
{
    if (list == NULL) {
        return EINVAL;
    }
    if (index == NULL) {
        return EINVAL;
    }

    void* item;
    for (size_t i = 0; i < list->size; i++) {
        arrayListGet(list, i, &item);
        if (item == find) {
            index[0] = 0;
            return 0;
        }
    }

    return 0;
}

/// @brief Replace first occurrence of target in list with replacement
/// @param list the list in which to replace target
/// @param target target element to replace
/// @param replacement what target will be replaced with
/// @return NOERROR, EINVAL, EINVAL, EADDRNOTAVAIL
int arrayListReplace(arraylist_t* list, void* target, void* replacement)
/// EINVAL will be returned, if the target is not contained in the list
{
    if (list == NULL) {
        return EINVAL;
    }
    size_t index = list->capacity;
    arrayListIndexOf(list, target, &index);
    // No interesting error codes returned here
    if (index == list->capacity) { // list->capacity would be out of bounds
        return EINVAL;
    }
    int rc = arrayListSet(list, index, replacement);
    // list-pointer can't get invalid in-between function calls
    if (rc == EADDRNOTAVAIL) {
        return EADDRNOTAVAIL;
    }

    return NOERROR;
}

int arrayListExtend(arraylist_t* destination, arraylist_t* source)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (source == NULL) {
        return EINVAL;
    }

    if (destination->capacity < source->size + destination->size) {
        size_t new_capacity = source->size + destination->size;
        if (arrayListResize(destination, new_capacity)) {
            return ENOMEM;
        }
    }

    void* destination_region = destination->array + destination->size;
    void* copy_start = source->array;
    size_t copy_size = sizeof(void*) * source->size;

    memcpy(destination_region, copy_start, copy_size);

    destination->size += source->size;

    return NOERROR;
}
