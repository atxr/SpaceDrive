#include <stdio.h>
#include <strings.h>

#include "libmineziper_huffman_tree.h"
#include "libmineziper_zip.h"

int main(int argc, char** argv)
{
  tree def = build_default_tree();
  print_huffman_tree(def);

  return 0;
}
