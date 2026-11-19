struct BadNode {
  int payload;
  struct BadNode next;
}; // size(BadNode) = 4 + size(BadNode)

struct GoodNode {
  int payload;
  struct GoodNode* next;
}; // size(GoodNode) = 4 + 8
