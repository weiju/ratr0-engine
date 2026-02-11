/**
 * test_console.c - this is a prototype to test keyboard input on
 * AmigaOS 1.3. Since the early versions of AmigaOS don't have a
 * keymap.library, we need to know how to access and utilize the user
 * keymap so we can use international keyboards properly.
 */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <devices/console.h>
#include <devices/conunit.h>
#include <devices/keymap.h>
#include <exec/execbase.h>
#include <SDI/SDI_compiler.h>

#include <stdio.h>
#include <string.h>

static BYTE error;
static struct MsgPort *console_mp;
static struct IOStdReq *console_io;

struct KeyMap keymap;

/**
 * Using a keymap structure, maps a raw keycode to a mapped code
 */
int ratr0_input_map_rawkey(struct KeyMap *keykmap, int keycode)
{
    int plain_keycode = keycode & 0x7f;
    if (keycode <= 0x3f) {
        // (0x00-0x3f) low keymap
    } else {
        // (0x40-0x7f) high keymap
    }
    return 0;
}

char type_buffer[200];

void type_to_buffer(char typecode)
{
    type_buffer[0] = '\0';
    if (typecode == 0) {
        sprintf(type_buffer, "KC_NOQUAL");
        return;
    }

    if (typecode & KCF_STRING) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_STRING");
    }

    if (typecode & KCF_NOP) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_NOP");
    }
    if (typecode & KCF_DEAD) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_DEAD");
    }
    if (typecode & KCF_SHIFT) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_SHIFT");
    }
    if (typecode & KCF_ALT) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_ALT");
    }
    if (typecode & KCF_CONTROL) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_CONTROL");
    }
    if (typecode & KCF_DOWNUP) {
        if (strlen(type_buffer) > 0) {
            strcat(type_buffer, " | ");
        }
        strcat(type_buffer, "KCF_DOWNNUP");
    }
}

void write_lo_keymap_types(FILE *fp, struct KeyMap *keymap)
{
    fprintf(fp, "Lo KeyMap Types:\n");
    fprintf(fp, "----------------\n");
    for (int i = 0; i < 0x40; i++) {
        fprintf(fp, "0x%02x -> ", i);
        type_to_buffer(keymap->km_LoKeyMapTypes[i]);
        fprintf(fp, "%s\n", type_buffer);
    }
}

void write_hi_keymap_types(FILE *fp, struct KeyMap *keymap)
{
    fprintf(fp, "\nHi KeyMap Types:\n");
    fprintf(fp, "----------------\n");
    // 0x6b-0x7f are UNDEFINED, so don't even bother !!!
    for (int i = 0; i < 0x2c; i++) {
        fprintf(fp, "0x%02x -> ", 0x40 + i);
        type_to_buffer(keymap->km_HiKeyMapTypes[i]);
        fprintf(fp, "%s\n", type_buffer);
    }
}

int get_num_qualifiers(int typecode)
{
    int result = 0;
    if (typecode & KCF_SHIFT) result++;
    if (typecode & KCF_ALT) result++;
    if (typecode & KCF_CONTROL) result++;
    // We can ignore DEAD and DOWNUP
    //if (typecode & KCF_DEAD) result++;
    //if (typecode & KCF_DOWNUP) result++;
    return result;
}

char str_buffer[30];
void write_string_descriptor(FILE *fp, int num_qualifiers, char *addr)
{
    for (int i = 0; i < num_qualifiers + 1; i++) {
        unsigned char slen = ((unsigned char *) addr)[2 * i + 0];
        unsigned char soff = ((unsigned char *) addr)[2 * i + 1];
        fprintf(fp, "  %d: slen= %d, soff= %d ", i, (int) slen, (int) soff);

        // there's actually no terminating 0-byte, so we need
        // to copy the bytes
        for (int j = 0; j < slen; j++) {
            str_buffer[j] = addr[soff + j];
        }
        str_buffer[slen] = '\0';
        fprintf(fp, "\"%s\"\n", str_buffer);
    }
}

void write_dead_descriptor(FILE *fp, int num_qualifiers, char *addr)
{
    for (int i = 0; i < num_qualifiers + 1; i++) {
        unsigned char flag = ((unsigned char *) addr)[2 * i + 0];
        const char *fs;
        if (flag == 0) fs = "0";
        else if (flag == DPF_DEAD) fs = "DPF_DEAD";
        else if (flag == DPF_MOD) fs = "DPF_MOD";
        else fs = "UNKNOWN";
        unsigned char c = ((unsigned char *) addr)[2 * i + 1];
        if (flag == DPF_MOD) {
            // DPF_MOD has to be handled differently, the
            // second byte of the descriptor is the offset of the
            // characters that are modified
            unsigned char soff = ((unsigned char *) addr)[2 * i + 1];
            c = addr[soff];
        }
        fprintf(fp, "  %d: [%s | '%c']\n", i,
                fs, c);
    }
}

void write_keymap_entry(FILE *fp, struct KeyMap *keymap, int raw_code)
{
    int typecode, offset = 0, mapped_value;
    if (raw_code < 0x40) {
        // LO (is mapped to NOP)
        // 0x0e, 0x1c, 0x2c, 0x3b
        typecode = keymap->km_LoKeyMapTypes[raw_code];
        mapped_value = keymap->km_LoKeyMap[raw_code];
    } else if (raw_code < 0x6b) {
        // HI
        // 0x47-0x49, 0x4b (mapped to NOP)
        typecode = keymap->km_HiKeyMapTypes[raw_code - 0x40];
        raw_code -= 0x40;
        offset = 0x40;
        mapped_value = keymap->km_HiKeyMap[raw_code];
    } else {
       // 0x6b-0x7f are UNDEFINED, so don't even bother !!!
        fprintf(fp, "0x%02x -> NOP", raw_code);
        return;
    }

    // generic handling of typecode and raw_code
    // 1. NOP -> quick exit
    if (typecode & KCF_NOP) {
        fprintf(fp, "0x%02x -> NOP\n", raw_code + offset);
        return;
    }
    // Has qualifiers
    int num_qualifiers = get_num_qualifiers(typecode);
    type_to_buffer(typecode);

    if (typecode & KCF_STRING) {
        // 1. STRING (e.g. TAB)
        fprintf(fp, "0x%02x [STRING] (%s)\n",
                raw_code + offset, type_buffer);
        write_string_descriptor(fp, num_qualifiers, (char *) mapped_value);
    } else if (typecode & KCF_DEAD) {
        // 2. DEAD class
        fprintf(fp, "0x%02x [DEAD] (%s)\n",
                raw_code + offset, type_buffer);
        write_dead_descriptor(fp, num_qualifiers, (char *) mapped_value);

    } else  {
        // 4. It's a character code entry
        int bytes = mapped_value;
        fprintf(fp, "0x%02x [KEY] (%s)\n",
                raw_code + offset, type_buffer);
        if ((typecode & KC_VANILLA) == KC_VANILLA) {
            fprintf(fp, "  '%c' [0x%02x] (shift + alt)\n",
                    (bytes >> 24) & 0x7f, (bytes >> 24) & 0x7f);
            fprintf(fp, "  '%c' [0x%02x] (alt)\n",
                    (bytes >> 16) & 0x7f, (bytes >> 16) & 0x7f);
            fprintf(fp, "  '%c' [0x%02x] (shift)\n",
                    (bytes >> 8) & 0x7f, (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (0x%02x) (unqualified)\n",
                    bytes & 0x7f, bytes & 0x7f);
            // Bits 6 and 5 set to 0
            fprintf(fp, "  '%c' (control + shift + alt)\n",
                    (bytes & 0x7f) & 0x9f);
        } else if (typecode & KCF_SHIFT && typecode & KCF_ALT) {
            fprintf(fp, "  '%c' (shift + alt)\n", (bytes >> 24) & 0x7f);
            fprintf(fp, "  '%c' (alt)\n", (bytes >> 16) & 0x7f);
            fprintf(fp, "  '%c' (shift)\n", (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_SHIFT && typecode & KCF_CONTROL) {
            fprintf(fp, "  '%c' (ctrl + shift)\n", (bytes >> 24) & 0x7f);
            fprintf(fp, "  '%c' (ctrl)\n", (bytes >> 16) & 0x7f);
            fprintf(fp, "  '%c' (shift)\n", (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_ALT && typecode & KCF_CONTROL) {
            fprintf(fp, "  '%c' (ctrl + alt)\n", (bytes >> 24) & 0x7f);
            fprintf(fp, "  '%c' (ctrl)\n", (bytes >> 16) & 0x7f);
            fprintf(fp, "  '%c' (alt)\n", (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_ALT) {
            fprintf(fp, "  '%c' (alt)\n", (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_CONTROL) {
            fprintf(fp, "  '%c' (control)\n", (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_SHIFT) {
            fprintf(fp, "  '%c' (shift)\n", (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else {
            fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        }
    }
}

void write_keymap_info(struct KeyMap *keymap)
{
    FILE *fp = fopen("keymap_info.txt", "w");

    // 0x60-0x6a are modifiers
    // > 0x6b is undefined
    for (int keycode = 0; keycode < 0x60; keycode++) {
        write_keymap_entry(fp, keymap, keycode);
        fputs("\n---------------------------------------------------------------------\n", fp);
    }
    fclose(fp);
}


// ------------------------------------------------------------------------
// RATR0 KEYMAP SYSTEM
// ------------------------------------------------------------------------

typedef enum {
    RATR0_KEYTYPE_NOP = 0,
    RATR0_KEYTYPE_CONTROL, RATR0_KEYTYPE_ASCII
} Ratr0KeyType;

/**
 * A simplified key mapping system that respects user defined keymaps
 */
struct Ratr0Key {
    Ratr0KeyType key_type; // control key or character
    char code;  // virtual code for control
};

struct Ratr0Key unshifted[0x60];
struct Ratr0Key shifted[0x60];

void set_keymap_entry(struct KeyMap *keymap, int raw_code,
                      struct Ratr0Key *unshifted, struct Ratr0Key *shifted)
{
    int typecode, offset = 0, mapped_value;
    if (raw_code < 0x40) {
        // LO (is mapped to NOP)
        // 0x0e, 0x1c, 0x2c, 0x3b
        typecode = keymap->km_LoKeyMapTypes[raw_code];
        mapped_value = keymap->km_LoKeyMap[raw_code];
    } else if (raw_code < 0x6b) {
        // HI
        // 0x47-0x49, 0x4b (mapped to NOP)
        typecode = keymap->km_HiKeyMapTypes[raw_code - 0x40];
        raw_code -= 0x40;
        offset = 0x40;
        mapped_value = keymap->km_HiKeyMap[raw_code];
    } else {
       // 0x6b-0x7f are UNDEFINED, so don't even bother !!!
        return;
    }

    // generic handling of typecode and raw_code
    // 1. NOP -> quick exit
    if (typecode & KCF_NOP) {
        // set type to NOP
        unshifted->key_type = shifted->key_type = RATR0_KEYTYPE_NOP;
        return;
    }
    // Has qualifiers
    int num_qualifiers = get_num_qualifiers(typecode);

    if (typecode & KCF_STRING) {
        // 1. STRING
        // TODO
        //write_string_descriptor(fp, num_qualifiers, (char *) mapped_value);
    } else if (typecode & KCF_DEAD) {
        // 2. DEAD class
        // TODO
        //write_dead_descriptor(fp, num_qualifiers, (char *) mapped_value);
    } else  {
        // 3. It's a character code entry
        // TODO
        int bytes = mapped_value;
        if ((typecode & KC_VANILLA) == KC_VANILLA) {
            /*
            fprintf(fp, "  '%c' [0x%02x] (shift + alt)\n",
                    (bytes >> 24) & 0x7f, (bytes >> 24) & 0x7f);
            fprintf(fp, "  '%c' [0x%02x] (alt)\n",
                    (bytes >> 16) & 0x7f, (bytes >> 16) & 0x7f);
            fprintf(fp, "  '%c' [0x%02x] (shift)\n",
                    (bytes >> 8) & 0x7f, (bytes >> 8) & 0x7f);
            fprintf(fp, "  '%c' (0x%02x) (unqualified)\n",
                    bytes & 0x7f, bytes & 0x7f);
            // Bits 6 and 5 set to 0
            fprintf(fp, "  '%c' (control + shift + alt)\n",
                    (bytes & 0x7f) & 0x9f);
            */
        } else if (typecode & KCF_SHIFT && typecode & KCF_ALT) {
            //fprintf(fp, "  '%c' (shift + alt)\n", (bytes >> 24) & 0x7f);
            //fprintf(fp, "  '%c' (alt)\n", (bytes >> 16) & 0x7f);
            //fprintf(fp, "  '%c' (shift)\n", (bytes >> 8) & 0x7f);
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_SHIFT && typecode & KCF_CONTROL) {
            //fprintf(fp, "  '%c' (ctrl + shift)\n", (bytes >> 24) & 0x7f);
            //fprintf(fp, "  '%c' (ctrl)\n", (bytes >> 16) & 0x7f);
            //fprintf(fp, "  '%c' (shift)\n", (bytes >> 8) & 0x7f);
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_ALT && typecode & KCF_CONTROL) {
            //fprintf(fp, "  '%c' (ctrl + alt)\n", (bytes >> 24) & 0x7f);
            //fprintf(fp, "  '%c' (ctrl)\n", (bytes >> 16) & 0x7f);
            //fprintf(fp, "  '%c' (alt)\n", (bytes >> 8) & 0x7f);
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_ALT) {
            //fprintf(fp, "  '%c' (alt)\n", (bytes >> 8) & 0x7f);
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_CONTROL) {
            //fprintf(fp, "  '%c' (control)\n", (bytes >> 8) & 0x7f);
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else if (typecode & KCF_SHIFT) {
            //fprintf(fp, "  '%c' (shift)\n", (bytes >> 8) & 0x7f);
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        } else {
            //fprintf(fp, "  '%c' (unqualified)\n", bytes & 0x7f);
        }
    }
}

void ratr0_make_keymap(struct KeyMap *keymap, struct Ratr0Key (*unshifted)[0x60],
                       struct Ratr0Key (*shifted)[0x60])
{
    for (int keycode = 0; keycode < 0x60; keycode++) {
        set_keymap_entry(keymap, keycode, unshifted[keycode],
                         shifted[keycode]);
    }
}

// ------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    console_mp = CreatePort(0, 0);
    console_io = (struct IOStdReq *) CreateExtIO(console_mp,
                                                 sizeof(struct IOStdReq));
    error = OpenDevice("console.device", CONU_LIBRARY,
                       (struct IORequest *) console_io, 0);
    if (!error) {
        // In library console devices, we only have the default keymap !!!
        console_io->io_Command = CD_ASKDEFAULTKEYMAP;
        //console_io->io_Command = CD_ASKKEYMAP;
        console_io->io_Length = sizeof(struct KeyMap);
        console_io->io_Data = (APTR) &keymap;
        console_io->io_Flags = 0;
        DoIO((struct IORequest *) console_io);
        if(console_io->io_Error) {
            printf("Could not retrieve keymap !!!!\n");
        }
        write_keymap_info(&keymap);
    } else {
        printf("Could not open device with CONU_LIBRARY !!!!\n");
    }

    if (console_io) {
        CloseDevice((struct IORequest *) console_io);
        DeleteExtIO((struct IORequest *) console_io);
    }
    if (console_mp) DeletePort(console_mp);

    return 0;
}
