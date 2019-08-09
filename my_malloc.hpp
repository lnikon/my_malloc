#pragma once

#include <unistd.h>

// Global variables to handle our allocator
int has_initialized = 0;
void* managed_memory_start = nullptr;
void* last_valid_address = nullptr;

// Control alloc-ed memory block
struct mem_control_block
{
    int is_available{0};
    int size{0};
};

void malloc_init()
{
    last_valid_address = sbrk(0);
    managed_memory_start = last_valid_address;
    has_initialized = 1;
}

void free(void *firstbyte)
{
    mem_control_block* mcb{nullptr};

    mcb = 
        reinterpret_cast<mem_control_block*>(
                static_cast<char*>(firstbyte) - sizeof(mem_control_block));

    mcb->is_available = 1;
    
    return;
}

void* malloc(long numbytes)
{
    void* current_location{nullptr};

    mem_control_block* current_location_mcb{nullptr};

    void* memory_location{nullptr};
    
    if(!has_initialized)
    {
        malloc_init();
    }

    numbytes = numbytes + sizeof(mem_control_block);

    current_location = managed_memory_start;

    while(current_location != last_valid_address)
    {
        current_location_mcb = 
            reinterpret_cast<mem_control_block*>(current_location);

        if(current_location_mcb->is_available)
        {
            if(current_location_mcb->size >= numbytes)
            {
                current_location_mcb->is_available = 0;
                memory_location = current_location;
                break;
            }
        }

        current_location = 
            static_cast<void*>(
                    static_cast<char*>(current_location) + current_location_mcb->size);
    }

    if(!memory_location)
    {
        sbrk(numbytes);
        memory_location = last_valid_address;

        last_valid_address = 
            static_cast<void*>(
                    static_cast<char*>(last_valid_address) + numbytes);
        
        current_location_mcb = 
            reinterpret_cast<mem_control_block*>(memory_location);
        current_location_mcb->is_available = 0;
        current_location_mcb->size = numbytes;
    }

    memory_location = 
        static_cast<void*>(
                static_cast<char*>(memory_location) + sizeof(mem_control_block));

    return memory_location;
}
