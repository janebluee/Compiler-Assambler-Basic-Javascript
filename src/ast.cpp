#include "../include/ast.hpp"
#include "../include/memory_pool.hpp"

namespace js {


thread_local MemoryPool<ASTNode> nodePool;

void* ASTNode::operator new(size_t size) {
    return nodePool.allocate(1);
}

void ASTNode::operator delete(void* ptr) noexcept {
    if (ptr) {
        nodePool.deallocate(static_cast<ASTNode*>(ptr));
    }
}

} 
