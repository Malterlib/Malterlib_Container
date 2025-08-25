# CLAUDE.md - Container Module

## Module Overview

The Container module provides high-performance, STL-compatible container classes built on top of the Intrusive module's primitives. These containers offer both intrusive and non-intrusive options with superior performance characteristics compared to standard implementations, while maintaining familiar interfaces for ease of use.

## Key Components

### Core Containers
- **Vector** (`TCVector`) - Dynamic array with configurable growth/shrink policies
  - Options for minimum size, shrinking behavior, bounds checking
  - Extensive operation support through modular implementation files
  - Iterator support with bidirectional and reverse variants

- **Map** (`TCMap`) - Ordered associative container based on AVL tree
  - Built on intrusive AVL tree for O(log n) operations
  - Multiple iterator types: value, key, key-value access
  - Node handles for efficient extraction/insertion
  - Batch operations support

- **Set** (`TCSet`) - Ordered unique element container
  - Shares implementation with Map (value-only)
  - Format and comparison support

- **LinkedList** - Non-intrusive wrapper around intrusive lists
  - Provides STL-like interface
  - Automatic memory management

### Specialized Containers
- **MapWithPool** / **SetWithPool** - Pool-allocated variants for performance
- **PagedByteVector** - Efficiently handles large byte arrays with paging
- **BitArray** - Compact bit storage with multiple implementations:
  - Standard BitArray
  - BitArrayHierarchical - Multi-level bit indexing
  - BitArrayPowerTwo - Optimized for power-of-2 sizes

- **Registry** - Hierarchical key-value store (like Windows Registry)
  - Mixed type support
  - String table optimization
  - Path-based access
  - Diff/merge capabilities

- **Regions** - Memory region management container

## Module-Specific Conventions

### Namespace Organization
- Primary namespace: `NMib::NContainer`
- Private implementations: `NMib::NContainer::NPrivate`

### Naming Patterns
- Template containers: `TC[Container]` (e.g., `TCVector`, `TCMap`)
- Options structures: `TC[Container]Options` (e.g., `TCVectorOptions`)
- Node types: `TC[Container]Node` (e.g., `TCMapNode`)
- Iterators: `C[Container]Iterator[Variant]` (e.g., `CMapIteratorBidirectional`)
- Results: `TC[Container]Result` (e.g., `TCMapResult`)

### Template Parameters Convention
- `t_CData` / `t_CKey` / `t_CValue` - Element types
- `t_CCompare` - Comparison functor (defaults to `CSort_Default`)
- `t_CAllocator` - Memory allocator (defaults to `CAllocator_Heap`)
- `t_COptions` - Container-specific options

### Function Naming
- `f_*` - Regular member functions
- `f_Get*` - Accessor functions
- `f_Insert*` - Insertion operations
- `f_Remove*` - Removal operations
- `f_Find*` - Search operations

## Dependencies

### Internal Malterlib Modules
- **Intrusive** - Provides underlying AVL tree and linked list implementations
- **Core** - Basic types and platform abstractions
- **Memory** - Allocator interfaces
- **Algorithm** - Sorting and searching algorithms
- **Iterator** - Iterator base classes
- **Stream** - Serialization support
- **String** - String operations for Registry

## Architecture Details

### Vector Implementation
```cpp
// Configurable vector options
template <mint t_MinSize, bool t_bShrink, bool t_bCheckBounds>
struct TCVectorOptions {
    static constexpr mint mc_MinSize = t_MinSize;        // Minimum allocation
    static constexpr bool mc_bShrink = t_bShrink;        // Auto-shrink on remove
    static constexpr bool mc_bCheckBounds = t_bCheckBounds; // Bounds checking
};
```

### Map Architecture
```cpp
// Built on intrusive AVL tree - nodes contain both key and value
template <typename t_CKey, typename t_CValue>
struct TCMapNode {
    NIntrusive::CAVLLink m_Link;  // Intrusive link
    t_CKey m_Key;
    t_CValue m_Value;
};
```

### Registry Structure
```cpp
// Hierarchical storage with mixed types
// Supports: bool, int, float, string, binary data
// Path-based access: "Software/Company/Product/Setting"
```

### Modular Implementation
The Container module uses extensive header separation for maintainability:
- Core headers define interfaces
- Implementation split into operation-specific files
- Example for Vector:
  - `Vector_Insert*.hpp` - Various insertion methods
  - `Vector_Remove.hpp` - Removal operations
  - `Vector_Sort.hpp` - Sorting functionality
  - `Vector_Stream.hpp` - Serialization

## Common Tasks

### Using Vector with Custom Options
```cpp
// Define custom options
using CMyOptions = TCVectorOptions<32, false, true>; // Min 32, no shrink, bounds check
using CMyVector = TCVector<int, CAllocator_Heap, CMyOptions>;

CMyVector Vec;
Vec.f_InsertLast(42);
```

### Map with Custom Comparison
```cpp
struct CMyCompare {
    auto operator()(CMyKey const &_Left, CMyKey const &_Right) const {
        return _Left.m_Priority <=> _Right.m_Priority;  // Three-way comparison
    }
};
using CMyMap = TCMap<CMyKey, CMyValue, CMyCompare>;
```

### Registry Usage
```cpp
CRegistry Reg;
Reg.f_SetValue("Software/MyApp/Version", "1.0.0");
auto Version = Reg.f_GetString("Software/MyApp/Version");
```

### Running Module Tests
```bash
# Build tests
./mib build Tests macOS x86_64 Debug

# Run all container tests
/opt/Deploy/Tests/RunAllTests --paths '["Container/*"]'

# Run specific tests
/opt/Deploy/Tests/RunAllTests --paths '["Container/Vector", "Container/Map"]'

# Performance tests
/opt/Deploy/Tests/RunAllTests --paths '["Container/MapPerformance", "Container/VectorPerformance"]'
```

## Important Files

### Headers (Public API)
- `Include/Mib/Container/*` - Public interfaces for all containers

### Core Container Implementations
- `Source/Malterlib_Container_Vector.h/cpp` - Vector implementation
- `Source/Malterlib_Container_Map.h` - Map implementation
- `Source/Malterlib_Container_Set.h` - Set implementation
- `Source/Malterlib_Container_LinkedList.h` - Linked list wrapper
- `Source/Malterlib_Container_Registry.h/cpp` - Registry implementation

### Specialized Containers
- `Source/Malterlib_Container_BitArray*.h` - Bit array variants
- `Source/Malterlib_Container_PagedByteVector.h/cpp` - Paged byte storage
- `Source/Malterlib_Container_Regions.h` - Region management
- `Source/Malterlib_Container_*WithPool.h` - Pool-allocated variants

### Modular Implementation Directories
- `Source/Vector/*.hpp` - Vector operation implementations
- `Source/Map/*.hpp` - Map operation implementations
- `Source/Set/*.hpp` - Set operation implementations
- `Source/Registry/*.hpp` - Registry operation implementations

## Module-Specific Notes

### Performance Characteristics
- **Vector**: O(1) append/remove at end, O(n) insert/remove in middle
- **Map/Set**: O(log n) all operations (AVL tree balanced)
- **Pool variants**: For special cases, for example when memory manager is not yet available. Usually memory manager is more efficient
- **BitArray**: O(1) bit operations, hierarchical variants offer fast scanning
- **Registry**: O(log n) per path component

### Comparison with STL
- Generally faster than STL due to:
  - Intrusive foundations (fewer allocations)
  - Better cache locality
  - Optimized allocators
  - Three-way comparison operator usage
- See `Documentation/Malterlib_Container_STLComparison.dox` for details

### Memory Management
- Containers manage memory for non-intrusive elements
- Vector shrinking is configurable per instance
- Registry uses string table for key deduplication

### Thread Safety
- Containers are NOT thread-safe by default
- External synchronization required
- Consider using concurrent variants from Concurrency module
- Read-only access from multiple threads is safe

### Iterator Invalidation Rules
- **Vector**: Invalidated on reallocation (insert/remove)
- **Map/Set**: All iterators are invalidated on insert/remove
- **LinkedList**: Only iterators to removed elements invalidated
- **Registry**: Path-based access remains valid across modifications
- Bidirectional iterators support both forward and backward traversal
- For Map/set bidirectional are less efficient so use forward/backward iterator only if sufficient

### Design Patterns
- Heavy use of templates for compile-time optimization
- Modular implementation through included headers
- Options structures for configurable behavior
- Node handles for efficient element transfer between containers

### Known Limitations
- Registry paths limited to reasonable depth (implementation defined)
- BitArrayHierarchical has overhead for small bit counts
- Vector bounds checking has runtime cost (configurable)

### Best Practices
- Use pool variants when container size is predictable
- Configure vector options based on usage patterns
- Prefer iterators over index access for traversal
- Use Registry for hierarchical configuration data
- Consider BitArrayPowerTwo for power-of-2 sized bit sets
