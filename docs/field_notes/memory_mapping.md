### Memory Mapping and Zero-Copy I/O

* Standard file I/O (`std::ifstream`, `fread`) is slow because every read requires a context switch. Data is copied from the disk to kernel space, and then into user space.
* Memory mapping (`mmap` on Linux, `MapViewOfFile` on Windows) eliminates this double-copy by mapping the physical file directly into the program's virtual memory address space.
* The OS hands back a `char*` pointer to the first byte of the file. As the pointer moves forward, the CPU's hardware (MMU) automatically pages data from the SSD to physical RAM via page faults.

### In-Place Parsing and Pointer Arithmetic

* Since the entire 10GB file is mapped as one continuous array, there's no need for intermediate buffers.
* Navigation is done entirely via pointer arithmetic. To reach the next packet, just add the message size to the current pointer (`ptr += message_length`).
* Using `reinterpret_cast<const OrderMessage*>(ptr)`, the C++ structs are placed directly over the mapped raw memory - No instantiating new structs or copying .

### Pointer Safety and The `const` Keyword

* Modifying read-only mapped memory triggers an instant segmentation fault, that is why we use `const`.
* I was introduced here to different syntax usecases of the const keyword:
* `const char* ptr` (or `char const* ptr`): The *data* is constant. The pointer can move forward (`ptr++`), but the data can't be modified (`*ptr = 'A'` fails to compile). This is exactly what's needed for iterating a read-only memory map.
* `char* const ptr`: The *pointer* is constant. Data can be modified, but the pointer address is locked.
* `const char* const ptr`: Both pointer and data are locked.


* Passing around a `const char*` guarantees the compiler blocks accidental overwrites while still allowing the pointer to slide across the exchange feed.
  We can also use const as a keyword in a function signature (like void get_price() const) to guarantee to the compiler that the method will not modify the objects internal state - all object data will be read only.