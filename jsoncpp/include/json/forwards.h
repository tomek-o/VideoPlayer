#ifndef JSON_FORWARDS_H_INCLUDED
# define JSON_FORWARDS_H_INCLUDED

# include "config.h"

namespace Json {

   // writer.h
   class FastWriter;
   class StyledWriter;

   // reader.h
   class Reader;


   // value.h
   typedef unsigned int ArrayIndex;
   class StaticString;
   class Path;
   class PathArgument;
   class Value;
   class ValueIteratorBase;
   class ValueIterator;
   class ValueConstIterator;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
   class ValueMapAllocator;
   class ValueInternalLink;
   class ValueInternalArray;
   class ValueInternalMap;
#endif // #ifdef JSON_VALUE_USE_INTERNAL_MAP

} // namespace Json


#endif // JSON_FORWARDS_H_INCLUDED
