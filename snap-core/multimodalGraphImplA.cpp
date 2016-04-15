/////////////////////////////////////////////////
// Multi-modal Graph, Impl A
TPair<TInt,TInt> TMultimodalGraphImplA::AddNode(const int ModeId) {
  int LocalNId = MxNId;
  MxNId++;
  Network.AddNode(LocalNId, ModeId);
  return TPair<TInt,TInt>(ModeId,LocalNId);
}

int TMultimodalGraphImplA::AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId.GetVal2(), DstNId.GetVal2()).CStr());
  if (IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;
  return Network.AddEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

void TMultimodalGraphImplA::DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId.GetVal2(), DstNId.GetVal2()).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;
  Network.DelEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

TIntNNet TMultimodalGraphImplA::GetSubGraph(const TIntV ModeIds) const {
  TIntNNet SubGraph = TIntNNet();

  for (TIntNNet::TNodeI NI=BegNI(); NI<EndNI(); NI++) {
    if (!ModeIds.IsIn(NI.GetDat())) {
      continue;
    }
    if (!SubGraph.IsNode(NI.GetId())) {
      SubGraph.AddNode(NI.GetId(), NI.GetDat());
    }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      int NeighboringNId = NI.GetOutNId(e);
      int NeighboringModeId = NI.GetOutNDat(e);
      if (!ModeIds.IsIn(NeighboringModeId)) {
        continue;
      }
      if (!SubGraph.IsNode(NeighboringNId)) {
        SubGraph.AddNode(NeighboringNId, NeighboringModeId);
      }
      SubGraph.AddEdge(NI.GetId(), NeighboringNId);
    }
  }

  return SubGraph;
}

PMultimodalGraphImplA TMultimodalGraphImplA::GetSmallGraph() {
  PMultimodalGraphImplA G = TMultimodalGraphImplA::New();
  TVec<TPair< TInt,TInt> > Nodes = TVec< TPair<TInt,TInt> >();
  for (int i = 0; i < 5; i++) { Nodes.Add(G->AddNode(0)); }
  for (int i = 0; i < 5; i++) { Nodes.Add(G->AddNode(1)); }
  G->AddEdge(Nodes.GetVal(0),Nodes.GetVal(1)); G->AddEdge(Nodes.GetVal(1),Nodes.GetVal(2));
  G->AddEdge(Nodes.GetVal(0),Nodes.GetVal(2)); G->AddEdge(Nodes.GetVal(1),Nodes.GetVal(3));
  G->AddEdge(Nodes.GetVal(3),Nodes.GetVal(4)); G->AddEdge(Nodes.GetVal(2),Nodes.GetVal(3));
  G->AddEdge(Nodes.GetVal(2),Nodes.GetVal(6)); G->AddEdge(Nodes.GetVal(4), Nodes.GetVal(8));
  G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(6)); G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(7));
  G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(7)); G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(9));
  G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(8)); G->AddEdge(Nodes.GetVal(8),Nodes.GetVal(9));
  G->AddEdge(Nodes.GetVal(9),Nodes.GetVal(2)); G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(0));
  G->DelNode(Nodes.GetVal(0));
  return G;
}

