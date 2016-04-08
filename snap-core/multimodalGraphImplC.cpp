/////////////////////////////////////////////////
// Multi-modal Graph, Impl C

int TMultimodalGraphImplC::GetNodes() const {
  int NumNodes = 0;
  for (TNodeH::TIter Node=NodeH.BegI(); Node<NodeH.EndI(); Node++) {
    NumNodes += Node.GetDat().Len();
  }
  return NumNodes;
}

int TMultimodalGraphImplC::AddNode(TPair<TInt,TInt> NId) {
  int LocalNId = NId.GetVal2();
  int ModeId = NId.GetVal1();
  if (NId.GetVal2() == -1) {
    LocalNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(LocalNId+1, MxNId());
  }
  if (!NodeH.IsKey(ModeId)) {
    NodeH.AddDat(ModeId);
  }
  NodeH[ModeId].AddDat(LocalNId,TNode(LocalNId));
  return NId;
}

void TMultimodalGraphImplC::DelNode(TPair<TInt,TInt> NId) {
  int LocalNId = NId.GetVal2();
  int ModeId = NId.GetVal1();
  IAssertR(IsNode(NId), TStr::Fmt("NodeId %d doesn't exist", NId));

  TNode& Node = NodeH[ModeId][LocalNId];
  for (int e = 0; e < Node.GetOutDeg(); e++) {
    const TPair<TInt,TInt> nbr = Node.GetOutNId(e);
    if (nbr == NId) { continue; }
    TNode& N = NodeH[nbr.GetVal1()][nbr.GetVal2()];
    const int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
    const TPair<TInt,TInt> nbr = Node.GetInNId(e);
    if (nbr == NId) { continue; }
    TNode& N = NodeH[nbr.GetVal1()][nbr.GetVal2()];
    const int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); }
  }
}

bool TMultimodalGraphImplC::IsNode(TPair<TInt,TInt> NId) {
  int ModeId = NId.GetVal1();
  int LocalNId = NId.GetVal2();
  if (!NodeH.IsKey(ModeId)) {
    return false;
  }
  if (!NodeH[ModeId].IsKey(LocalNId)) {
    return false;
  }
  return true;
}

int TMultimodalGraphImplC::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;
  return Network.AddEdge(SrcNId, DstNId);
}

void TMultimodalGraphImplC::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;
  Network.DelEdge(SrcNId, DstNId);
}

PMultimodalGraphImplA TMultimodalGraphImplC::GetSmallGraph() {
  PMultimodalGraphImplA G = TMultimodalGraphImplC::New();
  for (int i = 0; i < 5; i++) { G->AddNode(i, 0); }
  for (int i = 0; i < 5; i++) { G->AddNode(i, 1); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3); G->AddEdge(3,4); G->AddEdge(2,3);
  G->AddEdge(5,6); G->AddEdge(5,7); G->AddEdge(6,7);
  G->AddEdge(6,9); G->AddEdge(6,8); G->AddEdge(8,9);
  return G;
}

