// Small example testing basic functionality of SNAP

#include "Snap.h"

int main(int argc, char* argv[]) {
  // create a graph and save it
  PMultimodalGraphImplA Graph = TMultimodalGraphImplA::GetSmallGraph();
  // traverse nodes
  for (TIntNNet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", NI.GetId(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    for (int e = 0; e < NI.GetOutDeg(); e++) { printf("  %d", NI.GetOutNId(e)); }
    printf("\nInNodes: ");
    for (int e = 0; e < NI.GetInDeg(); e++) { printf("  %d", NI.GetInNId(e)); }
    printf("\n\n");
  }

  return 0;
}
