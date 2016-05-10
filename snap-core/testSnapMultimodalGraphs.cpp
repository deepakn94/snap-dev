// Small example testing basic functionality of SNAP

#include "Snap.h"

void PrintSubGraphs(const TIntNNet& SubGraph) {
  for (TIntNNet::TNodeI NI = SubGraph.BegNI(); NI < SubGraph.EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", NI.GetId(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    for (int e = 0; e < NI.GetOutDeg(); e++) { printf("  %d:%d", (int) NI.GetOutNDat(e), NI.GetOutNId(e)); }
    printf("\nInNodes: ");
    for (int e = 0; e < NI.GetInDeg(); e++) { printf("  %d:%d", (int) NI.GetInNDat(e), NI.GetInNId(e)); }
    printf("\n\n");
  }
}

int main(int argc, char* argv[]) {
  TIntV Modes1 = TIntV(); Modes1.Add(0);
  TIntV Modes2 = TIntV(); Modes2.Add(1);

  // create a graph, impl. A
  printf("Testing Implementation A...\n");
  PMultimodalGraphImplA Graph1 = TMultimodalGraphImplA::GetSmallGraph();
  // traverse nodes
  for (TIntNNet::TNodeI NI = Graph1->BegNI(); NI < Graph1->EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", NI.GetId(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    for (int e = 0; e < NI.GetOutDeg(); e++) { printf("  %d:%d", (int) NI.GetOutNDat(e), NI.GetOutNId(e)); }
    printf("\nInNodes: ");
    for (int e = 0; e < NI.GetInDeg(); e++) { printf("  %d:%d", (int) NI.GetInNDat(e), NI.GetInNId(e)); }
    printf("\n\n");
  }

  printf("Sub-graph composed purely of mode 0:\n");
  TIntNNet SubGraph11 = Graph1->GetSubGraph(Modes1); PrintSubGraphs(SubGraph11);
  printf("Sub-graph composed purely of mode 1:\n");
  TIntNNet SubGraph12 = Graph1->GetSubGraph(Modes2); PrintSubGraphs(SubGraph12);

  // create a graph, impl. B
  printf("Testing Implementation B...\n");
  PMultimodalGraphImplB Graph2 = TMultimodalGraphImplB::GetSmallGraph();
  // traverse nodes
  for (TMultimodalGraphImplB::TNodeI NI = Graph2->BegNI(); NI < Graph2->EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", NI.GetId(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    TIntV AdjacentModes = TIntV();
    NI.GetAdjacentModes(AdjacentModes);
    for (int ModeIdN = 0; ModeIdN < AdjacentModes.Len(); ModeIdN++) {
      int ModeId = AdjacentModes.GetVal(ModeIdN);
      for (int e = 0; e < NI.GetOutDeg(ModeId); e++) {
        printf(" %d:%d", ModeId, NI.GetOutNId(e, ModeId));
      }
    }
    printf("\nInNodes: ");
    for (int ModeIdN = 0; ModeIdN < AdjacentModes.Len(); ModeIdN++) {
      int ModeId = AdjacentModes.GetVal(ModeIdN);
      for (int e = 0; e < NI.GetInDeg(ModeId); e++) {
        printf(" %d:%d", ModeId, NI.GetInNId(e, ModeId));
      }
    }
    printf("\n\n");
  }

  printf("Sub-graph composed purely of mode 0:\n");
  TIntNNet SubGraph21 = Graph2->GetSubGraph(Modes1); PrintSubGraphs(SubGraph21);
  printf("Sub-graph composed purely of mode 1:\n");
  TIntNNet SubGraph22 = Graph2->GetSubGraph(Modes2); PrintSubGraphs(SubGraph22);

  // create a graph, impl. C
  printf("Testing Implementation C...\n");
  PMultimodalGraphImplC Graph3 = TMultimodalGraphImplC::GetSmallGraph();
  // traverse nodes
  for (TMultimodalGraphImplC::TNodeI NI = Graph3->BegNI(); NI < Graph3->EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", (int) NI.GetId().GetVal2(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      printf("  %d:%d", (int) NI.GetOutNId(e).GetVal1(), (int) NI.GetOutNId(e).GetVal2());
    }
    printf("\nInNodes: ");
    for (int e = 0; e < NI.GetInDeg(); e++) {
      printf("  %d:%d", (int) NI.GetInNId(e).GetVal1(), (int) NI.GetInNId(e).GetVal2());
    }
    printf("\n\n");
  }

  printf("Sub-graph composed purely of mode 0:\n");
  TIntNNet SubGraph31 = Graph3->GetSubGraph(Modes1); PrintSubGraphs(SubGraph31);
  printf("Sub-graph composed purely of mode 1:\n");
  TIntNNet SubGraph32 = Graph3->GetSubGraph(Modes2); PrintSubGraphs(SubGraph32);

  // create a graph, impl. BC
  printf("Testing implementation BC...\n");
  PMultimodalGraphImplBC Graph4 = TMultimodalGraphImplBC::GetSmallGraph();
  // traverse nodes
  for (TMultimodalGraphImplBC::TNodeI NI = Graph4->BegNI(); NI < Graph4->EndNI(); NI++) {
    printf("NodeId: %d, InDegree: %d, OutDegree: %d\n", (int) NI.GetId().GetVal2(), NI.GetInDeg(), NI.GetOutDeg());
    printf("OutNodes: ");
    TIntV AdjacentModes = TIntV();
    NI.GetAdjacentModes(AdjacentModes);
    for (int ModeIdN = 0; ModeIdN < AdjacentModes.Len(); ModeIdN++) {
      int ModeId = AdjacentModes.GetVal(ModeIdN);
      for (int e = 0; e < NI.GetOutDeg(ModeId); e++) {
        printf(" %d:%d", ModeId, (int) NI.GetOutNId(e, ModeId));
      }
    }
    printf("\nInNodes: ");
    for (int ModeIdN = 0; ModeIdN < AdjacentModes.Len(); ModeIdN++) {
      int ModeId = AdjacentModes.GetVal(ModeIdN);
      for (int e = 0; e < NI.GetInDeg(ModeId); e++) {
        printf(" %d:%d", ModeId, (int) NI.GetInNId(e, ModeId));
      }
    }
    printf("\n\n");
  }

  printf("Sub-graph composed purely of mode 0:\n");
  TIntNNet SubGraph41 = Graph4->GetSubGraph(Modes1); PrintSubGraphs(SubGraph41);
  printf("Sub-graph composed purely of mode 1:\n");
  TIntNNet SubGraph42 = Graph4->GetSubGraph(Modes2); PrintSubGraphs(SubGraph42);

  return 0;
}
