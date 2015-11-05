#include "utils.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <cstdint>
#include <iostream>

bool checkXZ(FILE *fp)
{
    uint8_t magicCh;
    const uint8_t XZ_HEADER_MAGIC[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    int sig_len = 6;
    for(int i=0;i<sig_len;i++) {
        fscanf(fp, "%c", &magicCh);
        if(magicCh != XZ_HEADER_MAGIC[i]) {
            return false;
        }
    }
    fseek(fp, 0, SEEK_SET);
    return true;
}

int copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
  off_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r < ARCHIVE_OK)
      return (r);
    r = archive_write_data_block(aw, buff, size, offset);
    if (r < ARCHIVE_OK) {
      fprintf(stderr, "%s\n", archive_error_string(aw));
      return (r);
    }
  }
}

bool extractXZ(FILE *sfp, std::string dest)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    if ((r = archive_read_open_FILE(a, sfp))) {
      std::cout<<archive_error_string(ext)<<std::endl;
      exit(1);
    }
    for (;;) {
      r = archive_read_next_header(a, &entry);
      if (r == ARCHIVE_EOF)
        break;
      if (r < ARCHIVE_OK)
        fprintf(stderr, "%s\n", archive_error_string(a));
      if (r < ARCHIVE_WARN)
        exit(1);
      std::string path = archive_entry_pathname(entry);
      //std::cout<<"old path: "<< path<<std::endl;
      archive_entry_set_pathname(entry, (dest + "/" + path).c_str());
      r = archive_write_header(ext, entry);
      if (r < ARCHIVE_OK)
        fprintf(stderr, "%s\n", archive_error_string(ext));
      else if (archive_entry_size(entry) > 0) {
        r = copy_data(a, ext);
        if (r < ARCHIVE_OK)
          fprintf(stderr, "%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
          exit(1);
      }
      r = archive_write_finish_entry(ext);
      if (r < ARCHIVE_OK)
        fprintf(stderr, "%s\n", archive_error_string(ext));
      if (r < ARCHIVE_WARN)
        exit(1);
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    exit(0);
}
