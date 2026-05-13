#include "is_bad_ptr.hpp"


bool is_bad_ptr(void* ptr)
{
    #ifdef __APPLE__

        vm_map_t task = mach_task_self();
        mach_vm_address_t address = (mach_vm_address_t)ptr;
        mach_vm_size_t size = 0;
        vm_region_basic_info_data_64_t info = {};
        mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
        mach_port_t object_name = 0;
        kern_return_t ret = mach_vm_region(task, &address, &size, VM_REGION_BASIC_INFO_64, (vm_region_info_t)&info, &count, &object_name);

        if (ret != KERN_SUCCESS) { return true; }

        if ((mach_vm_address_t)ptr < address || (mach_vm_address_t)ptr >= address + size) { return true; }

        if ((info.protection & VM_PROT_ALL) == 0) { return true; }

        return false;

    #elif defined(_WIN32)

        MEMORY_BASIC_INFORMATION mbi;

        if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) { return true; }

        if (mbi.State != MEM_COMMIT) { return true; }

        if (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) { return true; }
        
        return false;

    #elif defined(__linux__)

        static long page_size = 0;
        if (page_size == 0) page_size = sysconf(_SC_PAGESIZE);
        
        uintptr_t page_start = (uintptr_t)ptr & ~(page_size - 1);
        
        unsigned char vec = 0;
        if (mincore((void*)page_start, 1, &vec) != 0) { return true; }
        
        return false;

    #else
        return (ptr == NULL);

    #endif
}