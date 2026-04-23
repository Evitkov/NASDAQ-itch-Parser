### Reinterpret Cast and Pointers
* I got stuck on how to actually get the raw file bytes into my packed C++ structs. `file.read()` strictly demands a `char*` pointer because a `char` represents exactly 1 byte of physical memory.
* If I hand it `&msg` (a pointer to my complex struct), the compiler throws a wall and refuses to compile because the types don't match.
* The solution is overriding the compiler using `reinterpret_cast<char*>(&msg)`. 
* I specifically use `reinterpret_cast` instead of a C-style `(char*)` cast because the C-style cast is not as safe and would ignore all errors, which is why is see this as the better coding practise.  

### Endianness and Bit Flipping
* After getting the data into the struct, the prices and quantities are flipped. NASDAQ sends data in Big-Endian order (left-to-right) - from teh name big end first. The CPU uses Little-Endian (smallest byte first) - this is also because how we can optimize math - we can start counting even before getting the whole number.
* The solution is byte swapping to convert Big to Little. For a 32-bit int: [A][B][C][D] must become [D][C][B][A].
* The pipeline works like this: We shift bytes left `<<` or right `>>` to their new slots. We use a bitmask `&`  to isolate the exact byte and destroy other data. Finally, we use `|` to combine them back together.
* After doing this for all bytes we save it back to overwrite the original struct variable: `msg.locate = ...` so we can work with it correctly in the future.

### Message Parsing and Jump Tables
* Another interesting CPU optimization I learned about is how the compiler handles **jump tables**. 
* In the ITCH protocol, every message starts with a single character indicating its type—for example, 'A' for an Add Order, 'E' for an Execution, or 'C' for a Cancel.
* If I used an `if-else` chain to route these messages, the CPU would compile that into sequential branch instructions (ofcourse switch would always be used here also for code clarity but it is interesting to see the execution difference).
* By using a `switch` statement on that message type character, the C++ compiler can do something much smarter. Because the ASCII integer values of these characters  are close together, the compiler optimizes the `switch` into a jump table.
* A jump table is basically a hidden array created at compile-time. The indices of the array correspond to letters of the message types, and the data stored at each index is the physical memory address (a pointer) to the block of code that handles that specific message.
* When the engine runs and reads the first byte of a new packet, the CPU doesn't evaluate any conditions. It just takes the ASCII value of that character, uses it as a direct index into the jump table, and instantly jumps execution to the correct code block. This makes evaluation much faster.