#include <stdlib.h>
#include <clib/exec_protos.h>
#include <ratr0/debug_utils.h>
#include <ratr0/amiga/memory.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33mMEMORY\033[0m", __VA_ARGS__)

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

static void **general_mem_table, **chip_mem_table;
static UINT32 chip_table_size, general_table_size;
static UINT32 first_free_chip, first_free_general;
static UINT32 first_free_chip_table, first_free_general_table;

void ratr0_amiga_memory_startup(struct Ratr0MemoryConfig *config)
{
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

    general_mem_table = (void **) AllocMem(sizeof(void *) * general_table_size, MEMF_CLEAR);
    if (!general_mem_table) {
        PRINT_DEBUG("Can't allocate enough memory for general memory table");
        FreeMem(general_mem_pool, general_pool_size);
        FreeMem(chip_mem_pool, chip_pool_size);
        exit(-1);
    }
    chip_mem_table = (void **) AllocMem(sizeof(void *) * chip_table_size, MEMF_CLEAR);
    if (!chip_mem_table) {
        PRINT_DEBUG("Can't allocate enough memory for chip memory table");
        FreeMem(general_mem_table, sizeof(void *) * general_table_size);
        FreeMem(general_mem_pool, general_pool_size);
        FreeMem(chip_mem_pool, chip_pool_size);
        exit(-1);
    }

    first_free_chip = first_free_general = 0;
    first_free_chip_table = first_free_general_table = 0;
}

void ratr0_amiga_memory_shutdown(void)
{
    FreeMem(general_mem_table, sizeof(void *) * general_table_size);
    FreeMem(chip_mem_table, sizeof(void *) * chip_table_size);
    FreeMem(general_mem_pool, general_pool_size);
    FreeMem(chip_mem_pool, chip_pool_size);
}

Ratr0MemHandle ratr0_amiga_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 size)
{
    if (mem_type == RATR0_MEM_CHIP) {
        if (first_free_chip + size > chip_pool_size) {
            // This is a fatal error -> Exit the engine !!
            PRINT_DEBUG("Chip memory exhausted, can't reserve more.");
            ratr0_amiga_memory_shutdown();
            exit(-1);
        }
        Ratr0MemHandle result = first_free_chip_table;
        void *mem_block = (void *) ((UINT32) chip_mem_pool + first_free_chip);
        chip_mem_table[first_free_chip_table++] = mem_block;
        first_free_chip += size;
        return result | 0x80000000;  // add a chip mem tag
    } else {
        if (first_free_general + size > general_pool_size) {
            // This is a fatal error -> Exit the engine !!
            PRINT_DEBUG("General memory exhausted, can't reserve more.");
            ratr0_amiga_memory_shutdown();
            exit(-1);
        }
        Ratr0MemHandle result = first_free_general_table;
        void *mem_block = (void *) ((UINT32)general_mem_pool + first_free_general);
        general_mem_table[first_free_general_table++] = mem_block;
        first_free_general += size;
        return result;
    }
}

void ratr0_amiga_memory_free_block(Ratr0MemHandle handle)
{
}

void *ratr0_amiga_memory_block_address(Ratr0MemHandle handle)
{
    if ((handle & 0x80000000) == 0x80000000) {
        // chip mem
        return chip_mem_table[handle & 0x7fffffff];
    } else {
        return general_mem_table[handle];
    }
}
