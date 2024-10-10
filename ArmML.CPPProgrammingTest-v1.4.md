
# Arm ML programming test.

The following contains the outline of the *problem*, the details of the *task* to do, as well as *sensible assumption* and *desirable properties*. All of these form the programming test.


## The problem:

The speed at which memory for an object can be allocated and deallocated using
either malloc/free or new/delete can often pose a problem. Both new/delete and
malloc/free are general purpose allocators that typically have to:

- Handle arbitrary allocation sizes, in any order; 
- Minimise any resulting memory fragmentation;
- Make the best use of operating system features such as virtual memory;
- (Amongst other requirements).

This complexity can make them under-perform simpler schemes for some common
usage patterns. This is often the case for relatively small objects that are
frequently individually allocated/deallocated. 

A real world example of this would be a computational geometry algorithm that
has to regularly modify a half-edged planar graph structure. Such an algorithm
would most likely have to regularly create and delete individual vertices,
half-edges, edges and faces in an unpredictable fashion. This memory usage
pattern is the case we will be concerned with in this test. We will not need to
support contiguous array allocation.

A common technique for improving memory efficiency for this case is to create
'pools' of memory for the frequently allocated objects in advance. The
frequently allocated objects can then be quickly retrieved/returned from/to the
pool when they are allocated/deallocated. In addition, because all individual
'allocations' within the pool are the same size the pool will not suffer from
any memory fragmentation.

A single pool would handle this form of basic memory management for just one
class of object, so you would create a separate pool for each type object you
are concerned with. Since each pool only has to handle the
allocation/deallocation of small fixed-sized amounts of memory, it can be
implemented to run considerably quicker than a general purpose allocator.


## The task

In the interests of code re-use it is desirable to have a generic system for
performing pooling.

The task is to design and implement a generic system for pooling an arbitrary
class of object. It's expected that this pool system will have to implement
just 4 key operations:

 - "Create a pool"
 - "Allocate an object from this pool"
 - "Deallocate an object from this pool"
 - "Destroy this pool"

These 4 functions are the focus for the implementation and you are free to
implement these 4 operations in any way you see fit. No other functionality is
required. 

The interface you present to your system has similar importance, and again you
are free to decide how best to present and structure your code.

For example, your system could be a class, a set of global functions, a pure
virtual interface, a template class, or something else. If you attend an
interview you may be asked to justify your choice.

The system should consist of cleanly presented portable code, complete with
comments, in one or more header (.h) and source (.cpp) files. It's expected
that this code compiles without errors, and any unit tests you provide run
successfully.

The purpose of this test is primarily to exercise your code design skills,
rather than seeing if you can correctly handle all error cases. For this reason
we make a number of simplifications to the problem below. A good solution will
satisfy as many of the *desirable properties* (also listed below) as possible. 


## Sensible assumptions

It is sufficient that the classes of objects to be pooled are known at
compile-time. To help you, some example classes to be pooled are given in a
separate ExampleClasses.h file.

For the purpose of this test we will assume that the maximum number of objects
that may be allocated in each pool is known in advance so you do not need to
allocate or delete any memory at any time except when the pools are created and
deleted. You can assume that any allocation calls you do make when creating a
pool will succeed. 

You should add this line to the top of your header file to set an upper limit
on the maximum number of objects in any pool:

    const int g_MaxNumberOfObjectsInPool = 1000;

It is also acceptable to assume that the user will not attempt to allocate
beyond the maximum number of objects the pool can handle so you do not need to
explicitly handle this case. An assert is sufficient.  Similarly, it's
acceptable to assume the user will not attempt to deallocate objects through
your pool that did not originally come from the pool. You should silently
handle the case of attempting to deallocate a NULL pointer.

Your code does not have to be thread-safe.


## Desirable properties

- Efficient:

The implementation should be as efficient as possible in both running time and
memory usage. We will prefer algorithmic optimisations over low-level (i.e.
assembly-level) optimisations unless you can clearly demonstrate any
advantages.

If possible, it's desirable that all 4 operations take O(1) time. Please give a
running time for each operation and the order of memory usage for your system.

The minimum amount of memory a pool could theoretically consume is
maximumNumberOfObjectsInPool * sizeof(Object).  It's desirable that the amount
of memory the pool requires is as close to this lower bound as possible.

- As general and as robust as possible:

Ideally, no restriction is put on the type of object that may be pooled, so
everything from basic types (char, int, char*, etc.) to complex
multiply-inherited classes with virtual functions should be handled by the
system.

It's recommended that you write a short unit test for your pool system as a
single .cpp file. Demonstrate that the classes in ExampleClasses.h can be
allocated/deallocated through the pool to check that your solution works for
all these cases.  Please note in your comments any types that are not
compatible with your system.

Although you do not need to handle error cases in your code, the design of the
system should be as defensive as possible.  For example, it would be a
desirable characteristic of the system design if it made it impossible to
deallocate a pooled object from a pool it was not allocated from.

- Well structured:

In the spirit of maximising code re-use, pooling an object should be as easy as
possible. 

Is is desirable that the pooling is as transparent as possible to any code
which may use or allocates/deallocates the pooled objects. Please describe any
changes that would be necessary to a user's code to use your system in your
comments.

It is desirable that the pooling is as transparent as possible to the object
class being pooled. Ideally you would still be able to allocate/deallocate the
class through new/delete/malloc/free or another allocator and array allocation
should continue to work as normal. If this is not possible then you will need
to modify ExampleClasses.h to make those classes poolable with your system.
Please highlight any changes you make.


If you have any questions on the test please feel free to get in touch.
Thanks.
