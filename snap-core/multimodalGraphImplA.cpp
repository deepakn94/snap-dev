/////////////////////////////////////////////////
// Multi-modal Graph, Impl A
int TMultimodalGraphImplA::AddNode(const TPair<TInt,TInt>& NId) {
  int LocalNId = NId.GetVal1();
  if (LocalNId < MxNId) {
    LocalNId = MxNId;
    MxNId++;
  } else {
    MxNId = LocalNId+1;
  }
  Network.AddNode(LocalNId, NId.GetVal1());
  return LocalNId;
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

PMultimodalGraphImplA TMultimodalGraphImplA::GetSmallGraph() {
  PMultimodalGraphImplA G = TMultimodalGraphImplA::New();
  TVec<TPair< TInt,TInt> > Nodes = TVec< TPair<TInt,TInt> >();
  for (int i = 0; i < 5; i++) {
    TPair<TInt,TInt> Node = TPair<TInt,TInt>(0,i);
    int LocalNId = G->AddNode(Node);
    Nodes.Add(TPair<TInt,TInt>(Node.GetVal1(),LocalNId));
  }
  for (int i = 0; i < 5; i++) {
    TPair<TInt,TInt> Node = TPair<TInt,TInt>(1,i);
    int LocalNId = G->AddNode(Node);
    Nodes.Add(TPair<TInt,TInt>(Node.GetVal1(),LocalNId));
  }
  G->AddEdge(Nodes.GetVal(0),Nodes.GetVal(1)); G->AddEdge(Nodes.GetVal(1),Nodes.GetVal(2));
  G->AddEdge(Nodes.GetVal(0),Nodes.GetVal(2)); G->AddEdge(Nodes.GetVal(1),Nodes.GetVal(3));
  G->AddEdge(Nodes.GetVal(3),Nodes.GetVal(4)); G->AddEdge(Nodes.GetVal(2),Nodes.GetVal(3));
  G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(6)); G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(7));
  G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(7)); G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(9));
  G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(8)); G->AddEdge(Nodes.GetVal(8),Nodes.GetVal(9));
  return G;
}

