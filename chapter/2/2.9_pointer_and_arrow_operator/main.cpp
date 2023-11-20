struct MemoryMap
{
    unsigned long long buffer_size;
    void *buffer;
    unsigned long long map_size;
    unsigned long long map_key;
    unsigned long long descriptor_size;
    unsigned int descriptor_version;
};

int main()
{
    struct MemoryMap m;
    struct MemoryMap *pm = &m;
    // If you want to access the member of a struct pointer, you can use the arrow operator (->)
    // The arrow operator is a syntactic sugar of (*pm).buffer_size

    // These three lines are equivalent
    pm->buffer_size = 16;
    (*pm).buffer_size = 16;
    m.buffer_size = 16;
}
