#ifndef PARENT_H
#define PARENT_H

#include <iostream>
#include <semaphore.h>

void ParentRoutine(const char* pathToChild1, const char* pathToChild2, std::istream& input);

#endif
