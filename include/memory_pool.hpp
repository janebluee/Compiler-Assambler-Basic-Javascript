#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include <new>

namespace js {

template<typename T, size_t BlockSize = 4096>
class MemoryPool {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;

    MemoryPool() noexcept : currentBlock_(nullptr), currentSlot_(nullptr), lastSlot_(nullptr), freeSlots_(nullptr) {}

    ~MemoryPool() noexcept {
        for (auto block : blocks_) {
            if (block) {
                ::operator delete(block);
            }
        }
    }

    pointer allocate(size_type n = 1) {
        if (n > 1) {
            return static_cast<pointer>(::operator new(n * sizeof(value_type)));
        }

        if (freeSlots_) {
            pointer result = reinterpret_cast<pointer>(freeSlots_);
            freeSlots_ = freeSlots_->next;
            return result;
        }

        if (currentSlot_ >= lastSlot_) {
            allocateBlock();
        }

        return reinterpret_cast<pointer>(currentSlot_++);
    }

    void deallocate(pointer p, size_type n = 1) noexcept {
        if (!p) return;
        
        if (n > 1) {
            ::operator delete(p);
            return;
        }

        auto slot = reinterpret_cast<slot_pointer_>(p);
        slot->next = freeSlots_;
        freeSlots_ = slot;
    }

    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) noexcept {
        if (p) {
            p->~U();
        }
    }

private:
    struct Slot_ {
        union {
            value_type element;
            Slot_* next;
        };
        
        Slot_() noexcept : next(nullptr) {}
        ~Slot_() noexcept {}
    };

    using slot_pointer_ = Slot_*;

    slot_pointer_ currentBlock_;
    slot_pointer_ currentSlot_;
    slot_pointer_ lastSlot_;
    slot_pointer_ freeSlots_;
    std::vector<slot_pointer_> blocks_;

    void allocateBlock() {
        auto newBlock = reinterpret_cast<slot_pointer_>(
            ::operator new(BlockSize * sizeof(Slot_)));
        blocks_.push_back(newBlock);
        currentBlock_ = newBlock;
        currentSlot_ = newBlock;
        lastSlot_ = newBlock + BlockSize;
        
        for (size_t i = 0; i < BlockSize; ++i) {
            new (&newBlock[i]) Slot_();
        }
    }
};

} 
