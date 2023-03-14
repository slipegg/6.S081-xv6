#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *name)
{
  if (!strcmp(name, ".") || !strcmp(name, ".."))
  {
    return;
  }

  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // 检查
  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR)
  {
    fprintf(2, "Usage: find <directory> <filename>\n");
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
  {
    fprintf(2, "find: path too long\n");
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf) - 1; // 最后一位不是/就加入/
  if (p[0] != '/')
  {
    p++;
    *p++ = '/'; // p指向/后一位
  }
  else
  {
    p++;
  }
  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;          // 这时p是最新的文件名
    if (stat(buf, &st) < 0) // 由于p的操作导致buf现在是完整的路径
    {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
    {
      find(buf, name); // 由于p的操作导致buf现在是完整的路径
    }
    else if (strcmp(name, p) == 0)
    {
      printf("%s\n", buf);
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(2, "Usage: find <directory> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
