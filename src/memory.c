/** @file memory.c */
#include <stdio.h>
#include <stdlib.h>

#include <clib/exec_protos.h>
#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>
//#include <ratr0/amiga/memory.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33mMEMORY\033[0m", __VA_ARGS__)

/*
 * The global memory service instance.
 */
static struct Ratr0MemorySystem memory_system;
static Ratr0Engine *engine;

/*
 * Amiga Memory Subsystem
 * Since there are a few specifics in the Amiga Hardware (chip mem etc.) this
 * is a memory subsytem with those in mind.
 * The idea is to have 2 pools, one for assets and one for logic data which
 * should ideally be placed in 2 different areas of the memory.
 *
 * Because we have 2 memory pools, we use bit 31 as a tag, to indicate which pool
 * the handle is using. If bit 31 is set, we use the chip memory pool, otherwise
 * the general purpose pool.
 */
static void *general_mem_pool, *chip_mem_pool;
static UINT32 chip_pool_size, general_pool_size;

struct AllocatedBlock {
    void *block_address;
    UINT32 block_size;
};
/**
 * These tables hold our allocated blocks
 * TODO: Instead void *[], it should be struct AllocatedBlock[] so
 * we know the size of the block
 */
static struct AllocatedBlock *general_mem_table;
static struct AllocatedBlock *chip_mem_table;
static UINT32 chip_table_size, general_table_size;
static UINT32 first_free_chip, first_free_general;
static UINT32 first_free_chip_table, first_free_general_table;

// Forward declarations for the generic memory allocator
void ratr0_memory_shutdown(void);

struct Ratr0MemorySystem *ratr0_memory_startup(Ratr0Engine *eng, struct Ratr0MemoryConfig *config)
{
    engine = eng;
    memory_system.shutdown = &ratr0_memory_shutdown;


    chip_table_size = config->chip_table_size;
    general_table_size = config->general_table_size;
    chip_pool_size = config->chip_pool_size;
    general_pool_size = config->general_pool_size;

    general_mem_pool = (void *) AllocMem(general_pool_size, MEMF_CLEAR);
    if (!general_mem_pool) {
        PRINT_DEBUG("Can't allocate enough memory for general memory pool");
        exit(-1);
    }
    chip_mem_pool = (void *) AllocMem(chip_pool_size, MEMF_CHIP|MEMF_CLEAR);
    if (!chip_mem_pool) {
        PRINT_DEBUG("Can't allocate enough memory for chip memory pool");
        FreeMem(general_mem_pool, general_pool_size);
        exit(-1);
    }

    general_mem_table = (struct AllocatedBlock *)
        AllocMem(sizeof(struct AllocatedBlock) * general_table_size, MEMF_CLEAR);
    if (!general_mem_table) {
        PRINT_DEBUG("Can't allocate enough memory for general memory table");
        FreeMem(general_mem_pool, general_pool_size);
        FreeMem(chip_mem_pool, chip_pool_size);
        exit(-1);
    }
    chip_mem_table = (struct AllocatedBlock *)
        AllocMem(sizeof(struct AllocatedBlock) * chip_table_size, MEMF_CLEAR);
    if (!chip_mem_table) {
        PRINT_DEBUG("Can't allocate enough memory for chip memory table");
        FreeMem(general_mem_table,
                sizeof(struct AllocatedBlock) * general_table_size);
        FreeMem(general_mem_pool, general_pool_size);
        FreeMem(chip_mem_pool, chip_pool_size);
        exit(-1);
    }

    first_free_chip = first_free_general = 0;
    first_free_chip_table = first_free_general_table = 0;

    PRINT_DEBUG("Startup finished.");
    return &memory_system;
}

void ratr0_memory_shutdown(void)
{
    FreeMem(general_mem_table, sizeof(void *) * general_table_size);
    FreeMem(chip_mem_table, sizeof(void *) * chip_table_size);
    FreeMem(general_mem_pool, general_pool_size);
    FreeMem(chip_mem_pool, chip_pool_size);
    PRINT_DEBUG("Shutdown finished.");
}

Ratr0MemHandle ratr0_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 size)
{
    if (size % 2 == 1) {
#ifdef DEBUG
        fprintf(debug_fp, "requested to allocate odd number of bytes, adding a byte\n");
        fflush(debug_fp);
#endif
        size++;
    }
    if (mem_type == RATR0_MEM_CHIP) {
        if (first_free_chip + size > chip_pool_size) {
            // This is a fatal error -> Exit the engine !!
            PRINT_DEBUG("Chip memory exhausted, can't reserve more.");
#ifdef DEBUG
            fprintf(debug_fp, "Chip memory exhausted, can't reserve more.\n");
            fflush(debug_fp);
#endif
            ratr0_memory_shutdown();
            exit(-1);
        }
        Ratr0MemHandle result = first_free_chip_table;
        void *mem_block = (void *) ((UINT32) chip_mem_pool + first_free_chip);
        chip_mem_table[first_free_chip_table].block_address = mem_block;
        chip_mem_table[first_free_chip_table++].block_size = size;
        first_free_chip += size;
#ifdef DEBUG
        fprintf(debug_fp, "Allocated %u bytes of chip memory.\n", size);
        fflush(debug_fp);
#endif
        return result | 0x80000000;  // add a chip mem tag
    } else {
        if (first_free_general + size > general_pool_size) {
            // This is a fatal error -> Exit the engine !!
            PRINT_DEBUG("General memory exhausted, can't reserve more.");
#ifdef DEBUG
            fprintf(debug_fp, "General memory exhausted, can't reserve more.\n");
            fflush(debug_fp);
#endif
            ratr0_memory_shutdown();
            exit(-1);
        }
        Ratr0MemHandle result = first_free_general_table;
        void *mem_block = (void *) ((UINT32)general_mem_pool + first_free_general);
        general_mem_table[first_free_general_table].block_address = mem_block;
        general_mem_table[first_free_general_table++].block_size = size;
        first_free_general += size;
#ifdef DEBUG
        fprintf(debug_fp, "Allocated %u bytes of general purpose memory.\n", size);
        fflush(debug_fp);
#endif
        return result;
    }
}

void ratr0_memory_free_block(Ratr0MemHandle handle)
{
    // This is a no-op for now
    // As a first approach we should be able to free the last
    // allocated block. This is easy because it simply resets
    // the
    if ((handle & 0x80000000) == 0x80000000) {
        int chip_table_idx = handle & 0x7fffffff;
        if (chip_table_idx == (first_free_chip_table - 1)) {
#ifdef DEBUG
            fprintf(debug_fp, "Freeing the last allocated chip block, %u bytes\n",
                    chip_mem_table[chip_table_idx].block_size);
#endif
            first_free_chip -= chip_mem_table[chip_table_idx].block_size;
            // TODO:
            // first_free_chip_table--
        }
    }
}

void *ratr0_memory_block_address(Ratr0MemHandle handle)
{
    if ((handle & 0x80000000) == 0x80000000) {
        // chip mem
        return chip_mem_table[handle & 0x7fffffff].block_address;
    } else {
        return general_mem_table[handle].block_address;
    }
}
